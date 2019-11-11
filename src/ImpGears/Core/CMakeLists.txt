cmake_minimum_required(VERSION 3.0)

set(IG_CORE ImpGears/Core)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_CORE}/*.cpp
    )

file(GLOB IG_CORE_H
    ${PROJECT_SOURCE_DIR}/include/${IG_CORE}/*.h
    )

add_library(impgears-core SHARED ${SRCS} ${IG_CORE_H})

install(TARGETS impgears-core DESTINATION lib)
install(FILES ${IG_CORE_H} DESTINATION include/${IG_CORE})
