cmake_minimum_required(VERSION 3.0)

set(IG_DESCRIPTORS ImpGears/Descriptors)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_DESCRIPTORS}/*.cpp
    )

file(GLOB IG_DESCRIPTORS_H
    ${PROJECT_SOURCE_DIR}/include/${IG_DESCRIPTORS}/*.h
    )

add_library(impgears-descriptors SHARED ${SRCS} ${IG_DESCRIPTORS_H})

target_link_libraries( impgears-descriptors impgears-geometry )
target_link_libraries( impgears-descriptors impgears-graphics )
target_link_libraries( impgears-descriptors impgears-scenegraph )

install(TARGETS impgears-descriptors DESTINATION lib)
install(FILES ${IG_DESCRIPTORS_H} DESTINATION include/${IG_DESCRIPTORS})
