#pragma once

#include "NRA_visionGL/spacial.h"
#include "NRA_visionGL/renderable.h"

class ArmSegment{
private:
    static NRA::VGL::VertexBufferLayout layout;
    // Use SI units: meters, meters, newton meters, kg
    float length, radius, torque, mass;
    ArmSegment *previous;
    ArmSegment *next;
    NRA::VGL::SpacialTree position;
    NRA::VGL::Renderable renderable;
public:
    static void initLayout();
    ArmSegment(float length, float radius, float torque, float mass, ArmSegment *previous = nullptr);
    void renderUI();
    void step();
    void setTorque();
    void render();
};