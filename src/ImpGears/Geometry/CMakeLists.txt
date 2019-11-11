cmake_minimum_required(VERSION 3.0)

set(IG_GEOMETRY ImpGears/Geometry)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_GEOMETRY}/*.cpp
    )

file(GLOB IG_GEOMETRY_H
    ${PROJECT_SOURCE_DIR}/include/${IG_GEOMETRY}/*.h
    )

add_library(impgears-geometry SHARED ${SRCS} ${IG_GEOMETRY_H})

target_link_libraries( impgears-geometry impgears-core )

install(TARGETS impgears-geometry DESTINATION lib)
install(FILES ${IG_GEOMETRY_H} DESTINATION include/${IG_GEOMETRY})
