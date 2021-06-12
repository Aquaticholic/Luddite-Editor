#define LD_ENTRYPOINT
#include "Luddite/Core/EntryPoint.hpp"
#include "Editor/pch.hpp"

#include "RuntimeObjectSystem.h"

#include "RCCppLogger.hpp"
#include "SystemTable.h"
#include "Editor/GameInstance.hpp"

//ECS
#include "Editor/ECS/Components/Components.hpp"

//Panels
#include "Editor/Panels/ViewportPanel.hpp"

static RCCppLogger g_Logger;
static SystemTable g_SystemTable;

class Editor : public Luddite::Application
{
        public:
        Editor()
        {
                LD_LOG_INFO("Test Application starting");
                CreateMainWindow("Luddite Test App");
                LD_LOG_INFO("Window Created");
                InitRCCpp();
                g_SystemTable.pGameInstanceI->Initialize();
                ImGui::SetCurrentContext(m_pMainWindow->GetImGuiContext());
        }
        ~Editor()
        {
                CleanRCCpp();
        }

        void Initialize()
        {
                m_pViewportPanel = std::make_unique<ViewportPanel>();
                m_World.RegisterSystem<S_SceneSubmitter>();
                m_World.ConfigureSystems();


                {
                        auto e = m_World.CreateEntity();
                        auto& transform = e.AddComponent<C_Transform3D>();
                        // transform.Translation.x = 1.f;
                        // transform.Translation.y = 1.f;
                        // transform.Translation.z = 1.f;
                        // transform.Rotation.y = glm::pi<float>();
                        e.AddComponent<C_Model>(Luddite::ModelLoader::GetBasicModel("Assets/suzanne.obj"));
                        m_SelectedEntityID = e.GetID();
                }
        }

        void OnUpdate(float delta_time)
        {
                g_SystemTable.pGameInstanceI->OnUpdate(delta_time);
                UpdateRCCpp(delta_time);
        }

        void OnRender(float lerp_alpha)
        {
                if (m_pViewportPanel->ShowWindow)
                {
                        m_World.UpdateSystem<S_SceneSubmitter>(m_World, lerp_alpha);
                        m_pViewportPanel->CheckForResize();
                        Luddite::Renderer::Draw(m_pViewportPanel->GetRenderTarget(), m_pViewportPanel->m_Camera);
                }
                g_SystemTable.pGameInstanceI->OnRender(lerp_alpha, m_pMainWindow->GetRenderTarget());
        }

        void OnImGuiRender(float lerp_alpha)
        {
                RenderImGuiDockspace();
                m_pViewportPanel->Draw(m_World, m_SelectedEntityID);
                g_SystemTable.pGameInstanceI->OnImGuiRender(lerp_alpha, m_pMainWindow->GetRenderTarget());
        }

        void RenderImGuiDockspace()
        {
                static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
                ImGuiViewport* viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                ImGui::Begin("Dockspace", nullptr, window_flags);
                ImGui::PopStyleVar();
                ImGui::PopStyleVar(2);
                //DockSpace
                // ImGuiIO& io = ImGui::GetIO();
                ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

                static auto first_time = true;
                if (first_time)
                {
                        first_time = false;
                        ImGui::DockBuilderRemoveNode(dockspace_id);         // clear any previous layout
                        ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

                        auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);

                        ImGui::DockBuilderDockWindow("Viewport", dockspace_id);
                        ImGui::DockBuilderDockWindow("Left", dock_id_left);

                        ImGui::DockBuilderFinish(dockspace_id);
                }

                if (ImGui::BeginMenuBar())
                {
                        if (ImGui::BeginMenu("File"))
                        {
                                ImGui::EndMenu();
                        }
                        if (ImGui::BeginMenu("Edit"))
                        {
                                ImGui::EndMenu();
                        }
                        if (ImGui::BeginMenu("View"))
                        {
                                static bool selected = true;
                                ImGui::MenuItem("Viewport", NULL, &m_pViewportPanel->ShowWindow);
                                ImGui::EndMenu();
                        }
                        ImGui::EndMenuBar();
                }
                ImGui::End();

                ImGui::Begin("Left");
                ImGui::Text("Hello, left!");
                ImGui::End();
        }

        bool InitRCCpp()
        {
                g_SystemTable.pRuntimeObjectSystem = new RuntimeObjectSystem;
                g_SystemTable.pRuntimeObjectSystem->SetFastCompileMode(true);
                g_SystemTable.pRuntimeObjectSystem->SetOptimizationLevel(RCppOptimizationLevel::RCCPPOPTIMIZATIONLEVEL_DEBUG);

                if (!g_SystemTable.pRuntimeObjectSystem->Initialise(&g_Logger, &g_SystemTable))
                {
                        delete g_SystemTable.pRuntimeObjectSystem;
                        g_SystemTable.pRuntimeObjectSystem = 0;
                        return false;
                }

                g_SystemTable.pRuntimeObjectSystem->CleanObjectFiles();
                #ifndef LD_PLATFORM_WINDOWS
                g_SystemTable.pRuntimeObjectSystem->SetAdditionalCompileOptions("-std=c++17 -D" LD_PLATFORM " -D" LD_CONFIGURATION " -include cmake_pch.hxx -H");
                #endif

                // ensure include directories are set - use location of this file as starting point
                //this file needs to be in the src/editor folder
                FileSystemUtils::Path basePath = g_SystemTable.pRuntimeObjectSystem->FindFile(__FILE__).ParentPath().ParentPath();
                #define ADD_DIR(path) g_SystemTable.pRuntimeObjectSystem->AddIncludeDir(#path);
                ENGINE_INCLUDE_DIRS
                #undef ADD_DIR

                #define ADD_DIR(path) g_SystemTable.pRuntimeObjectSystem->AddLibraryDir(#path);
                ENGINE_LIBRARY_DIRS
                #undef ADD_DIR

                return true;
        }

        void UpdateRCCpp(float dt)
        {
                if (g_SystemTable.pRuntimeObjectSystem->GetIsCompiledComplete())
                {
                        g_SystemTable.pRuntimeObjectSystem->LoadCompiledModule();
                        g_SystemTable.pGameInstanceI->Initialize();
                }

                if (!g_SystemTable.pRuntimeObjectSystem->GetIsCompiling())
                {
                        g_SystemTable.pRuntimeObjectSystem->GetFileChangeNotifier()->Update(dt);
                }
        }

        void CleanRCCpp()
        {
        }

        private:
        Luddite::World m_World;
        Luddite::EntityID m_SelectedEntityID = Luddite::NullEntityID;

        std::unique_ptr<ViewportPanel> m_pViewportPanel;
};

Luddite::Application* Luddite::CreateApplication()
{
        return new Editor();
}