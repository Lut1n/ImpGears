cmake_minimum_required(VERSION 3.0)

set(IG_GRAPHICS ImpGears/Graphics)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_GRAPHICS}/*.cpp
    )

file(GLOB IG_GRAPHICS_H
    ${PROJECT_SOURCE_DIR}/include/${IG_GRAPHICS}/*.h
    )

add_library(impgears-graphics SHARED ${SRCS} ${IG_GRAPHICS_H})

target_link_libraries( impgears-graphics impgears-core )

install(TARGETS impgears-graphics DESTINATION lib)
install(FILES ${IG_GRAPHICS_H} DESTINATION include/${IG_GRAPHICS})
