#pragma once
#include "Editor/pch.hpp"
#include "Editor/EditorState.hpp"
#include "Editor/History.hpp"

namespace Editor
{
#define LIBRARY_TYPES_DECLARE \
        LIBRARY_TYPE_DECLARATION(ModelLibrary, "Models") \
        LIBRARY_TYPE_DECLARATION(TextureLibrary, "Textures") \
        LIBRARY_TYPE_DECLARATION(ShaderLibrary, "Shaders")
#undef LIBRARY_TYPES_DECLARE

enum class eLibraryTypes : uint8_t
{
        #define LIBRARY_TYPE_DECLARATION(Type, Name) e ## Type,
        LIBRARY_TYPES_DECLARE
        #undef LIBRARY_TYPE_DECLARATION
        // SIZE
};

// const char* LibraryTypesNames =
// {
//         #define LIBRARY_TYPE_DECLARATION(Type, Name) Name,
//         LIBRARY_TYPES_DECLARE
//         #undef LIBRARY_TYPE_DECLARATION
// };

struct AssetBrowserPanel
{
        AssetBrowserPanel() = default;

        bool ShowWindow = true;
        // eLibraryTypes m_CurrentType{0};

        void Draw(EditorState& state, History& history)
        {
                LD_PROFILE_FUNCTION();
                if (ShowWindow)
                {
                        if (ImGui::Begin("Asset Browser", &ShowWindow, ImGuiWindowFlags_NoCollapse))
                        {
                        }
                        ImGui::End();
                }
        }
};
}