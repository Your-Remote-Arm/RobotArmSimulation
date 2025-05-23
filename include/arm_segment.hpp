#pragma once
#include <string>

#include "NRA_visionGL/renderable.h"
#include "NRA_visionGL/shader.h"

class ArmSegment{
private:
    static NRA::VGL::VertexBufferLayout layout;
    // Use SI units: meters, meters, newton meters, kg. SIze is length, radius
    float size[2], angle, torque, mass;
    glm::vec3 axis;
    glm::vec3 displacement;
    ArmSegment *previous, *next;
    NRA::VGL::Renderable renderable;
public:
    static void initLayout();
    ArmSegment(float length, float radius, float angle, float torque, float mass, glm::vec3 axis = glm::vec3(1.0f,0.0f,0.0f), ArmSegment *previous = nullptr);
    void step();
    void setTorque();
    void getTransform(glm::mat4 &mat);
    glm::mat4 render(NRA::VGL::Shader &shader, std::string modelMatName, glm::mat4 mat = glm::mat4(1.0f));
    glm::mat4 renderR(NRA::VGL::Shader &shader, std::string modelMatName, glm::mat4 mat = glm::mat4(1.0f));
    void renderUI(std::string label);
    void renderUIR(std::size_t index = 0);
    void addSegment(float length, float radius, float angle, float torque, float mass, glm::vec3 axis = glm::vec3{1.0f,0.0f,0.0f});
    inline ArmSegment *getNext(){return this->next;}
    inline ArmSegment *getPrevious(){return this->previous;}
};