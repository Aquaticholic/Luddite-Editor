cmake_minimum_required (VERSION 3.6)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

project(Luddite-Editor LANGUAGES CXX VERSION 0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(LINUX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fvisibility=hidden -shared -DCOMPILE_PATH=\"\\\"${CMAKE_BINARY_DIR}\\\"\"")
endif()

file(GLOB_RECURSE GAME_SOURCE "src/*.c" "src/*.cc" "src/*.cpp" "src/*.h" "src/*.hpp")
if(WINDOWS)
add_library(Game SHARED WIN32 ${GAME_SOURCE})
else()
add_library(Game SHARED ${GAME_SOURCE})
endif()

target_compile_definitions(Game PRIVATE 
${ENGINE_PLATFORM_DEFINE} 
${ENGINE_CONFIGURATION_DEFINE} 
)

# target_link_libraries(Game
# Luddite-Engine
# RuntimeCompiler
# RuntimeObjectSystem
# )

MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

SUBDIRLIST(ENGINE_INCLUDE_PATHS ${ENGINE_INCLUDE})
target_include_directories(Game PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
foreach(ENGINE_INCLUDE_PATH ${ENGINE_INCLUDE_PATHS})
    target_include_directories(Game PRIVATE "${ENGINE_INCLUDE_PATH}")
endforeach(ENGINE_INCLUDE_PATH ${ENGINE_INCLUDE_PATHS})


target_precompile_headers(Game PUBLIC src/pch.hpp)