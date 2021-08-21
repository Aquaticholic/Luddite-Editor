#include "Editor/GameInstanceCompiler.hpp"

//Posix
#include <unistd.h>
#include <sys/wait.h>
#include <dlfcn.h>

namespace Editor
{
void GameInstanceCompiler::Update(float delta_time)
{
        if (m_CurrentlyCompiling)
                if (m_CompileFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                {
                        m_CurrentlyCompiling = false;
                        m_CompilationReady = true;
                        m_CompileFuture = std::future<void>{};
                }
        if (!m_CurrentlyCompiling && m_RecompileWhenDone)
        {
                m_RecompileWhenDone = false;
                Compile();
        }
}

void GameInstanceCompiler::FreeOldLib()
{
        if (m_OldLib)
        {
                dlclose(m_OldLib);
                m_OldLib = nullptr;
        }
}

void GameInstanceCompiler::FreeOldGameInstance(Luddite::IGameInstance* pGameInstance)
{
        #ifdef _WIN32
        typedef void (__cdecl *FreeGameInstance_PROC)(Luddite::IGameInstance*);
        #else
        typedef void ( *FreeGameInstance_PROC)(Luddite::IGameInstance*);
        #endif
        auto proc = (FreeGameInstance_PROC)dlsym(m_OldLib, "_Z16FreeGameInstancePN7Luddite13IGameInstanceE");
        proc(pGameInstance);
}

Luddite::IGameInstance* GameInstanceCompiler::GetNewGameInstance()
{
        #ifdef _WIN32
        typedef Luddite::IGameInstance* (__cdecl *GetNewInstance_PROC)();
        #else
        typedef Luddite::IGameInstance* ( *GetNewInstance_PROC)();
        #endif
        auto proc = (GetNewInstance_PROC)dlsym(m_NewLib, "_Z15NewGameInstancev");
        return proc();
}

void GameInstanceCompiler::LoadNewLib()
{
        m_CompilationReady = false;
        m_OldLib = m_NewLib;
        m_NewLib = dlopen((m_ProjectDir / "bin/libGame.so").string().c_str(), RTLD_NOW);
}

void GameInstanceCompiler::Compile()
{
        if (m_CurrentlyCompiling)
        {
                m_RecompileWhenDone = true;
                return;
        }
        m_CurrentlyCompiling = true;
        std::filesystem::path include_dir = std::filesystem::current_path() / "Dev/include";
        m_CompileFuture = std::async(std::launch::async, [&]() {
                        pid_t retPID;
                        switch (retPID = fork())
                        {
                        case 0:
                                {
                                        std::string build_string = "cmake -DCMAKE_BUILD_TYPE:STRING=Debug -H";
                                        build_string += m_ProjectDir.string().c_str();
                                        build_string += " -B";
                                        build_string += m_ProjectDir.string().c_str();
                                        build_string += "/bin";
                                        build_string += " -G Ninja";
                                        build_string += " -DENGINE_INCLUDE:STRING=\"" ENGINE_INCLUDE_DIR "\"";
                                        build_string += " -DENGINE_PLATFORM_DEFINE=\"" LD_PLATFORM "\"";
                                        build_string += " -DENGINE_CONFIGURATION_DEFINE=\"" LD_CONFIGURATION "\"";
                                        // execl("/bin/sh", "sh ", "-c", build_string.c_str(), compile_string.c_str(), (const char*)NULL);
                                        execl("/bin/sh", "sh ", "-c", build_string.c_str(), (const char*)NULL);
                                }
                                break;

                        default:
                                {
                                        int status;
                                        waitpid(retPID, &status, 0);
                                        switch (retPID = fork())
                                        {
                                        case 0:
                                                {
                                                        std::string compile_string = "cmake --build ";
                                                        compile_string += m_ProjectDir.string().c_str();
                                                        compile_string += "/bin --config Debug --target Game -j 10";
                                                        execl("/bin/sh", "sh ", "-c", compile_string.c_str(), (const char*)NULL);
                                                }

                                        default:
                                                waitpid(retPID, &status, 0);
                                                break;
                                        }
                                }
                                return;
                        }
                });
}
}