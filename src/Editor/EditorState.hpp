#pragma once
#include "Editor/pch.hpp"

namespace Editor
{
struct EditorState
{
        Luddite::World m_World;
        Luddite::EntityID m_SelectedEntityID = Luddite::NullEntityID;
        bool m_GameRunning = false;
        std::string m_CurrentLayer = "Test";
};
}