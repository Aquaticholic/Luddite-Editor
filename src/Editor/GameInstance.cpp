#include "Editor/Runtime.hpp"

#include "ObjectInterfacePerModule.h"
#include "IObject.h"
#include "Editor/GameInstance.hpp"
#include "Editor/SystemTable.h"
#include "ISimpleSerializer.h"
#include "IRuntimeObjectSystem.h"
#include "IObjectFactorySystem.h"

// #include "Editor/Layers/Layers.hpp"
#include "Editor/Layers/BaseLayer.hpp"


enum InterfaceIDEnumLuddite
{
        IID_IRCCPP_GAME_INSTANCE = IID_ENDInterfaceID, // IID_ENDInterfaceID from IObject.h InterfaceIDEnum

        IID_ENDInterfaceIDEnumLuddite
};

struct GameInstance : GameInstanceI, TInterface<IID_IRCCPP_GAME_INSTANCE, IObject>
{
        GameInstance()
        {
                PerModuleInterface::g_pSystemTable->pGameInstanceI = this;
                g_pSys->pRuntimeObjectSystem->GetObjectFactorySystem()->SetObjectConstructorHistorySize(10);
        }

        // void Serialize(ISimpleSerializer *pSerializer) override
        // {
        //         // SERIALIZE(show_demo_window);
        //         // SERIALIZE(compileStartTime);
        //         // SERIALIZE(compileEndTime);
        // }
        void Initialize() override
        {
                LD_LOG_INFO("Initialized");
                m_LayerStack.PushLayer(std::make_shared<BaseLayer>());
                m_LayerStack.UpdateStack();
        }

        void OnUpdate(float delta_time) override
        {
                m_LayerStack.UpdateLayers(delta_time);
        }

        void OnRender(float lerp_alpha, Luddite::RenderTarget render_target) override
        {
                m_LayerStack.RenderLayers(lerp_alpha, render_target);
        }

        void OnImGuiRender(float lerp_alpha, Luddite::RenderTarget render_target) override
        {
                m_LayerStack.RenderLayersImGui(lerp_alpha, render_target);
        }

        void LoadWorld(std::shared_ptr<Luddite::Layer> layer, Luddite::World& world)
        {
                world.CloneTo<
                        C_Transform3D, C_Model, C_Camera,
                        C_CollisionShape, C_Collider, C_RigidBody,
                        C_PointLight, C_SpotLight, C_DirectionalLight
                        >(layer->GetWorld());
                world.CloneSingletonsTo<C_ActiveCamera>(layer->GetWorld());
        }

        Luddite::LayerStack& GetLayerStack() {return m_LayerStack;}

        Luddite::LayerStack m_LayerStack;
};

REGISTERSINGLETON(GameInstance, true);