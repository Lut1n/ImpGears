cmake_minimum_required(VERSION 3.0)

set(IG_RENDERER ImpGears/Renderer)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_RENDERER}/*.cpp
    ${PROJECT_SOURCE_DIR}/src/${IG_RENDERER}/*.h
    )

file(GLOB IG_RENDERER_H
    ${PROJECT_SOURCE_DIR}/include/${IG_RENDERER}/*.h
    )

add_library(impgears-renderer SHARED ${SRCS} ${IG_RENDERER_H})

target_link_libraries( impgears-renderer impgears-scenegraph )

install(TARGETS impgears-renderer DESTINATION lib)
install(FILES ${IG_RENDERER_H} DESTINATION include/${IG_RENDERER})
