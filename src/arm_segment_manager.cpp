#include "arm_segment_manager.hpp"

#include <iostream>
#include <filesystem>

#include "imgui/imgui.h"


ArmSegmentManager::ArmSegmentManager():
base{1, 0.1f, 0.0f, 1, 1, {0.0f,1.0f,0.0f}},fileUI{FILE_UI::LOAD}{
    std::filesystem::path persistent = std::filesystem::current_path().append("persistent.dat");
    std::ifstream persistentFile{persistent};

    memset(this->folderpath, 0, 1024);
    this->folderpath[0] = '/';

    if(persistentFile.is_open()){
        std::string filePath;
        persistentFile >> filePath;
        memcpy(this->folderpath, filePath.c_str(), std::min(filePath.length(),ArmSegmentManager::FOLDER_PATH_SIZE));
    }
}
ArmSegmentManager::~ArmSegmentManager(){
    std::filesystem::path persistent = std::filesystem::current_path().append("persistent.dat");
    std::ofstream persistentFile{persistent};
    persistentFile << std::string(this->folderpath);
}
void ArmSegmentManager::renderUI(){
    ImGui::Begin("Arm", nullptr, ImGuiWindowFlags_MenuBar);

    if(ImGui::BeginMenuBar()){
        if(ImGui::BeginMenu("File")){
            if(ImGui::MenuItem("Save")){
                this->fileUI = FILE_UI::SAVE;
            }
            if(ImGui::MenuItem("Load")){
                this->fileUI = FILE_UI::LOAD;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();        
    }

    switch (this->fileUI) {
        case FILE_UI::NONE:{}break;
        case FILE_UI::SAVE:{
            ImGui::Begin("Save");
            ImGui::InputText("File path", this->folderpath, ArmSegmentManager::FOLDER_PATH_SIZE);
            if(ImGui::Button("Refresh")){
                this->fileNames.empty();
                try{
                    for(const auto &entry : std::filesystem::directory_iterator(this->folderpath)){
                        if(entry.is_regular_file() && entry.path().filename().extension() == ".arm"){
                            this->fileNames.push_back(entry.path().filename().string());
                        }
                    }
                }catch(const std::filesystem::filesystem_error &e){
                    std::cerr << e.what() << std::endl;
                }catch(const std::exception &e){
                    std::cerr << e.what() << std::endl;
                }
            }
            ImGui::BeginChild("Files");
            for(const auto & fname : this->fileNames){
                if(ImGui::Selectable(fname.c_str(),fname == this->selectedFileName)){
                    this->selectedFileName = fname;
                }
            }
            ImGui::EndChild();
            ImGui::End();
        }break;
        case FILE_UI::LOAD:{
            ImGui::Begin("Load");
            ImGui::InputText("File path", this->folderpath, ArmSegmentManager::FOLDER_PATH_SIZE);
            ImGui::BeginChild("Files");
            ImGui::EndChild();
            ImGui::End();
        }break;
    }

    if(ImGui::Button("Add Arm Segment")){
        this->base.addSegment(1.0f, 0.05f, 0.0f, 1.0f, 1.0f);
    }

    {
        int index = 0;
        ArmSegment *seg = &this->base;
        while(seg != nullptr){
            seg->renderUI(std::to_string(index++));
            seg = seg->getNext();
        }
    }

    ImGui::End();
}
glm::mat4 ArmSegmentManager::render(NRA::VGL::Shader &shader, glm::mat4 mat){
    ArmSegment *seg = &this->base;
    while(seg != nullptr){
        mat = seg->render(shader, "U_mMat", mat);
        seg = seg->getNext();
    }
    return mat;
}