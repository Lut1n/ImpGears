cmake_minimum_required(VERSION 3.0)

set(IG_SCENEGRAPH ImpGears/SceneGraph)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_SCENEGRAPH}/*.cpp
    )

file(GLOB IG_SCENEGRAPH_H
    ${PROJECT_SOURCE_DIR}/include/${IG_SCENEGRAPH}/*.h
    )

add_library(impgears-scenegraph SHARED ${SRCS} ${IG_SCENEGRAPH_H})

target_link_libraries( impgears-scenegraph impgears-graphics )
target_link_libraries( impgears-scenegraph impgears-geometry )

install(TARGETS impgears-scenegraph DESTINATION lib)
install(FILES ${IG_SCENEGRAPH_H} DESTINATION include/${IG_SCENEGRAPH})
