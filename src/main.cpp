#include <string>
#include <iostream>
#include <cstddef>

#include "NRA_visionGL/config.h"
#include "NRA_visionGL/window.h"
#include "NRA_visionGL/shader.h"
#include "NRA_visionGL/renderable.h"
#include "NRA_visionGL/mesh.h"
#include "NRA_visionGL/meshbuilder.h"
#include "NRA_visionGL/controls.h"
#include "NRA_visionGL/camera.h"
#include "NRA_visionGL/spacial.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/backends/imgui_impl_glfw.h"

const int worldWidth = 10, worldHeight = 10;

namespace TestControls{
    // First block (0 - 31):    Movement
    const NRA::VGL::ControlBind forwards =      {false, 0};
    const NRA::VGL::ControlBind backwards =     {false, 1};
    const NRA::VGL::ControlBind left =          {false, 2};
    const NRA::VGL::ControlBind right =         {false, 3};
    const NRA::VGL::ControlBind up =            {false, 4};
    const NRA::VGL::ControlBind down =          {false, 5};
    // Second block (32 - 63):  Admin
    const NRA::VGL::ControlBind pause =         {false, 32};
};

int main(){
    std::cout << "NRA_visionGL test v" << (std::string)NRA_visionGL_VERSION << std::endl;

    NRA::VGL::Window::init();

    std::vector<NRA::VGL::ControlsInit> controlsList;
    {
        using namespace NRA;
        using namespace VGL;
        using namespace TestControls;
        controlsList.emplace_back(ControlsInit{ButtonType::KEY,GLFW_KEY_W,          forwards});
        controlsList.emplace_back(ControlsInit{ButtonType::KEY,GLFW_KEY_S,          backwards});
        controlsList.emplace_back(ControlsInit{ButtonType::KEY,GLFW_KEY_A,          left});
        controlsList.emplace_back(ControlsInit{ButtonType::KEY,GLFW_KEY_D,          right});
        controlsList.emplace_back(ControlsInit{ButtonType::KEY,GLFW_KEY_SPACE,      up});
        controlsList.emplace_back(ControlsInit{ButtonType::KEY,GLFW_KEY_LEFT_SHIFT, down});
        controlsList.emplace_back(ControlsInit{ButtonType::KEY,GLFW_KEY_ESCAPE,     pause});
    }

    NRA::VGL::Window window(1080,720,"NRA vision GL test",controlsList);
    NRA::VGL::Controls &controls = window.getControls();
    window.makeCurrent();
    window.swapInterval(1);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glClearColor(0.0f, 0.5f, 0.8f, 1.0f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();  // or StyleColorsClassic(), StyleColorsLight()

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    std::filesystem::path shaderPath = std::filesystem::current_path().append("submodules/NRA_visionGL/res/shaders/");
    std::filesystem::path vertexPath = shaderPath;
    std::filesystem::path fragmentPath = shaderPath;
    vertexPath.append("solid/solid.vertex");
    fragmentPath.append("solid/solid.fragment");

    std::filesystem::path vertexPathN = shaderPath;
    std::filesystem::path fragmentPathN = shaderPath;
    vertexPathN.append("point_light/point_light.vertex");
    fragmentPathN.append("point_light/point_light.fragment");

    NRA::VGL::Shader shader(vertexPath,fragmentPath);
    NRA::VGL::Shader shaderN(vertexPathN,fragmentPathN);

    NRA::VGL::Renderable renderable;
    renderable.init();
    NRA::VGL::Renderable renderableN;
    renderableN.init();

    NRA::VGL::VertexBufferLayout layout;
    // pos
    layout.push(GL_FLOAT,3);

    NRA::VGL::VertexBufferLayout layoutN;
    // pos
    layoutN.push(GL_FLOAT,3);
    // normal
    layoutN.push(GL_FLOAT,3);

    renderable.setVBOLayout(layout);
    renderableN.setVBOLayout(layoutN);

    typedef NRA::VGL::Mesh<GLuint> Mesh_t;
    typedef NRA::VGL::MeshBuilder<GLuint> MeshBuilder_t;
    struct vertex{
        GLfloat pos[3];
    };
    struct vertexN{
        GLfloat pos[3];
        GLfloat n[3];
    };
    MeshBuilder_t::Parameters meshParams = {
        20,
        0,
        0, 0, 0,
        {0, 4, 0}
    };
    MeshBuilder_t::Parameters meshParamsN = {
        20,
        MeshBuilder_t::Parameters::NORMAL_BIT,
        0, 0, offsetof(vertexN, n),
        {0, 0, 0}
    };

    Mesh_t mesh = Mesh_t(sizeof(vertex)/4);
    MeshBuilder_t::createSphere(mesh, meshParams, 1.5);
    //meshParams.offsetX = 1;
    //meshParams.offsetY = 1;
    //MeshBuilder_t::createCone(mesh, meshParams, 2, 1, 2);
    //meshParams.offsetY = 3;
    //MeshBuilder_t::createCone(mesh, meshParams, 2, 0, 1);
    //meshParams.offsetX = 0;
    //meshParams.offsetY = -2;
    //MeshBuilder_t::createPlane(mesh, meshParams, 100, 100);
    renderable.loadVertexData(mesh.getVertices(), mesh.getVertexCount()*sizeof(vertex));
    renderable.loadIndexData(mesh.getIndices(), mesh.getIndexCount());

    Mesh_t meshN = Mesh_t(sizeof(vertexN)/4);
    meshParamsN.offset = {0,-3,0};
    MeshBuilder_t::createPlane(meshN, meshParamsN, 50, 50);
    meshParamsN.offset = {-4,2,2};
    MeshBuilder_t::createSphere(meshN, meshParamsN, 1.5);
    meshParamsN.offset = {3,0,0};
    MeshBuilder_t::createSphere(meshN, meshParamsN, 1.25);
    meshParamsN.offset = {3,0,2};
    MeshBuilder_t::createSphere(meshN, meshParamsN, 0.5);
    meshParamsN.offset = {-3, -1, -3};
    MeshBuilder_t::createSphere(meshN, meshParamsN, 2);
    meshParamsN.offset = {0,0,0};
    meshParamsN.sphere = MeshBuilder_t::Parameters::SPHERE_TYPE::ICO;
    meshParamsN.detail = 5;
    MeshBuilder_t::createSphere(meshN, meshParamsN, 1);
    renderableN.loadVertexData(meshN.getVertices(), meshN.getVertexCount()*sizeof(vertexN));
    renderableN.loadIndexData(meshN.getIndices(), meshN.getIndexCount());

    NRA::VGL::SpacialBase cameraPos({0.0f,0.0f,10.0f},glm::quat(glm::vec3(0.0f,0.0f,0.0f)));
    NRA::VGL::ProjectionParams projectionParams = {window.getAspect(), NRA::VGL::ProjectionParams::horizontalFOV(90.0f,window.getAspect())};
    NRA::VGL::Camera camera(glm::vec3(), glm::quat(), projectionParams, &cameraPos);

    float meshColor[4] = {0.0f,0.1f,0.1f,1.0f};
    float meshColorN[3] = {0.1f, 1.0f, 0.1f};
    float lightPos[4] = {0.0f,2.0f,0.0f,10.0f};
    int time = 0;

    glm::mat4 modelMat = glm::mat4(1.0f);
    glm::mat4 vpMat;
    NRA::VGL::SpacialBase modelPos;
    modelPos.moveA({0,-1,5});

    controls.setSensitivity(0.01);

    while(!(window.shouldClose())){
        ++time;
        meshColor[0] = 0.25f*(std::cos(time*0.01f)+1.0f)+0.25f;
        lightPos[0] = 5*std::sin(time*0.005f);
        lightPos[1] = 8.0f + 4.0f*std::sin(time*0.02f);
        lightPos[2] = 5*std::cos(time*0.005f);

        modelPos.rotateA(glm::vec3(0,1.0f,0),0.001f);
        modelMat = glm::mat4(1.0f);
        modelPos.transform(modelMat);

        {
            glm::mat4 ref{1.0f};
            camera.transform(ref);
            glm::vec3 up =          glm::vec3(0,1,0);
            glm::vec3 forwards =    glm::vec3(0,0,-1);
            glm::vec3 right =       glm::normalize(glm::cross(up, forwards * glm::mat3(ref)));
            forwards = glm::cross(up, right);
            if(controls.queryControl(TestControls::forwards)){
                cameraPos.moveA(forwards * -0.1f);
            }
            if(controls.queryControl(TestControls::backwards)){
                cameraPos.moveA(forwards * 0.1f);
            }
            if(controls.queryControl(TestControls::left)){
                cameraPos.moveL({-0.1f,0.0f,0.0f});
            }
            if(controls.queryControl(TestControls::right)){
                cameraPos.moveL({0.1f,0.0f,0.0f});
            }
            if(controls.queryControl(TestControls::up)){
                cameraPos.moveA({0.0f,0.1f,0.0f});
            }
            if(controls.queryControl(TestControls::down)){
                cameraPos.moveA({0.0f,-0.1f,0.0f});
            }
            if(window.getIsPointerLocked()){
                auto dpos = controls.getDMousePos();
                cameraPos.rotateL(glm::vec3(1,0,0), dpos.second);
                cameraPos.rotateA(glm::vec3(0,-1,0), dpos.first);
            }
        }
        if(controls.queryControl(TestControls::pause)){
            if(window.getIsPointerLocked()){
                window.unlockPointer();
                NRA::VGL::Window::setWireframeMode(true, true);
            }else{
                window.lockPointer();
                NRA::VGL::Window::setWireframeMode(false);
            }
            controls.unsetControl(TestControls::pause);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        
        
        NRA::VGL::Window::update();
        if(window.getAspectChanged()){
            projectionParams.aspect = window.getAspect();
            camera.updateProjectionMatrix(projectionParams);
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        vpMat = glm::mat4(1.0f);
        camera.transformP(vpMat);
        glm::mat4 identity = glm::mat4(1.0f);
        shader.bind();
        shader.setUniform4<float>("u_Color",meshColor);
        shader.setUniformMat<4>("U_vpMat", &vpMat[0][0]);
        shader.setUniformMat<4>("U_mMat", &identity[0][0]);
        
        renderable.bindBuffers();
        renderable.render();
        
        shaderN.bind();
        shaderN.setUniform4<float>("u_lightPos",lightPos);
        shaderN.setUniform3<float>("u_Color",meshColorN);
        shaderN.setUniformMat<4>("U_vpMat", &vpMat[0][0]);
        shaderN.setUniformMat<4>("U_mMat", &modelMat[0][0]);
        
        renderableN.bindBuffers();
        renderableN.render();
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        window.swapBuffer();
    }
}