cmake_minimum_required(VERSION 3.0)

add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

set(IG_DESCRIPTORS ImpGears/Descriptors)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_DESCRIPTORS}/*.cpp
    )

file(GLOB IG_DESCRIPTORS_H
    ${PROJECT_SOURCE_DIR}/include/${IG_DESCRIPTORS}/*.h
    )

add_library(impgears-descriptors SHARED ${SRCS} ${IG_DESCRIPTORS_H})
target_include_directories(impgears-descriptors PUBLIC ${PROJECT_SOURCE_DIR}/include/ImpGears)

install(TARGETS impgears-descriptors DESTINATION lib)
install(FILES ${IG_DESCRIPTORS_H} DESTINATION include/${IG_DESCRIPTORS})
