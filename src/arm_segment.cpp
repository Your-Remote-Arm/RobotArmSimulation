#include "arm_segment.hpp"

#include "NRA_visionGL/mesh.h"
#include "NRA_visionGL/meshbuilder.h"

ArmSegment::ArmSegment(float length, float radius, float torque, float mass, ArmSegment *previous):
length{length},radius{radius},torque{torque},mass{mass},previous{previous},next{nullptr},renderable{}{
    this->renderable.init();
    this->renderable.setVBOLayout(ArmSegment::layout);

    struct vertex{
        GLfloat pos[3];
        GLfloat norm[3];
    };

    NRA::VGL::MeshBuilder<GLuint>::Parameters p = {
        20,
        NRA::VGL::MeshBuilder<GLuint>::Parameters::NORMAL_BIT,
        0, 0, offsetof(vertex, norm),
        {0,0,0}
    };
    NRA::VGL::Mesh<GLuint> mesh{sizeof(vertex)/4};

    NRA::VGL::MeshBuilder<GLuint>::createCylinder(mesh, p, this->radius, this->length);

    this->renderable.loadMesh(mesh);
}

void ArmSegment::render(){
    this->renderable.render();
}

void ArmSegment::initLayout(){
    // pos
    ArmSegment::layout.push(GL_FLOAT,3);
    // normal
    ArmSegment::layout.push(GL_FLOAT,3);
}
NRA::VGL::VertexBufferLayout ArmSegment::layout{};