#pragma once
#include "Editor/pch.hpp"
#include "Editor/ECS/Components/Components.hpp"
#include "Editor/Input.hpp"

//Events
#include "Editor/Events.hpp"


struct ViewportPanel
{
        ViewportPanel()
        {
                SetRenderTexture();
                m_Camera.Position = {1.f, 1.f, 1.f};
                m_Camera.ViewDirection = glm::normalize(-m_Camera.Position);
        }

        void Resize()
        {
                if (SetRenderTexture())
                        Luddite::Renderer::ReleaseBufferResources();
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


        void Draw(Luddite::World& world, Luddite::EntityID& selected_id)
        {
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

                                if (selected_id != Luddite::NullEntityID)
                                {
                                        LD_LOG_INFO("ID: {}", selected_id);
                                        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, size.x, size.y);

                                        ImGuizmo::SetOrthographic(false);
                                        ImGuizmo::SetDrawlist();

                                        Luddite::Entity selected = world.GetEntityFromID(selected_id);

                                        glm::mat4 projection = m_RenderTexture.GetRenderTarget().GetProjectionMatrix(m_Camera);
                                        glm::mat4 view = m_RenderTexture.GetRenderTarget().GetViewMatrix(m_Camera);

                                        auto& c_selected_transform = selected.GetComponent<C_Transform3D>();
                                        glm::mat4 transform = c_selected_transform.GetTransformMatrix();

                                        ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                                ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform));
                                        if (ImGuizmo::IsUsing())
                                        {
                                                C_Transform3D replace;
                                                ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform),
                                                        glm::value_ptr(replace.Translation),
                                                        glm::value_ptr(replace.Rotation),
                                                        glm::value_ptr(replace.Scale)
                                                        );
                                                selected.ReplaceComponent<C_Transform3D>(replace);
                                        }
                                }
                                if (ImGui::IsMouseHoveringRect(ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y), ImVec2(ImGui::GetWindowPos().x + size.x, ImGui::GetWindowPos().y + size.y)))
                                {
                                        // for (auto event: Luddite::Events::GetList<Luddite::MouseScrollEvent>())
                                        // {
                                        //         m_CamZoom *= glm::pow(0.9f, event.scrolls);
                                        //         LD_LOG_INFO("Scrolls: {}", event.scrolls);
                                        // }

                                        m_CamZoom *= glm::pow(0.85f, ImGui::GetIO().MouseWheel);
                                        m_CamZoom = glm::clamp(m_CamZoom, 0.01f, 500.f);


                                        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
                                        {
                                                ImVec2 im_drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
                                                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Middle);
                                                if (EditorPersistentInput::shift_down)
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
                        ImGui::End();
                        ImGui::PopStyleVar();
                }
        }

        Luddite::RenderTarget& GetRenderTarget() {return m_RenderTexture.GetRenderTarget();}

        Luddite::Camera m_Camera;

        bool ShowWindow = true;
        private:
        Luddite::RenderTexture m_RenderTexture;
        glm::ivec2 size = glm::ivec2(500, 500);

        glm::vec3 m_CamCenter = glm::vec3(0.f);
        float m_CamPitch = 0.f;
        float m_CamYaw = 0.f;
        float m_CamZoom = 10.f;
};