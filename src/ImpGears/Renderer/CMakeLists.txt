cmake_minimum_required(VERSION 3.0)

add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

set(IG_RENDERER ImpGears/Renderer)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_RENDERER}/*.cpp
    ${PROJECT_SOURCE_DIR}/src/${IG_RENDERER}/*.h
    )

file(GLOB IG_RENDERER_H
    ${PROJECT_SOURCE_DIR}/include/${IG_RENDERER}/*.h
    )

add_library(impgears-renderer SHARED ${SRCS} ${IG_RENDERER_H})
target_include_directories(impgears-renderer PUBLIC ${PROJECT_SOURCE_DIR}/include/ImpGears)

install(TARGETS impgears-renderer DESTINATION lib)
install(FILES ${IG_RENDERER_H} DESTINATION include/${IG_RENDERER})
