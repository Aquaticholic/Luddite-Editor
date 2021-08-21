#pragma once
#include "Editor/pch.hpp"
typedef void* HMODULE;

namespace Editor
{
class GameInstanceCompiler
{
        public:
        void Compile();
        void Update(float delta_time);
        void FreeOldLib();
        void FreeOldGameInstance(Luddite::IGameInstance* pGameInstance);
        Luddite::IGameInstance* GetNewGameInstance();
        void LoadNewLib();
        inline void SetProjectDirectory(const std::filesystem::path& project_dir) {m_ProjectDir = project_dir;}
        inline bool IsCompilationDone() const {return m_CompilationReady;}
        inline bool IsCompiling() const {return m_CurrentlyCompiling;}
        private:
        std::filesystem::path m_ProjectDir;
        std::future<void> m_CompileFuture;
        bool m_CompilationReady = false;
        bool m_CurrentlyCompiling = false;
        bool m_RecompileWhenDone = false;
        HMODULE m_OldLib = nullptr;
        HMODULE m_NewLib = nullptr;
};
}