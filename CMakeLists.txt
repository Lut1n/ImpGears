cmake_minimum_required(VERSION 3.0)

project(libimpgears)

option(BUILD_GLPLUGIN "Build the OpenGL plugin" ON)

set(CMAKE_CXX_STANDARD 17)

add_definitions("-Wall" "-Werror" "-g" "-fvisibility=hidden")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

include_directories(${PROJECT_SOURCE_DIR}/include/ImpGears)

add_subdirectory(src/ImpGears/Core)
add_subdirectory(src/ImpGears/Geometry)
add_subdirectory(src/ImpGears/Graphics)
add_subdirectory(src/ImpGears/SceneGraph)
add_subdirectory(src/ImpGears/Renderer)
add_subdirectory(src/ImpGears/Descriptors)
add_subdirectory(src/ImpGears/Plugins)

if(BUILD_GLPLUGIN)
    add_subdirectory(src/OGLPlugin)
endif()


message( "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
