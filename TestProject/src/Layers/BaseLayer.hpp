#pragma once
#include "pch.hpp"

class BaseLayer : public Luddite::Layer
{
        public:
        void Initialize();
        void HandleEvents();
        void Update(double delta_time);
        void FixedUpdate(double delta_time);
        void Render(double alpha, Luddite::RenderTarget render_target);
        void RenderImGui(double alpha, Luddite::RenderTarget render_target);
        const char* GetName() {return "Test";}

        private:
        // std::unique_ptr<ViewportPanel> m_pViewportPanel;
};