cmake_minimum_required(VERSION 3.0)

project(libimpgears)

option(BUILD_GLPLUGIN "Build the OpenGL plugin" ON)
option(USE_DLFCN_WIN32 "Use the dlfcn-win32 for Mingw" OFF)
#option(USE_EXTERNAL_GLEW_LIB "Use an external glew library" OFF)
option(PROFILING_OPT "Build with the profiling option (to be used with gprof)" OFF)

set(CMAKE_CXX_STANDARD 17)

if(PROFILING_OPT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()

add_definitions("-Wall" "-g" "-fvisibility=hidden")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src/ImpGears/Core)
add_subdirectory(src/ImpGears/Geometry)
add_subdirectory(src/ImpGears/Graphics)
add_subdirectory(src/ImpGears/SceneGraph)
add_subdirectory(src/ImpGears/Renderer)
add_subdirectory(src/ImpGears/Descriptors)
add_subdirectory(src/ImpGears/Plugins)

file(GLOB IMPGEARS_API_H
${PROJECT_SOURCE_DIR}/include/ImpGears/*.h
)

install(FILES ${IMPGEARS_API_H} DESTINATION include/ImpGears/)

if(BUILD_GLPLUGIN)
    add_subdirectory(src/OGLPlugin)
endif()


message( "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
