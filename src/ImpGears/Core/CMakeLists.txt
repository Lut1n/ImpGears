cmake_minimum_required(VERSION 3.0)

add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

set(IG_CORE ImpGears/Core)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_CORE}/*.cpp
    )

file(GLOB IG_CORE_H
    ${PROJECT_SOURCE_DIR}/include/${IG_CORE}/*.h
    )

add_library(impgears-core SHARED ${SRCS} ${IG_CORE_H})
target_include_directories(impgears-core PUBLIC ${PROJECT_SOURCE_DIR}/include/ImpGears)

install(TARGETS impgears-core DESTINATION lib)
install(FILES ${IG_CORE_H} DESTINATION include/${IG_CORE})
