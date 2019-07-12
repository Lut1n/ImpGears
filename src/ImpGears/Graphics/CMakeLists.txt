cmake_minimum_required(VERSION 3.0)

add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

set(IG_GRAPHICS ImpGears/Graphics)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_GRAPHICS}/*.cpp
    )

file(GLOB IG_GRAPHICS_H
    ${PROJECT_SOURCE_DIR}/include/${IG_GRAPHICS}/*.h
    )

add_library(impgears-graphics SHARED ${SRCS} ${IG_GRAPHICS_H})
target_include_directories(impgears-graphics PUBLIC ${PROJECT_SOURCE_DIR}/include/ImpGears)

install(TARGETS impgears-graphics DESTINATION lib)
install(FILES ${IG_GRAPHICS_H} DESTINATION include/${IG_GRAPHICS})
