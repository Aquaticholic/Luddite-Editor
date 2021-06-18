#pragma once
#include "Editor/pch.hpp"
#include "Editor/ECS/Components/Components.hpp"
#include "Editor/Input.hpp"

struct ComponentsPanel
{
        ComponentsPanel() {}

        void Draw(Luddite::World& world, Luddite::EntityID& selected_id)
        {
                world.GetRegistry().visit([&](const auto info) {
                });
        }

        template <typename T>
        DrawComponent()
        {
        }
}