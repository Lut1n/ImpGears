cmake_minimum_required(VERSION 3.0)

add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

set(IG_PLUGINS ImpGears/Plugins)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_PLUGINS}/*.cpp
    )

file(GLOB IG_PLUGINS_H
    ${PROJECT_SOURCE_DIR}/include/${IG_PLUGINS}/*.h
    )

add_library(impgears-plugins SHARED ${SRCS} ${IG_PLUGINS_H})
target_include_directories(impgears-plugins PUBLIC ${PROJECT_SOURCE_DIR}/include/ImpGears)

install(TARGETS impgears-plugins DESTINATION lib)
install(FILES ${IG_PLUGINS_H} DESTINATION include/${IG_PLUGINS})
