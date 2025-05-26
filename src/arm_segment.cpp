#include "arm_segment.hpp"

#include "NRA_visionGL/mesh.h"
#include "NRA_visionGL/meshbuilder.h"

#include "imgui/imgui.h"
#include "glm/ext/matrix_transform.hpp"

struct vertex{
    GLfloat pos[3];
    GLfloat norm[3];
};
typedef NRA::VGL::MeshBuilder<GLuint> MeshBuilder_t;

void ArmSegment::updateMesh(){
    MeshBuilder_t::Parameters p = {
        this->serialized.detail,
        MeshBuilder_t::Parameters::NORMAL_BIT,
        0, 0, offsetof(vertex, norm),
        {0,this->serialized.size[0]/2,0},
        MeshBuilder_t::Parameters::SPHERE_TYPE::UV,
        MeshBuilder_t::Parameters::CAP_TYPE::SPHERE,
        MeshBuilder_t::Parameters::CAP_TYPE::SPHERE
    };
    NRA::VGL::Mesh<GLuint> mesh{sizeof(vertex)/4};

    MeshBuilder_t::createCylinder(mesh, p, this->serialized.size[1], this->serialized.size[0]);

    this->renderable.loadMesh(mesh);
}

ArmSegment::ArmSegment(float length, float radius, float angle, float torque, float mass, glm::vec3 axis, ArmSegment *previous):
serialized{{length,radius},angle,torque,mass,axis,5},displacement{0,this->serialized.size[0],0},previous{previous},next{nullptr},renderable{}{
    this->renderable.init();
    this->renderable.setVBOLayout(ArmSegment::layout);

    this->updateMesh();
}
ArmSegment::~ArmSegment(){
    if(this->next != nullptr){
        delete this->next;
    }
}
void ArmSegment::addSegment(float length, float radius, float angle, float torque, float mass, glm::vec3 axis){
    if(this->next == nullptr){
        this->next = new ArmSegment(length, radius, angle, torque, mass, axis, this);
    }else{
        this->next->addSegment(length, radius, angle, torque, mass, axis);
    }
}
void ArmSegment::removeNext(){
    if(this->next != nullptr){
        this->next->removeNext();
        delete this->next;
        this->next = nullptr;
    }
}

glm::mat4 ArmSegment::render(NRA::VGL::Shader &shader, std::string modelMatName, glm::mat4 mat){
    shader.setUniformMat<4>(modelMatName, &mat[0][0]);
    this->renderable.render();
    mat = mat * glm::translate(glm::mat4(1.0f), this->displacement);
    mat = mat * glm::rotate(glm::mat4(1.0f), this->serialized.angle, this->serialized.axis);
    return mat;
}
glm::mat4 ArmSegment::renderR(NRA::VGL::Shader &shader, std::string modelMatName, glm::mat4 mat){
    mat = this->render(shader, modelMatName, mat);
    if(this->next != nullptr){
        mat = this->next->renderR(shader, modelMatName, mat);
    }
    return mat;
}
void ArmSegment::renderUI(std::string label){
    ImGui::BeginChild(label.c_str(), {500,100});
    std::string l = "Length / Radius " + label;
    if(ImGui::SliderFloat2(l.c_str(),this->serialized.size,0.01f,10.0f,"%.2f")){
        this->updateMesh();

        this->displacement.y = this->serialized.size[0];
    }
    l = "Axis " + label;
    if(ImGui::SliderFloat3(l.c_str(), &this->serialized.axis[0],-1.0f,1.0f,"%.4f")){
    }
    l = "Angle " + label;
    if(ImGui::SliderAngle(l.c_str(), &this->serialized.angle)){
    }
    ImGui::EndChild();
}
void ArmSegment::renderUIR(std::size_t index){
    this->renderUI(std::to_string(index));
    if(this->next != nullptr){
        this->next->renderUIR(index+1);
    }
}

void ArmSegment::save(std::ofstream &fileStream){
    fileStream.write(this->serialized,sizeof(this->serialized));
}
void ArmSegment::load(std::ifstream &fileStream){
    this->removeNext();
    fileStream.read(this->serialized,sizeof(this->serialized));
    this->updateMesh();
    this->displacement.y = this->serialized.size[0];
}

void ArmSegment::initLayout(){
    // pos
    ArmSegment::layout.push(GL_FLOAT,3);
    // normal
    ArmSegment::layout.push(GL_FLOAT,3);
}
NRA::VGL::VertexBufferLayout ArmSegment::layout{};