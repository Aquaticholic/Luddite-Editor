#pragma once
#include "Editor/pch.hpp"
#include "ECS/ComponentDescription.hpp"
#include "Editor/GameInstanceCompiler.hpp"

namespace Editor
{
struct EditorState
{
        Luddite::World m_World;
        Luddite::EntityID m_SelectedEntityID = Luddite::NullEntityID;
        bool m_GameRunning = false;
        std::string m_CurrentLayer = "Test";
        std::filesystem::path m_CurrentProjectDir;
        Luddite::IGameInstance* m_pGameInstance = nullptr;
        GameInstanceCompiler m_GameInstanceCompiler;
};
}