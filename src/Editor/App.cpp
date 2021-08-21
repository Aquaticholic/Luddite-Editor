#define LD_ENTRYPOINT
#include "Luddite/Core/EntryPoint.hpp"
#include "Editor/pch.hpp"
#include "Editor/EditorState.hpp"
#include "Editor/History.hpp"

//Events
#include "Editor/Events.hpp"

//IO
#include "Editor/Keybinds.hpp"

//Runtime
#include "Editor/GameInstanceCompiler.hpp"
// #include "RuntimeObjectSystem.h"
// #include "RCCppLogger.hpp"
// #include "SystemTable.h"

//ECS
#include "ECS/ComponentDescription.hpp"

//Panels
#include "Editor/Panels/ViewportPanel.hpp"
#include "Editor/Panels/ScenePanel.hpp"
#include "Editor/Panels/ComponentsPanel.hpp"

namespace Editor
{
class Editor : public Luddite::Application
{
        public:
        Editor()
        {
                LD_LOG_INFO("Test Application starting");
                CreateMainWindow("Luddite Test App");
                LD_LOG_INFO("Window Created");
                ImGui::SetCurrentContext(m_pMainWindow->GetImGuiContext());
        }
        ~Editor()
        {
                // CleanRCCpp();
        }

        void Initialize()
        {
                KeyBinds::Init();
                m_pViewportPanel = std::make_unique<ViewportPanel>();
                m_pScenePanel = std::make_unique<ScenePanel>();
                m_pComponentsPanel = std::make_unique<ComponentsPanel>();

                m_EditorState.m_World.RegisterSystem<S_SceneSubmitter>();
                m_EditorState.m_World.ConfigureSystems();

                Luddite::Shader::Handle test_shader = Luddite::Assets::GetShaderLibrary().GetAsset(5815116752502599444ULL);

                {
                        auto e = m_EditorState.m_World.CreateEntity();
                        e.AddComponent<C_Name>("Monkey See");
                        auto& transform = e.AddComponent<C_Transform3D>();
                        e.AddComponent<C_Model>(Luddite::Assets::GetBasicModelLibrary().GetAsset(13985675099142567469ULL));
                        auto& rb = e.AddComponent<C_RigidBody>();
                        rb.mass = 0.f;
                }

                {
                        auto e = m_EditorState.m_World.CreateEntity();
                        e.AddComponent<C_Name>("Monkey Do");
                        auto& transform = e.AddComponent<C_Transform3D>();
                        transform.Translation.x = 1.f;
                        e.AddComponent<C_Model>(Luddite::Assets::GetBasicModelLibrary().GetAsset(11702536871773590062ULL));
                        C_CollisionShape::CollisionShape m_Shape;
                        m_Shape.type = C_CollisionShape::Shape::Box;
                        m_Shape.data.x = 1.;
                        m_Shape.data.y = 1.;
                        m_Shape.data.z = 1.;
                        e.AddComponent<C_CollisionShape>().shapes.push_back(m_Shape);
                        e.AddComponent<C_RigidBody>();
                        m_EditorState.m_SelectedEntityID = e.GetID();
                }

                {
                        auto cam = m_EditorState.m_World.CreateEntity();
                        auto& transform = cam.AddComponent<C_Transform3D>();
                        transform.Translation.z = 5.f;
                        // transform.r
                        transform.Rotation.y = glm::pi<float>();
                        cam.AddComponent<C_Camera>();
                        auto ccccc = cam.GetComponent<C_Camera>();
                        m_EditorState.m_World.SetSingleton<C_ActiveCamera>(cam.GetID());
                }

                {
                        auto light = m_EditorState.m_World.CreateEntity();
                        auto& transform = light.AddComponent<C_Transform3D>();
                        transform.Translation.y = 1.5f;
                        auto& c_light = light.AddComponent<C_PointLight>();
                        c_light.Range = 10.f;
                }
                m_EditorState.m_CurrentProjectDir = "/home/evan/Documents/Dev/Luddite-Editor/TestProject";
                m_EditorState.m_GameInstanceCompiler.SetProjectDirectory(m_EditorState.m_CurrentProjectDir);
                m_EditorState.m_GameInstanceCompiler.Compile();
        }

        void OnUpdate(float delta_time)
        {
                m_EditorState.m_GameInstanceCompiler.Update(delta_time);
                if (m_EditorState.m_GameInstanceCompiler.IsCompilationDone())
                {
                        m_EditorState.m_GameInstanceCompiler.LoadNewLib();
                        Luddite::IGameInstance* pNewGameInstance = m_EditorState.m_GameInstanceCompiler.GetNewGameInstance();
                        if (m_EditorState.m_pGameInstance)
                        {
                                //transfer game state if game is running
                                if (m_EditorState.m_GameRunning)
                                {
                                        // pNewGameInstance->LoadWorld()
                                }
                                m_EditorState.m_GameInstanceCompiler.FreeOldGameInstance(m_EditorState.m_pGameInstance);
                        }
                        m_EditorState.m_GameInstanceCompiler.FreeOldLib();
                        m_EditorState.m_pGameInstance = pNewGameInstance;
                        LD_LOG_INFO("Compilation Loaded");
                }
                // UpdateRCCpp(delta_time);
                if (m_EditorState.m_GameRunning)
                        m_EditorState.m_pGameInstance->OnUpdate(delta_time);
        }

        void OnRender(float lerp_alpha)
        {
                KeyBinds::Update();

                // if (KeyBinds::ModsPressed(0))
                // if (KeyBinds::ModsPressed(Luddite::IO::GetModBit(Luddite::Keys::ShiftLeft)))
                if (KeyBinds::Pressed(eKeyBinds::Undo))
                        m_History.Undo();

                if (KeyBinds::Pressed(eKeyBinds::Redo))
                        m_History.Redo();
                // case Luddite::Keys::Z:
                //         if (EditorPersistentInput::ctrl_down && !EditorPersistentInput::shift_down && !EditorPersistentInput::alt_down)
                //         if (EditorPersistentInput::ctrl_down && EditorPersistentInput::shift_down && !EditorPersistentInput::alt_down)
                //                 m_History.Redo();
                //         break;

                if (m_pViewportPanel->ShowWindow)
                {
                        m_pViewportPanel->CheckForResize();
                        if (!m_EditorState.m_GameRunning)
                        {
                                m_EditorState.m_World.UpdateSystem<S_SceneSubmitter>(m_EditorState.m_World, lerp_alpha);
                                Luddite::Renderer::Draw(m_pViewportPanel->GetRenderTarget(), m_pViewportPanel->m_Camera);
                        }
                        else
                        {
                                m_EditorState.m_pGameInstance->OnRender(lerp_alpha, m_pViewportPanel->GetRenderTarget());
                        }
                }

                m_History.ExecuteDefferedCommands();
                // g_SystemTable.pGameInstanceI->OnRender(lerp_alpha, m_pMainWindow->GetRenderTarget());
        }

        void OnImGuiRender(float lerp_alpha)
        {
                RenderImGuiDockspace();
                m_pScenePanel->Draw(m_EditorState, m_History);
                m_pViewportPanel->Draw(m_EditorState, m_History);
                m_pComponentsPanel->Draw(m_EditorState, m_History);
                // g_SystemTable.pGameInstanceI->OnImGuiRender(lerp_alpha, m_pMainWindow->GetRenderTarget());

                if (Luddite::Events::GetList<RunGameEvent>().GetSize() > 0)
                {
                        Luddite::Events::GetList<RunGameEvent>().Clear();
                        RunGameInstance();
                }
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
                ImGuiID dockspace_id = ImGui::GetID("Dockspace");
                ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

                static auto first_time = true;
                if (first_time)
                {
                        first_time = false;
                        ImGui::DockBuilderRemoveNode(dockspace_id);         // clear any previous layout
                        ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
                        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

                        auto dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, nullptr, &dockspace_id);
                        auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.3f, nullptr, &dockspace_id);
                        // auto dock_id_bottom_left = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.3f, nullptr, &dock_id_left);

                        ImGui::DockBuilderDockWindow("Viewport", dockspace_id);;
                        ImGui::DockBuilderDockWindow("Scene", dock_id_left);
                        ImGui::DockBuilderDockWindow("Components", dock_id_right);

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
                                // static bool selected = true;
                                ImGui::MenuItem("Viewport", NULL, &m_pViewportPanel->ShowWindow);
                                ImGui::MenuItem("Scene", NULL, &m_pScenePanel->ShowWindow);
                                ImGui::MenuItem("Components", NULL, &m_pComponentsPanel->ShowWindow);
                                ImGui::EndMenu();
                        }
                        ImGui::EndMenuBar();
                }
                ImGui::End();
        }

        void RunGameInstance()
        {
                if (m_EditorState.m_pGameInstance)
                {
                        m_EditorState.m_pGameInstance->Initialize();
                        auto layer = m_EditorState.m_pGameInstance->GetLayerStack().GetLayerByName("Test");
                        if (layer)
                        {
                                // m_EditorState.m_World.CloneTo<
                                //         C_Camera, C_Transform3D, C_Model,
                                //         C_CollisionShape, C_Collider, C_RigidBody,
                                //         C_PointLight, C_SpotLight, C_DirectionalLight
                                //         >(layer->GetWorld());
                                m_EditorState.m_pGameInstance->LoadWorld(layer, m_EditorState.m_World);
                        }
                        m_EditorState.m_GameRunning = true;
                }
        }

        // bool InitRCCpp()
        // {
        //         g_SystemTable.pRuntimeObjectSystem = new RuntimeObjectSystem;
        //         g_SystemTable.pRuntimeObjectSystem->SetFastCompileMode(true);
        //         g_SystemTable.pRuntimeObjectSystem->SetOptimizationLevel(RCppOptimizationLevel::RCCPPOPTIMIZATIONLEVEL_DEBUG);

        //         if (!g_SystemTable.pRuntimeObjectSystem->Initialise(&g_Logger, &g_SystemTable))
        //         {
        //                 delete g_SystemTable.pRuntimeObjectSystem;
        //                 g_SystemTable.pRuntimeObjectSystem = 0;
        //                 return false;
        //         }

        //         g_SystemTable.pRuntimeObjectSystem->CleanObjectFiles();
        //         #ifndef LD_PLATFORM_WINDOWS
        // g_SystemTable.pRuntimeObjectSystem->SetAdditionalCompileOptions("-std=c++17 -D" LD_PLATFORM " -D" LD_CONFIGURATION " -include cmake_pch.hxx -H");
        //         #endif

        //         // ensure include directories are set - use location of this file as starting point
        //         //this file (App.cpp) must remain in the src/editor folder
        //         FileSystemUtils::Path basePath = g_SystemTable.pRuntimeObjectSystem->FindFile(__FILE__).ParentPath().ParentPath();
        //         #define ADD_DIR(path) g_SystemTable.pRuntimeObjectSystem->AddIncludeDir(#path);
        //         ENGINE_INCLUDE_DIRS
        //         #undef ADD_DIR

        //         #define ADD_DIR(path) g_SystemTable.pRuntimeObjectSystem->AddLibraryDir(#path);
        //         ENGINE_LIBRARY_DIRS
        //         #undef ADD_DIR

        //         return true;
        // }

        // void UpdateRCCpp(float dt)
        // {
        //         if (g_SystemTable.pRuntimeObjectSystem->GetIsCompiledComplete())
        //         {
        //                 g_SystemTable.pRuntimeObjectSystem->LoadCompiledModule();
        //                 g_SystemTable.pGameInstanceI->Initialize();
        //         }

        //         if (!g_SystemTable.pRuntimeObjectSystem->GetIsCompiling())
        //         {
        //                 g_SystemTable.pRuntimeObjectSystem->GetFileChangeNotifier()->Update(dt);
        //         }
        // }

        // void CleanRCCpp()
        // {
        // }

        private:
        EditorState m_EditorState;
        History m_History;
        std::unique_ptr<ViewportPanel> m_pViewportPanel;
        std::unique_ptr<ScenePanel> m_pScenePanel;
        std::unique_ptr<ComponentsPanel> m_pComponentsPanel;
};
}

Luddite::Application* Luddite::CreateApplication()
{
        return new Editor::Editor();
}