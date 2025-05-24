#pragma once

#include "NRA_visionGL/shader.h"

#include "arm_segment.hpp"

class ArmSegmentManager{
private:
    enum class FILE_UI : uint8_t{
        NONE,
        SAVE,
        LOAD
    };
    ArmSegment base;
    FILE_UI fileUI;
    static constexpr std::size_t FOLDER_PATH_SIZE = 1023;
    char folderpath[FOLDER_PATH_SIZE+1];
    std::string selectedFileName;
    std::vector<std::string> fileNames;
public:
    ArmSegmentManager();
    ~ArmSegmentManager();
    void renderUI();
    glm::mat4 render(NRA::VGL::Shader &shader, glm::mat4 mat = glm::mat4(1.0f));
};