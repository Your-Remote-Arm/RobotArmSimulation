#pragma once
#include <string>

#include "NRA_visionGL/spacial.h"
#include "NRA_visionGL/renderable.h"
#include "NRA_visionGL/shader.h"

class ArmSegment{
private:
    static NRA::VGL::VertexBufferLayout layout;
    // Use SI units: meters, meters, newton meters, kg. SIze is length, radius
    float size[2], torque, mass;
    ArmSegment *previous, *next;
    NRA::VGL::SpacialTree begin, end;
    NRA::VGL::Renderable renderable;
public:
    static void initLayout();
    ArmSegment(float length, float radius, float torque, float mass, ArmSegment *previous = nullptr);
    void step();
    void setTorque();
    void getTransform(glm::mat4 &mat);
    void render(NRA::VGL::Shader &shader, std::string modelMatName);
    void renderR(NRA::VGL::Shader &shader, std::string modelMatName, glm::mat4 mat);
    void renderUI(std::string label);
    void renderUIR(std::size_t index = 0);
    void addSegment(float length, float radius, float torque, float mass);
    inline ArmSegment *getNext(){return this->next;}
    inline ArmSegment *getPrevious(){return this->previous;}
};