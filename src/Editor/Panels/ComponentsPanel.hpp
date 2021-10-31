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
                LD_PROFILE_FUNCTION();
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
                                                Luddite::Entity entity = state.m_World.GetEntityFromID(state.m_SelectedEntityID);
                                                #define COMPONENTMENUITEM(component, name) if (!entity.HasComponent<component>()) if (ImGui::MenuItem(#name)) {history.ExecuteCommand<AddComponentCommand<component> >(entity); }
                                                // COMPONENTMENUITEM(C_Transform3D, Transform);
                                                // COMPONENTMENUITEM(C_Model, Model);
                                                COMPONENTMENUITEM(C_Camera, Camera);
                                                COMPONENTMENUITEM(C_PointLight, Point Light);
                                                COMPONENTMENUITEM(C_SpotLight, Spot Light);
                                                COMPONENTMENUITEM(C_DirectionalLight, Directional Light);
                                                COMPONENTMENUITEM(C_Name, Name);
                                                COMPONENTMENUITEM(C_RigidBody, Rigid Body);
                                                COMPONENTMENUITEM(C_CollisionShape, Collision Shape);
                                                // COMPONENTMENUITEM(C_Collider, Collider);
                                                #undef  COMPONENTMENUITEM
                                                ImGui::EndPopup();
                                        }
                                }
                        }
                        ImGui::End();
                }
        }
        bool ShowWindow = true;
        private:
        template <typename T, typename GUIFunc>
        static void DrawComponent(const std::string& label, Luddite::Entity entity, GUIFunc func)
        {
                T* pComp = entity.TryComponent<T>();
                if (pComp)
                {
                        const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
                        if (ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, label.c_str()))
                        {
                                T old = *pComp;
                                func(pComp, old);
                                ImGui::TreePop();
                        }
                }
        }
        void DrawEntityComponents(Luddite::Entity entity, History& history)
        {
                C_Name* pName = entity.TryComponent<C_Name>();
                if (pName)
                {
                        char name_buffer[512];
                        memset(name_buffer, 0, sizeof(name_buffer));
                        strncpy(name_buffer, pName->Name.c_str(), sizeof(name_buffer));
                        if (ImGui::InputText("##Name", name_buffer, sizeof(name_buffer), ImGuiInputTextFlags_AutoSelectAll))
                        {
                                history.ExecuteCommand<RenameCommand>(entity, pName->Name, std::string(name_buffer));
                        }
                }

                DrawComponent<C_Transform3D>("Transform", entity, [&](auto* pTransform, auto& old)
                        {
                                if (ImGui::DragFloat3("Translation", glm::value_ptr(pTransform->Translation), 0.1f))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(entity, old, *pTransform);
                                if (ImGui::DragFloat3("Rotation", glm::value_ptr(pTransform->Rotation), 0.1f))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(entity, old, *pTransform);
                                if (ImGui::DragFloat3("Scale", glm::value_ptr(pTransform->Scale), 0.1f))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(entity, old, *pTransform);
                        });

                DrawComponent<C_Camera>("Camera", entity, [&](auto* pCamera, auto& old)
                        {
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
                        });

                DrawComponent<C_PointLight>("Point Light", entity, [&](auto* pPointLight, auto& old)
                        {
                                if (ImGui::ColorEdit3("Color", glm::value_ptr(pPointLight->Color.GetVec3()), ImGuiColorEditFlags_PickerHueWheel) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_PointLight> >(entity, old, *pPointLight);
                                if (ImGui::InputFloat("Intensity", &pPointLight->Intensity) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_PointLight> >(entity, old, *pPointLight);
                                if (ImGui::InputFloat("Range", &pPointLight->Range) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_PointLight> >(entity, old, *pPointLight);
                        });

                DrawComponent<C_SpotLight>("Spot Light", entity, [&](auto* pSpotLight, auto& old)
                        {
                                if (ImGui::ColorEdit3("Color", glm::value_ptr(pSpotLight->Color.GetVec3()), ImGuiColorEditFlags_PickerHueWheel) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_SpotLight> >(entity, old, *pSpotLight);
                                if (ImGui::InputFloat("Intensity", &pSpotLight->Intensity) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_SpotLight> >(entity, old, *pSpotLight);
                                if (ImGui::InputFloat("Range", &pSpotLight->Range) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_SpotLight> >(entity, old, *pSpotLight);
                                if (ImGui::SliderAngle("Half Angle", &pSpotLight->HalfAngle, 0.f, 180.f) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_SpotLight> >(entity, old, *pSpotLight);
                        });
                DrawComponent<C_DirectionalLight>("Directional Light", entity, [&](auto* pDirectionalLight, auto& old)
                        {
                                if (ImGui::ColorEdit3("Color", glm::value_ptr(pDirectionalLight->Color.GetVec3()), ImGuiColorEditFlags_PickerHueWheel) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_DirectionalLight> >(entity, old, *pDirectionalLight);
                                if (ImGui::InputFloat("Intensity", &pDirectionalLight->Intensity) && !ImGui::IsMouseDown(0))
                                        history.ExecuteCommand<ReplaceComponentCommand<C_DirectionalLight> >(entity, old, *pDirectionalLight);
                        });
        }
};
}