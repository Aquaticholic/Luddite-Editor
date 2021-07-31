#pragma once
#include "Editor/pch.hpp"
#include "Editor/EditorState.hpp"
#include "Editor/History.hpp"

namespace Editor
{
struct ScenePanel
{
        ScenePanel()
        {
        }

        void Draw(EditorState& state, History& history)
        {
                if (ShowWindow)
                {
                        // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                        if (ImGui::Begin("Scene", &ShowWindow, ImGuiWindowFlags_NoCollapse))
                        {
                                if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
                                        state.m_SelectedEntityID = Luddite::NullEntityID;
                                state.m_World.each([&](Luddite::EntityID id) {
                                                DrawEntityTree(state.m_World.GetEntityFromID(id), state.m_SelectedEntityID);
                                        });
                                // for (auto id : world.each())
                                // {
                                //         LD_LOG_INFO("id: {}", id);
                                // }
                        }
                        ImGui::End();
                        // ImGui::PopStyleVar();
                }
        }

        bool ShowWindow = true;
        private:
        void DrawEntityTree(Luddite::Entity entity, Luddite::EntityID& selected_id)
        {
                ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
                if (entity.GetID() == selected_id)
                        flags |= ImGuiTreeNodeFlags_Selected;
                C_Name* c_name = entity.TryComponent<C_Name>();
                const char* name;
                if (c_name)
                        name = c_name->Name.c_str();
                else
                        name = "Unnamed";
                bool opened = ImGui::TreeNodeEx((void*)entity.GetID(), flags, name);
                if (ImGui::IsItemClicked())
                {
                        selected_id = entity.GetID();
                }

                if (opened)
                {
                        ImGui::TreePop();
                }
        }
};
}