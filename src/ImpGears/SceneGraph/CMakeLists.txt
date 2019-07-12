cmake_minimum_required(VERSION 3.0)

add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

set(IG_SCENEGRAPH ImpGears/SceneGraph)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_SCENEGRAPH}/*.cpp
    )

file(GLOB IG_SCENEGRAPH_H
    ${PROJECT_SOURCE_DIR}/include/${IG_SCENEGRAPH}/*.h
    )

add_library(impgears-scenegraph SHARED ${SRCS} ${IG_SCENEGRAPH_H})
target_include_directories(impgears-scenegraph PUBLIC ${PROJECT_SOURCE_DIR}/include/ImpGears)

install(TARGETS impgears-scenegraph DESTINATION lib)
install(FILES ${IG_SCENEGRAPH_H} DESTINATION include/${IG_SCENEGRAPH})
