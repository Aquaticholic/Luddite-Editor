// #pragma once
// #include "Editor/pch.hpp"
// #include "Editor/EditorState.hpp"
// #include "Editor/History.hpp"

// struct Panel
// {
//         public:
//         void Draw(EditorState& state, History& history)
//         {
//                 if (ShowWindow)
//                 {
//                         if (ImGui::Begin(m_Name.c_str(), &ShowWindow, m_WindowFlags))
//                                 DrawImplementation(state, History & history);
//                         ImGui::End();
//                 }
//         }

//         protected:
//         std::string m_Name = "";
//         bool ShowWindow = true;
//         virtual void DrawImplementation(EditorState& state) = 0;
//         ImGuiWindowFlags m_WindowFlags = 0;

//         virtual void PushStyles() {}
//         int m_StyleVarCount = 0;
// };