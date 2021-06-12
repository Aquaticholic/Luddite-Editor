cmake_minimum_required (VERSION 3.6)

project(Luddite-Editor LANGUAGES CXX VERSION 0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Luddite-Engine)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/RuntimeCompiledCPlusPlus/Aurora)

if(LINUX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -DCOMPILE_PATH=\"\\\"${CMAKE_BINARY_DIR}\\\"\"")
endif()

file(GLOB_RECURSE EDITOR_SOURCE "src/*.c" "src/*.cc" "src/*.cpp" "src/*.h" "src/*.hpp")
if(WINDOWS)
add_executable(Editor WIN32 ${EDITOR_SOURCE})
else()
add_executable(Editor ${EDITOR_SOURCE})
endif()

set(ENGINE_INCLUDE_DIR_STRING "${ENGINE_INCLUDE_DIR_STRING}ADD_DIR(${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/Editor.dir/) ")
foreach(path IN LISTS ENGINE_INCLUDE)
set(ENGINE_INCLUDE_DIR_STRING "${ENGINE_INCLUDE_DIR_STRING}ADD_DIR(${path}) ")
endforeach(path IN ENGINE_INCLUDE)
set(ENGINE_INCLUDE_DIR_STRING "${ENGINE_INCLUDE_DIR_STRING}ADD_DIR(${CMAKE_CURRENT_SOURCE_DIR}/src)")
message("paths: ${ENGINE_INCLUDE_DIR_STRING}")

set(ENGINE_LIBRARY_DIR_STRING "${ENGINE_LIBRARY_DIR_STRING}ADD_DIR(${CMAKE_CURRENT_BINARY_DIR}/Luddite-Engine)")


target_compile_definitions(Editor PRIVATE 
${ENGINE_PLATFORM_DEFINE} 
${ENGINE_CONFIGURATION_DEFINE} 
ENGINE_INCLUDE_DIRS=${ENGINE_INCLUDE_DIR_STRING}
ENGINE_LIBRARY_DIRS=${ENGINE_LIBRARY_DIR_STRING}
)

target_link_libraries(Editor 
Luddite-Engine
RuntimeCompiler
RuntimeObjectSystem
)

target_include_directories(Editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src 
"${ENGINE_INCLUDE}"
"ThirdParty/RuntimeCompiledCPlusPlus/Aurora/RuntimeCompiler"
"ThirdParty/RuntimeCompiledCPlusPlus/Aurora/RuntimeObjectSystem"
)

target_precompile_headers(Editor PUBLIC src/Editor/pch.hpp)

# file(GET_RUNTIME_DEPENDENCIES LIBRARIES RUNTIME_DEPENDENCIES)
# message(${RUNTIME_DEPENDENCIES})

# file(COPY ${CMAKE_CURRENT_BINARY_DIR}/Luddite-Engine/Assets DESTINATION ${CMAKE_SOURCE_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Luddite-Engine/Assets DESTINATION ${CMAKE_SOURCE_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})