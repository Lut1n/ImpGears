cmake_minimum_required(VERSION 3.0)

set(IG_PLUGINS ImpGears/Plugins)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_PLUGINS}/*.cpp
    )

file(GLOB IG_PLUGINS_H
    ${PROJECT_SOURCE_DIR}/include/${IG_PLUGINS}/*.h
    )

add_library(impgears-plugins SHARED ${SRCS} ${IG_PLUGINS_H})

target_link_libraries( impgears-plugins dl )

install(TARGETS impgears-plugins DESTINATION lib)
install(FILES ${IG_PLUGINS_H} DESTINATION include/${IG_PLUGINS})
