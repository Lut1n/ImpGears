cmake_minimum_required(VERSION 3.0)

set(IG_PLUGINS ImpGears/Plugins)

file(GLOB SRCS
    ${PROJECT_SOURCE_DIR}/src/${IG_PLUGINS}/*.cpp
    )

file(GLOB IG_PLUGINS_H
    ${PROJECT_SOURCE_DIR}/include/${IG_PLUGINS}/*.h
    )

add_library(impgears-plugins SHARED ${SRCS} ${IG_PLUGINS_H})


if (WIN32)
else()
    # link dlfcn
    if (USE_DLFCN_WIN32)
      find_package(dlfcn-win32 REQUIRED)
      set(CMAKE_DL_LIBS dlfcn-win32::dl)
    else()
      set(CMAKE_DL_LIBS dl)
    endif ()
    target_link_libraries(impgears-plugins ${CMAKE_DL_LIBS})
    message(STATUS ${CMAKE_DL_LIBS})
endif()

target_link_libraries( impgears-plugins impgears-core )


install(TARGETS impgears-plugins DESTINATION lib)
install(FILES ${IG_PLUGINS_H} DESTINATION include/${IG_PLUGINS})
