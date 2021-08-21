#pragma once
#include "Editor/pch.hpp"
#include "Editor/EditorState.hpp"
#include "Editor/History.hpp"
#include "Editor/Commands/Commands.hpp"

namespace Editor
{
struct ComponentsPanel
{
        ComponentsPanel()
        {}

        void Draw(EditorState& state, History& history)
        {
                if (ShowWindow)
                {
                        if (ImGui::Begin("Components", &ShowWindow))
                        {
                                if (state.m_SelectedEntityID != Luddite::NullEntityID)
                                {
                                        DrawEntityComponents(state.m_World.GetEntityFromID(state.m_SelectedEntityID), history);
                                        if (ImGui::Button("Add Component"))
                                                ImGui::OpenPopup("AddComponentPopup");
                                        if (ImGui::BeginPopup("AddComponentPopup"))
                                        {
                                                if (ImGui::MenuItem("Camera"))
                                                {
                                                        history.ExecuteCommand<AddComponentCommand<C_Camera> >(state.m_World.GetEntityFromID(state.m_SelectedEntityID));
                                                }
                                                ImGui::EndPopup();
                                        }
                                }
                        }
                        ImGui::End();
                }
        }
        bool ShowWindow = true;
        private:
        void DrawEntityComponents(Luddite::Entity entity, History& history)
        {
                C_Name* pName = entity.TryComponent<C_Name>();
                if (pName)
                {
                        char name_buffer[512];
                        memset(name_buffer, 0, sizeof(name_buffer));
                        strncpy(name_buffer, pName->Name.c_str(), sizeof(name_buffer));
                        if (ImGui::InputText("Name", name_buffer, sizeof(name_buffer)))
                        {
                                history.ExecuteCommand<RenameCommand>(entity, pName->Name, std::string(name_buffer));
                        }
                }

                C_Transform3D* pTransform = entity.TryComponent<C_Transform3D>();
                if (pTransform)
                {
                        if (ImGui::TreeNodeEx((void*)typeid(C_Transform3D).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
                        {
                                C_Transform3D old = *pTransform;
                                if (ImGui::DragFloat3("Translation", glm::value_ptr(pTransform->Translation), 0.1f))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(entity, old, *pTransform);
                                if (ImGui::DragFloat3("Rotation", glm::value_ptr(pTransform->Rotation), 0.1f))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(entity, old, *pTransform);
                                if (ImGui::DragFloat3("Scale", glm::value_ptr(pTransform->Scale), 0.1f))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(entity, old, *pTransform);
                                ImGui::TreePop();
                        }
                }

                C_Camera* pCamera = entity.TryComponent<C_Camera>();
                if (pCamera)
                {
                        if (ImGui::TreeNodeEx((void*)typeid(C_Camera).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
                        {
                                C_Camera old = *pCamera;
                                if (pCamera->Projection == Luddite::Camera::ProjectionType::PERSPECTIVE)
                                        if (ImGui::InputFloat("FOV", &pCamera->FOV))
                                                history.ExecuteCommand<ReplaceComponentCommand<C_Camera> >(entity, old, *pCamera);
                                if (pCamera->Projection == Luddite::Camera::ProjectionType::ORTHOGRAPHIC)
                                        if (ImGui::InputFloat("Scale", &pCamera->OrthoScale))
                                                history.ExecuteCommand<ReplaceComponentCommand<C_Camera> >(entity, old, *pCamera);
                                if (ImGui::InputFloat("Clip Near", &pCamera->ClipNear))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_Camera> >(entity, old, *pCamera);
                                if (ImGui::InputFloat("Clip Far", &pCamera->ClipFar))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_Camera> >(entity, old, *pCamera);
                                ImGui::TreePop();
                        }
                }

                C_PointLight* pPointLight = entity.TryComponent<C_PointLight>();
                if (pPointLight)
                {
                        if (ImGui::TreeNodeEx((void*)typeid(C_PointLight).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Point Light"))
                        {
                                C_PointLight old = *pPointLight;
                                if (ImGui::ColorEdit3("Color", glm::value_ptr(pPointLight->Color.GetVec3()), ImGuiColorEditFlags_PickerHueWheel) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_PointLight> >(entity, old, *pPointLight);
                                if (ImGui::InputFloat("Intensity", &pPointLight->Intensity) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_PointLight> >(entity, old, *pPointLight);
                                if (ImGui::InputFloat("Range", &pPointLight->Range) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_PointLight> >(entity, old, *pPointLight);
                                ImGui::TreePop();
                        }
                }
        }
};
}