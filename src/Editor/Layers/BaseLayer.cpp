#include "Editor/Layers/BaseLayer.hpp"
#include "Editor/Layers/EditorLayer.hpp"
#include "Editor/ECS/Components/Components.hpp"

// Luddite::Entity cam;
void BaseLayer::Initialize()
{
        m_World.RegisterSystem<S_SceneSubmitter>();
        m_World.RegisterSystem<S_RenderActiveCamera>();
        m_World.RegisterSystem<S_Physics>();
        m_World.ConfigureSystems();

        // {
        //         auto e = m_World.CreateEntity();
        //         auto& transform = e.AddComponent<C_Transform3D>();
        //         // transform.Translation.x = 1.f;
        //         // transform.Translation.y = 1.f;
        //         // transform.Translation.z = 1.f;
        //         // transform.Rotation.y = glm::pi<float>();
        //         e.AddComponent<C_Model>(Luddite::ModelLoader::GetBasicModel("Assets/suzanne.obj"));
        //         m_World.SetSingleton<C_SelectedEntity>(e.GetID());
        // }
}

void BaseLayer::HandleEvents()
{
}

void BaseLayer::Update(double delta_time)
{
        m_World.UpdateSystem<S_Physics>(m_World, delta_time);
}

void BaseLayer::Render(double alpha, Luddite::RenderTarget render_target)
{
        m_World.UpdateSystem<S_SceneSubmitter>(m_World, alpha);
        m_World.UpdateSystem<S_RenderActiveCamera>(m_World, render_target);
        // LD_LOG_INFO("TEST");
}

void BaseLayer::RenderImGui(double alpha, Luddite::RenderTarget render_target)
{
}