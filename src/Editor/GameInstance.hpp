#pragma once
// #include "Luddite/Core/pch.hpp"

struct GameInstanceI
{
        virtual void Initialize() = 0;
        virtual void OnUpdate(float delta_time) = 0;
        virtual void OnRender(float lerp_alpha, Luddite::RenderTarget render_target) = 0;
        virtual void OnImGuiRender(float lerp_alpha, Luddite::RenderTarget render_target) = 0;
};
