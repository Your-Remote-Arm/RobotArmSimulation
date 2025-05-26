#include "arm_segment_manager.hpp"

#include <iostream>
#include <filesystem>

bool ArmSegmentManager::FileProperties::operator==(const FileProperties &other) const{
    return this->name == other.name && this->extension == other.extension && this->type == other.type;
}

ArmSegmentManager::ArmSegmentManager():
base{1, 0.1f, 0.0f, 1, 1, {0.0f,1.0f,0.0f}},fileUI{FILE_UI::LOAD},folderPathBuff{nullptr},folderPathBuffSize{0},selectedFile{"","",FILE_TYPE::FOLDER},showAllFiles{false},showExtensions{false},showHidden{false},forceRefresh{true}{
    std::filesystem::path persistent = std::filesystem::current_path().append("persistent.dat");
    std::ifstream persistentFile{persistent};

    if(persistentFile.is_open()){
        std::string filePath;
        persistentFile >> filePath;
        this->folderPath = std::filesystem::path(filePath);
        this->folderPathBuffSize = std::max(this->folderPath.string().length()+64,(std::size_t)1023);
        this->folderPathBuff = new char[this->folderPathBuffSize+1];
        memset(this->folderPathBuff,0,this->folderPathBuffSize+1);
        memcpy(this->folderPathBuff,filePath.c_str(),filePath.length());
    }else{
        this->folderPathBuffSize = 1023;
        this->folderPathBuff = new char[this->folderPathBuffSize+1];
        memset(this->folderPathBuff,0,this->folderPathBuffSize+1);
        this->folderPathBuff[0] = '.';
        this->folderPathBuff[1] = '.';
        this->folderPathBuff[2] = '/';
        this->folderPathBuff[3] = '.';
        this->folderPathBuff[4] = '.';
        this->folderPathBuff[5] = '/';
    }
}
ArmSegmentManager::~ArmSegmentManager(){
    std::filesystem::path persistent = std::filesystem::current_path().append("persistent.dat");
    std::ofstream persistentFile{persistent};
    persistentFile << std::string(this->folderPath.string()) << '\n';

    delete[] this->folderPathBuff;
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
            this->forceRefresh |= this->filePathEditor();
            if(ImGui::Button("Create Folder")){
                std::filesystem::create_directory(this->folderPath);
            }
            if(ImGui::Button("Cancel")){
                this->fileUI = FILE_UI::NONE;
            }
            ImGui::SameLine();
            this->fileRefreshButton(this->forceRefresh);
            ImGui::SameLine();
            auto res = this->showFiles();
            this->forceRefresh = res[1];
            if(res[0]){
                try{
                    std::ofstream fout{this->folderPath, std::ios::binary};
                    std::size_t segCount = 0;
                    ArmSegment *seg = &this->base;
                    while(seg != nullptr){
                        ++segCount;
                        seg = seg->getNext();
                    }
                    fout.write((char*)&segCount,sizeof(std::size_t));
                    seg = &this->base;
                    while(seg != nullptr){
                        seg->save(fout);
                        seg = seg->getNext();
                    }
                    fout.close();
                }catch(std::filesystem::filesystem_error &e){
                    std::cerr << e.what() << std::endl;
                }
                this->fileUI = FILE_UI::NONE;
                this->forceRefresh = true;
            }
            ImGui::End();
        }break;
        case FILE_UI::LOAD:{
            ImGui::Begin("Load");
            this->forceRefresh |= this->filePathEditor();
            if(ImGui::Button("Cancel")){
                this->fileUI = FILE_UI::NONE;
            }
            ImGui::SameLine();
            this->fileRefreshButton(this->forceRefresh);
            ImGui::SameLine();
            auto res = this->showFiles();
            this->forceRefresh = res[1];
            if(res[0]){
                try{
                    std::ifstream fin{this->folderPath, std::ios::binary};
                    std::size_t segCount;
                    fin.read((char*)&segCount,sizeof(std::size_t));

                    ArmSegment::Serialized data;
                    this->base.load(fin);
                    for(std::size_t i=1;i<segCount;++i){
                        fin.read(data,sizeof(ArmSegment::Serialized));
                        this->base.addSegment(data.size[0],data.size[1],data.angle,data.torque,data.mass,data.axis);
                    }
                    
                    fin.close();
                }catch(std::filesystem::filesystem_error &e){
                    std::cerr << e.what() << std::endl;
                }

                this->fileUI = FILE_UI::NONE;
                this->forceRefresh = true;
            }
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

void ArmSegmentManager::updateFolderPathBuff(){
    if(this->folderPath.string().length() > this->folderPathBuffSize){
        this->folderPathBuffSize = this->folderPath.string().length() + 64;
        delete[] this->folderPathBuff;
        this->folderPathBuff = new char[this->folderPathBuffSize+1];
    }
    memset(this->folderPathBuff,0,this->folderPathBuffSize+1);
    memcpy(this->folderPathBuff,this->folderPath.c_str(),std::max(this->folderPath.string().length(),this->folderPathBuffSize));
}
bool ArmSegmentManager::filePathEditor(){
    bool res = false;
    if(ImGui::Button("<")){
        res = true;
        if(this->folderPath.string()==".." || this->folderPath.string()=="../"){
            this->folderPath = std::filesystem::current_path();
        }
        this->folderPath = this->folderPath.parent_path();
        this->updateFolderPathBuff();
    }
    ImGui::SameLine();
    res |= ImGui::InputText("File path", this->folderPathBuff, this->folderPathBuffSize);
    if(this->folderPathBuff[this->folderPathBuffSize-1] != '\0'){
        this->folderPathBuffSize += 64;
        char *newBuff = new char[this->folderPathBuffSize+1];
        memcpy(newBuff,this->folderPathBuff,this->folderPathBuffSize+1);
        delete[] this->folderPathBuff;
        this->folderPathBuff = newBuff;
    }

    return res;
}
void ArmSegmentManager::fileRefreshButton(bool force){
    if(ImGui::Button("Refresh") || force){
        this->folderPath = std::filesystem::path(this->folderPathBuff);
        this->files.clear();
        try{
            for(const auto &entry : std::filesystem::directory_iterator(this->folderPath)){
                std::string filename = entry.path().filename().stem().string();
                std::string fileextension = "";
                FILE_TYPE fType = FILE_TYPE::FOLDER;
                if(entry.is_regular_file()){
                    fileextension = entry.path().filename().extension().string();
                    if(fileextension == ".arm"){
                        fType = FILE_TYPE::ARM_FILE;
                    }else{
                        fType = FILE_TYPE::OTHER_FILE;
                    }
                }

                this->files.push_back({filename, fileextension, fType});
            }
        }catch(const std::filesystem::filesystem_error &e){
            //std::cerr << e.what() << std::endl;
        }catch(const std::exception &e){
            //std::cerr << e.what() << std::endl;
        }
    }
}
std::array<bool,2> ArmSegmentManager::showFiles(){
    bool confirmed = ImGui::Button("Confirm");
    ImGui::BeginChild("Files");
    ImGui::Checkbox("Show All Files",&this->showAllFiles);      ImGui::SameLine();
    ImGui::Checkbox("Show Extensions",&this->showExtensions);   ImGui::SameLine();
    ImGui::Checkbox("Show Hidden",&this->showHidden);
    ImGui::BeginChild("FileList");
    for(const auto & f : this->files){
        if((this->showHidden || f.name.at(0) != '.') && (this->showAllFiles || f.type == FILE_TYPE::ARM_FILE || f.type == FILE_TYPE::FOLDER)){
            std::string fname = this->showExtensions ? f.fullName() : f.name;
            ImGui::PushStyleColor(ImGuiCol_Text, ArmSegmentManager::fileTypeColors[(uint_fast8_t)f.type][0]);
            if(ImGui::Selectable(fname.c_str(),(f == this->selectedFile))){
                this->selectedFile = f;
            }
            ImGui::PopStyleColor(1);
        }
    }
    ImGui::EndChild();
    ImGui::EndChild();
    if(confirmed){
        if(this->selectedFile.name != "" && this->selectedFile.type == FILE_TYPE::FOLDER){
            this->folderPath.append(this->selectedFile.name);
            this->updateFolderPathBuff();
            this->selectedFile = {"","",FILE_TYPE::FOLDER};
            return {false,true};
        }else if(this->selectedFile.name.length() > 0 && this->selectedFile.type != FILE_TYPE::FOLDER){
            this->folderPath.append(this->selectedFile.fullName());
            this->updateFolderPathBuff();
            return {true,false};
        }else{
            return {true,false};
        }
    }else{
        return {false,false};
    }
}