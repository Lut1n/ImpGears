cmake_minimum_required(VERSION 3.0)

add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

set(IG_GEOMETRY ImpGears/Geometry)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_GEOMETRY}/*.cpp
    )

file(GLOB IG_GEOMETRY_H
    ${PROJECT_SOURCE_DIR}/include/${IG_GEOMETRY}/*.h
    )

add_library(impgears-geometry SHARED ${SRCS} ${IG_GEOMETRY_H})
target_include_directories(impgears-geometry PUBLIC ${PROJECT_SOURCE_DIR}/include/ImpGears)

install(TARGETS impgears-geometry DESTINATION lib)
install(FILES ${IG_GEOMETRY_H} DESTINATION include/${IG_GEOMETRY})
