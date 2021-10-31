#pragma once
#include "Editor/pch.hpp"
#include "Luddite/Core/Event.hpp"

namespace Editor
{
struct KeyBind
{
        uint32_t KeyCode;
        uint16_t Mods;
        inline bool operator==(const KeyBind& other) const
        {
                return (this->KeyCode == other.KeyCode) && (this->Mods == other.Mods);
        }
};

//Identifier, Name, Default KeyCode, Default Mods
#define KEYBINDS_DECLARE \
        KEYBIND_DECLARATION(Undo, "Undo", Luddite::IO::GetKeyCode(Luddite::Keys::Z), Luddite::IO::GetModBit(Luddite::Keys::ControlLeft)) \
        KEYBIND_DECLARATION(Redo, "Redo", Luddite::IO::GetKeyCode(Luddite::Keys::Z), Luddite::IO::GetModBit(Luddite::Keys::ControlLeft) | Luddite::IO::GetModBit(Luddite::Keys::ShiftLeft)) \
        KEYBIND_DECLARATION(TranslateTool, "Use Translate Tool", Luddite::IO::GetKeyCode(Luddite::Keys::W), 0) \
        KEYBIND_DECLARATION(RotateTool, "Use Rotate Tool", Luddite::IO::GetKeyCode(Luddite::Keys::E), 0) \
        KEYBIND_DECLARATION(ScaleTool, "Use Scale Tool", Luddite::IO::GetKeyCode(Luddite::Keys::R), 0) \
        KEYBIND_DECLARATION(ResetTranslation, "Reset Translation", Luddite::IO::GetKeyCode(Luddite::Keys::W), Luddite::IO::GetModBit(Luddite::Keys::AltLeft)) \
        KEYBIND_DECLARATION(ResetRotation, "Reset Rotation", Luddite::IO::GetKeyCode(Luddite::Keys::E), Luddite::IO::GetModBit(Luddite::Keys::AltLeft)) \
        KEYBIND_DECLARATION(ResetScale, "Reset Scale", Luddite::IO::GetKeyCode(Luddite::Keys::R), Luddite::IO::GetModBit(Luddite::Keys::AltLeft)) \
        KEYBIND_DECLARATION(Rename, "Rename", Luddite::IO::GetKeyCode(Luddite::Keys::F2), 0) \
        KEYBIND_DECLARATION(CenterCamera, "Center Camera", Luddite::IO::GetKeyCode(Luddite::Keys::F), 0) \
        KEYBIND_DECLARATION(ProfilingToggle, "Start/Stop Profiling", Luddite::IO::GetKeyCode(Luddite::Keys::P), 0)


enum class eKeyBinds : uint16_t
{
            #define KEYBIND_DECLARATION(ID, Name, DefaultKey, DefaultMods) ID,
        KEYBINDS_DECLARE
            #undef KEYBIND_DECLARATION
        SIZE
};

const char* KeyBindNames[] =
{
          #define KEYBIND_DECLARATION(ID, Name, DefaultKey, DefaultMods) Name,
        KEYBINDS_DECLARE
           #undef KEYBIND_DECLARATION
};

const KeyBind KeyBindDefaults[] =
{
            #define KEYBIND_DECLARATION(ID, Name, DefaultKey, DefaultMods) KeyBind{DefaultKey, DefaultMods},
        KEYBINDS_DECLARE
            #undef KEYBIND_DECLARATION
};

struct KeyBinds
{
        static void Init()
        {
                m_ModBits = 0;
                uint16_t num_key_binds = static_cast<uint16_t>(eKeyBinds::SIZE);
                for (uint16_t i = 0; i < num_key_binds; i++)
                {
                        KeyBind bind = KeyBindDefaults[i];
                        m_Binds[i] = bind;
                        // m_Binds.insert({bind, static_cast<eKeyBinds>(i)});
                }
        }

        static void Update()
        {
                memset(m_PressedBinds, false, sizeof(m_PressedBinds));
                for (auto& event : Luddite::Events::GetList<Luddite::KeyReleaseEvent>())
                {
                        uint16_t mod_bit = Luddite::IO::GetModBit(event.key_code);
                        if (mod_bit > 0)
                                UnsetModBit(mod_bit);
                }


                for (auto& event : Luddite::Events::GetList<Luddite::KeyPressEvent>())
                {
                        uint16_t mod_bit = Luddite::IO::GetModBit(event.key_code);
                        if (mod_bit > 0)
                                SetModBit(mod_bit);
                        else
                        {
                                KeyBind bind{Luddite::IO::GetKeyCode(event.key_code), m_ModBits};
                                auto it = std::find(m_Binds.begin(), m_Binds.end(), bind);
                                if (it != m_Binds.end())
                                {
                                        m_PressedBinds[it - m_Binds.begin()] = true;
                                }
                                event.SetHandled();
                        }
                }
        }

        static void SetModBit(uint16_t bit)
        {
                m_ModBits |= bit;
        }

        static void UnsetModBit(uint16_t bit)
        {
                m_ModBits &= ~bit;
        }

        static bool Pressed(const eKeyBinds& bind)
        {
                return m_PressedBinds[static_cast<uint16_t>(bind)];
        }

        static bool ModsPressed(const uint16_t& mods)
        {
                return m_ModBits == mods;
                // return (m_ModBits & mods) == mods;
        }
        static inline std::array<KeyBind, static_cast<uint16_t>(eKeyBinds::SIZE)> m_Binds;
        static inline bool m_PressedBinds[static_cast<uint16_t>(eKeyBinds::SIZE)];
        static inline uint16_t m_ModBits;
};
}
