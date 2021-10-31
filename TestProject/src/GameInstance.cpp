#include "pch.hpp"
#include "Layers.hpp"

struct GameInstance : public Luddite::IGameInstance
{
        GameInstance()
        {
                // PerModuleInterface::g_pSystemTable->pGameInstanceI = this;
                // g_pSys->pRuntimeObjectSystem->GetObjectFactorySystem()->SetObjectConstructorHistorySize(10);
        }

        // void Serialize(ISimpleSerializer *pSerializer) override
        // {
        //         // SERIALIZE(show_demo_window);
        //         // SERIALIZE(compileStartTime);
        //         // SERIALIZE(compileEndTime);
        // }
        void Initialize() override
        {
                LD_PROFILE_FUNCTION();
                RegisterECSTypeIds();
                LD_LOG_INFO("Initialized");
                m_LayerStack.PushLayer(std::make_shared<BaseLayer>());
                m_LayerStack.UpdateStack();
        }

        void OnUpdate(float delta_time) override
        {
                LD_PROFILE_FUNCTION();
                m_LayerStack.UpdateLayers(delta_time);
                for (auto& event : Luddite::Events::GetList<Luddite::KeyPressEvent>())
                {
                        LD_LOG_INFO("press");
                }
        }

        void OnRender(float lerp_alpha, Luddite::RenderTarget render_target) override
        {
                LD_PROFILE_FUNCTION();
                m_LayerStack.RenderLayers(lerp_alpha, render_target);
        }

        void OnImGuiRender(float lerp_alpha, Luddite::RenderTarget render_target) override
        {
                LD_PROFILE_FUNCTION();
                m_LayerStack.RenderLayersImGui(lerp_alpha, render_target);
        }

        void LoadWorld(std::shared_ptr<Luddite::Layer> layer, Luddite::World& world)
        {
                LD_PROFILE_FUNCTION();
                world.CloneTo<
                        C_Camera, C_Transform3D, C_Model,
                        C_CollisionShape, C_Collider, C_RigidBody,
                        C_PointLight, C_SpotLight, C_DirectionalLight
                        >(layer->GetWorld());
                world.CloneSingletonsTo<C_ActiveCamera>(layer->GetWorld());
                // layer->GetWorld().GetEntityFromID(Luddite::EntityID(2)).AddComponent<C_Camera>();
        }

        Luddite::LayerStack& GetLayerStack() {return m_LayerStack;}
        Luddite::LayerStack m_LayerStack;
};

#ifdef _WIN32
__declspec(dllexport)           //should create file with export import macros etc.
#else
__attribute__((visibility("default")))
#endif
Luddite::IGameInstance* NewGameInstance()
{
        return static_cast<Luddite::IGameInstance*>(new GameInstance);
}

#ifdef _WIN32
__declspec(dllexport)           //should create file with export import macros etc.
#else
__attribute__((visibility("default")))
#endif
void FreeGameInstance(Luddite::IGameInstance* pGameInstance)
{
        delete static_cast<GameInstance*>(pGameInstance);
}