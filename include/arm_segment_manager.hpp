#pragma once

#include "NRA_visionGL/shader.h"

#include "arm_segment.hpp"

class ArmSegmentManager{
private:
    enum class FILE_UI : uint8_t{
        NONE = 0,
        SAVE,
        LOAD
    };
    enum class FILE_TYPE : uint8_t{
        FOLDER = 0,
        ARM_FILE,
        OTHER_FILE
    };
    struct FileProperties{
        std::string name;
        std::string extension;
        FILE_TYPE type;
        inline std::string fullName() const {return this->name + this->extension;};
        bool operator==(const FileProperties &other) const;
    };

    ArmSegment base;

    // Properties related to UI
    FILE_UI fileUI;
    std::filesystem::path folderPath;
    std::size_t folderPathBuffSize;
    char *folderPathBuff;
    FileProperties selectedFile;
    bool showAllFiles;
    bool showExtensions;
    bool showHidden;
    bool forceRefresh;
    std::vector<FileProperties> files;
public:
    ArmSegmentManager();
    ~ArmSegmentManager();
    void renderUI();
    glm::mat4 render(NRA::VGL::Shader &shader, glm::mat4 mat = glm::mat4(1.0f));
private:
    void updateFolderPathBuff();
    bool filePathEditor();
    void fileRefreshButton(bool force);
    std::array<bool,2> showFiles();
};