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

ArmSegment::ArmSegment(float length, float radius, float angle, float torque, float mass, glm::vec3 axis, ArmSegment *previous):
size{length,radius},angle{angle},torque{torque},mass{mass},axis{axis},displacement{0,this->size[0],0},previous{previous},next{nullptr},renderable{},detail{5}{
    this->renderable.init();
    this->renderable.setVBOLayout(ArmSegment::layout);

    MeshBuilder_t::Parameters p = {
        this->detail,
        MeshBuilder_t::Parameters::NORMAL_BIT,
        0, 0, offsetof(vertex, norm),
        {0,this->size[0]/2,0},
        MeshBuilder_t::Parameters::SPHERE_TYPE::UV,
        MeshBuilder_t::Parameters::CAP_TYPE::SPHERE,
        MeshBuilder_t::Parameters::CAP_TYPE::SPHERE
    };
    NRA::VGL::Mesh<GLuint> mesh{sizeof(vertex)/4};

    MeshBuilder_t::createCylinder(mesh, p, this->size[1], this->size[0]);

    this->renderable.loadMesh(mesh);
}
void ArmSegment::addSegment(float length, float radius, float angle, float torque, float mass, glm::vec3 axis){
    if(this->next == nullptr){
        this->next = new ArmSegment(length, radius, angle, torque, mass, axis, this);
    }else{
        this->next->addSegment(length, radius, angle, torque, mass, axis);
    }
}

glm::mat4 ArmSegment::render(NRA::VGL::Shader &shader, std::string modelMatName, glm::mat4 mat){
    shader.setUniformMat<4>(modelMatName, &mat[0][0]);
    this->renderable.render();
    mat = mat * glm::translate(glm::mat4(1.0f), this->displacement);
    mat = mat * glm::rotate(glm::mat4(1.0f), this->angle, this->axis);
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
    if(ImGui::SliderFloat2(l.c_str(),this->size,0.01f,10.0f,"%.2f")){
        MeshBuilder_t::Parameters p = {
            this->detail,
            MeshBuilder_t::Parameters::NORMAL_BIT,
            0, 0, offsetof(vertex, norm),
            {0,this->size[0]/2,0},
            MeshBuilder_t::Parameters::SPHERE_TYPE::UV,
            MeshBuilder_t::Parameters::CAP_TYPE::SPHERE,
            MeshBuilder_t::Parameters::CAP_TYPE::SPHERE
        };
        NRA::VGL::Mesh<GLuint> mesh{sizeof(vertex)/4};
        
        MeshBuilder_t::createCylinder(mesh, p, this->size[1], this->size[0]);
        
        this->renderable.loadMesh(mesh);

        this->displacement.y = this->size[0];
    }
    l = "Axis " + label;
    if(ImGui::SliderFloat3(l.c_str(), &this->axis[0],-1.0f,1.0f,"%.4f")){
    }
    l = "Angle " + label;
    if(ImGui::SliderAngle(l.c_str(), &this->angle)){
    }
    ImGui::EndChild();
}
void ArmSegment::renderUIR(std::size_t index){
    this->renderUI(std::to_string(index));
    if(this->next != nullptr){
        this->next->renderUIR(index+1);
    }
}

void ArmSegment::save(std::ofstream fileStream){

}
void ArmSegment::load(std::ifstream fileStream){

}

void ArmSegment::initLayout(){
    // pos
    ArmSegment::layout.push(GL_FLOAT,3);
    // normal
    ArmSegment::layout.push(GL_FLOAT,3);
}
NRA::VGL::VertexBufferLayout ArmSegment::layout{};