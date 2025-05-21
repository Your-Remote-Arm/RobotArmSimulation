#include "arm_segment.hpp"

#include "NRA_visionGL/mesh.h"
#include "NRA_visionGL/meshbuilder.h"

#include "imgui/imgui.h"

struct vertex{
    GLfloat pos[3];
    GLfloat norm[3];
};

ArmSegment::ArmSegment(float length, float radius, float torque, float mass, ArmSegment *previous):
size{length,radius},torque{torque},mass{mass},previous{previous},next{nullptr},begin{},end{},renderable{}{
    if(this->previous != nullptr){
        this->begin.changeParent(&this->previous->end);
    }
    this->end.changeParent(&this->begin);
    this->end.moveA({0,this->size[0],0});
    
    this->renderable.init();
    this->renderable.setVBOLayout(ArmSegment::layout);


    NRA::VGL::MeshBuilder<GLuint>::Parameters p = {
        20,
        NRA::VGL::MeshBuilder<GLuint>::Parameters::NORMAL_BIT,
        0, 0, offsetof(vertex, norm),
        {0,0,this->size[0]/2}
    };
    NRA::VGL::Mesh<GLuint> mesh{sizeof(vertex)/4};

    NRA::VGL::MeshBuilder<GLuint>::createCylinder(mesh, p, this->size[1], this->size[0]);

    this->renderable.loadMesh(mesh);
}

void ArmSegment::render(NRA::VGL::Shader &shader, std::string modelMatName){
    glm::mat4 modelMat = glm::mat4(1.0f);
    this->begin.transformR(modelMat);
    shader.setUniformMat<4>(modelMatName, &modelMat[0][0]);
    this->renderable.render();
}
void ArmSegment::renderUI(std::string label){
    std::string l = "Length / Radius " + label;
    if(ImGui::SliderFloat2(l.c_str(),this->size,0.001f,100.0f,"%.3f")){
        NRA::VGL::MeshBuilder<GLuint>::Parameters p = {
            20,
            NRA::VGL::MeshBuilder<GLuint>::Parameters::NORMAL_BIT,
            0, 0, offsetof(vertex, norm),
            {0,0,this->size[0]/2}
        };
        NRA::VGL::Mesh<GLuint> mesh{sizeof(vertex)/4};
        
        NRA::VGL::MeshBuilder<GLuint>::createCylinder(mesh, p, this->size[1], this->size[0]);
        
        this->renderable.loadMesh(mesh);
        this->end.setZero();
        this->end.moveA({0,this->size[0]/2,0});
    }
}
void ArmSegment::renderUIR(std::size_t index){
    this->renderUI(std::to_string(index));
    if(this->next != nullptr){
        this->next->renderUIR(index+1);
    }
}
void ArmSegment::addSegment(float length, float radius, float torque, float mass){
    if(this->next == nullptr){
        this->next = new ArmSegment(length, radius, torque, mass, this);
    }else{
        this->next->addSegment(length, radius, torque, mass);
    }
}

void ArmSegment::initLayout(){
    // pos
    ArmSegment::layout.push(GL_FLOAT,3);
    // normal
    ArmSegment::layout.push(GL_FLOAT,3);
}
NRA::VGL::VertexBufferLayout ArmSegment::layout{};