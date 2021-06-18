#pragma once
#include "Editor/pch.hpp"

struct GameInstanceI
{
        virtual void Initialize() = 0;
        virtual void OnUpdate(float delta_time) = 0;
        virtual void OnRender(float lerp_alpha, Luddite::RenderTarget render_target) = 0;
        virtual void OnImGuiRender(float lerp_alpha, Luddite::RenderTarget render_target) = 0;
        virtual Luddite::LayerStack& GetLayerStack() = 0;
        virtual void LoadWorld(std::shared_ptr<Luddite::Layer> layer, Luddite::World& world) = 0;
};
