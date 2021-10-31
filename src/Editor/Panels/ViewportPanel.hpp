#pragma once
#include "Editor/pch.hpp"
#include "Editor/Keybinds.hpp"
#include "Editor/EditorState.hpp"
#include "Editor/History.hpp"
#include "Editor/Commands/Commands.hpp"


//Events
#include "Editor/Events.hpp"

namespace Editor
{
struct ViewportPanel
{
        enum class Tools : uint8_t
        {
                TRANSLATE,
                ROTATE,
                SCALE
        };
        ViewportPanel()
        {
                SetRenderTexture();
                m_Camera.Position = {1.f, 1.f, 1.f};
                m_Camera.ViewDirection = glm::normalize(-m_Camera.Position);
        }

        void Resize()
        {
                Luddite::Renderer::ReleaseBufferResources();
                SetRenderTexture();
        }

        bool SetRenderTexture()
        {
                if (size.x > 0 && size.y > 0)
                {
                        m_RenderTexture = Luddite::Renderer::CreateRenderTexture(size.x, size.y);
                        return true;
                }
                return false;
        }

        void CheckForResize()
        {
                if (m_RenderTexture.GetRenderTarget().width != int(size.x) || m_RenderTexture.GetRenderTarget().height != int(size.y))
                        Resize();
        }


        void Draw(EditorState& state, History& history)
        {
                LD_PROFILE_FUNCTION();
                if (ShowWindow)
                {
                        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
                        if (ImGui::Begin("Viewport", &ShowWindow, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar))
                        {
                                if (ImGui::BeginMenuBar())
                                {
                                        if (ImGui::SmallButton("Play"))
                                        {
                                                Luddite::Events::GetList<RunGameEvent>().DispatchEvent();
                                        }
                                        ImGui::EndMenuBar();
                                }
                                auto max = ImGui::GetWindowContentRegionMax();
                                auto min = ImGui::GetWindowContentRegionMin();
                                size = glm::ivec2(max.x - min.x, max.y - min.y);
                                ImGui::Image(m_RenderTexture.GetShaderResourceView(), ImVec2(size.x, size.y));
                                if (!state.m_GameRunning)
                                {
                                        if (state.m_SelectedEntityID != Luddite::NullEntityID)
                                        {
                                                ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, size.x, size.y);

                                                ImGuizmo::SetOrthographic(false);
                                                ImGuizmo::SetDrawlist();

                                                Luddite::Entity selected = state.m_World.GetEntityFromID(state.m_SelectedEntityID);

                                                glm::mat4 projection = m_RenderTexture.GetRenderTarget().GetProjectionMatrix(m_Camera);
                                                glm::mat4 view = m_RenderTexture.GetRenderTarget().GetViewMatrix(m_Camera);

                                                C_Transform3D* pTransform = selected.TryComponent<C_Transform3D>();
                                                if (pTransform)
                                                {
                                                        auto& c_selected_transform = *pTransform;
                                                        glm::mat4 transform = c_selected_transform.GetTransformMatrix();

                                                        // if (KeyBinds::ModsPressed(0))
                                                        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                                                static_cast<ImGuizmo::OPERATION>(m_TransformOperation), ImGuizmo::LOCAL, glm::value_ptr(transform));
                                                        static bool pUsing;
                                                        static C_Transform3D old_transform;
                                                        if (ImGuizmo::IsUsing())
                                                        {
                                                                if (!pUsing)
                                                                {
                                                                        old_transform = c_selected_transform;
                                                                }
                                                                pUsing = true;
                                                                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform),
                                                                        glm::value_ptr(c_selected_transform.Translation),
                                                                        glm::value_ptr(c_selected_transform.Rotation),
                                                                        glm::value_ptr(c_selected_transform.Scale)
                                                                        );
                                                                c_selected_transform.Rotation.x = glm::radians(c_selected_transform.Rotation.x);
                                                                c_selected_transform.Rotation.y = glm::radians(c_selected_transform.Rotation.y);
                                                                c_selected_transform.Rotation.z = glm::radians(c_selected_transform.Rotation.z);
                                                        }
                                                        else if (ImGui::IsWindowFocused())
                                                        {
                                                                if (pUsing)
                                                                {
                                                                        pUsing = false;
                                                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(selected, old_transform, c_selected_transform);
                                                                }
                                                                if (KeyBinds::Pressed(eKeyBinds::ResetTranslation))
                                                                {
                                                                        C_Transform3D reset = c_selected_transform;
                                                                        reset.Translation = glm::vec3(0.f);
                                                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(selected, c_selected_transform, reset);
                                                                }
                                                                if (KeyBinds::Pressed(eKeyBinds::ResetRotation))
                                                                {
                                                                        C_Transform3D reset = c_selected_transform;
                                                                        reset.Rotation = glm::vec3(0.f);
                                                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(selected, c_selected_transform, reset);
                                                                }
                                                                if (KeyBinds::Pressed(eKeyBinds::ResetScale))
                                                                {
                                                                        C_Transform3D reset = c_selected_transform;
                                                                        reset.Scale = glm::vec3(1.f);
                                                                        history.ExecuteCommand<ReplaceComponentCommand<C_Transform3D> >(selected, c_selected_transform, reset);
                                                                }
                                                                if (KeyBinds::Pressed(eKeyBinds::CenterCamera))
                                                                        m_CamCenter = c_selected_transform.Translation;
                                                                if (KeyBinds::Pressed(eKeyBinds::TranslateTool))
                                                                        m_TransformOperation ^= ImGuizmo::OPERATION::TRANSLATE;
                                                                if (KeyBinds::Pressed(eKeyBinds::RotateTool))
                                                                        m_TransformOperation ^= ImGuizmo::OPERATION::ROTATE;
                                                                if (KeyBinds::Pressed(eKeyBinds::ScaleTool))
                                                                        m_TransformOperation ^= ImGuizmo::OPERATION::SCALE;
                                                        }
                                                }
                                        }
                                        if (ImGui::IsMouseHoveringRect(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y), ImVec2(ImGui::GetWindowPos().x + size.x, ImGui::GetWindowPos().y + size.y)))
                                        {
                                                // for (auto event: Luddite::Events::GetList<Luddite::MouseScrollEvent>())
                                                // {
                                                //         m_CamZoom *= glm::pow(0.9f, event.scrolls);
                                                //         // LD_LOG_INFO("Scrolls: {}", event.scrolls);
                                                // }

                                                m_CamZoom *= glm::pow(0.85f, ImGui::GetIO().MouseWheel);
                                                m_CamZoom = glm::clamp(m_CamZoom, 0.01f, 500.f);


                                                if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
                                                {
                                                        ImVec2 im_drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
                                                        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
                                                        // KeyBinds::ModsPressed()
                                                        // uint16_t mods = Luddite::IO::GetModBit(Luddite::Keys::ShiftLeft);
                                                        // if (KeyBinds::ModsPressed(mods))
                                                        if (KeyBinds::ModsPressed(Luddite::IO::GetModBit(Luddite::Keys::ShiftLeft)))
                                                        {
                                                                glm::vec2 drag {im_drag.x, im_drag.y};
                                                                m_CamCenter -= glm::vec3(
                                                                        glm::vec4(drag.x * 0.0013 * m_CamZoom,
                                                                                -drag.y * 0.0013 * m_CamZoom,
                                                                                0.f, 0.f)
                                                                        * glm::rotate(m_CamPitch, glm::vec3(1.f, 0.f, 0.f))
                                                                        * glm::rotate(m_CamYaw, glm::vec3(0.f, 1.f, 0.f))
                                                                        );
                                                        }
                                                        else
                                                        {
                                                                m_CamYaw += im_drag.x * 0.004;
                                                                m_CamPitch += im_drag.y * 0.004;
                                                                m_CamPitch = glm::clamp(m_CamPitch, -1.57f, 1.57f);
                                                        }
                                                }
                                        }

                                        m_Camera.Position = m_CamCenter + glm::vec3(
                                                glm::vec4(0.f, 0.f, m_CamZoom, 0.f)
                                                * glm::rotate(m_CamPitch, glm::vec3(1.f, 0.f, 0.f))
                                                * glm::rotate(m_CamYaw, glm::vec3(0.f, 1.f, 0.f))
                                                );
                                        m_Camera.ViewDirection = glm::normalize(m_CamCenter - m_Camera.Position);
                                }
                        }
                        ImGui::End();
                        ImGui::PopStyleVar();
                }
        }

        Luddite::RenderTarget& GetRenderTarget() {return m_RenderTexture.GetRenderTarget();}

        Luddite::Camera m_Camera;

        bool ShowWindow = true;
        private:
        uint32_t m_TransformOperation = static_cast<uint32_t>(ImGuizmo::OPERATION::TRANSLATE);
        Luddite::RenderTexture m_RenderTexture;
        glm::ivec2 size = glm::ivec2(500, 500);

        glm::vec3 m_CamCenter = glm::vec3(0.f);
        float m_CamPitch = 0.f;
        float m_CamYaw = 0.f;
        float m_CamZoom = 10.f;
};
}