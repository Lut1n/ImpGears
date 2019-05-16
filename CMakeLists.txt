cmake_minimum_required(VERSION 3.0)

project(libimpgears)

add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

file(GLOB SRCS 
    ${PROJECT_SOURCE_DIR}/src/ImpGears/Core/*.cpp
    ${PROJECT_SOURCE_DIR}/src/ImpGears/Geometry/*.cpp
    ${PROJECT_SOURCE_DIR}/src/ImpGears/Graphics/*.cpp
    ${PROJECT_SOURCE_DIR}/src/ImpGears/SceneGraph/*.cpp
    ${PROJECT_SOURCE_DIR}/src/ImpGears/Utils/*.cpp
    ${PROJECT_SOURCE_DIR}/src/ImpGears/Core/*.h
    ${PROJECT_SOURCE_DIR}/src/ImpGears/Geometry/*.h
    ${PROJECT_SOURCE_DIR}/src/ImpGears/Graphics/*.h
    ${PROJECT_SOURCE_DIR}/src/ImpGears/SceneGraph/*.h
    ${PROJECT_SOURCE_DIR}/src/ImpGears/Utils/*.h
    )

file(GLOB HEADERS 
    ${PROJECT_SOURCE_DIR}/include/ImpGears/Core/*.h
    ${PROJECT_SOURCE_DIR}/include/ImpGears/Geometry/*.h
    ${PROJECT_SOURCE_DIR}/include/ImpGears/Graphics/*.h
    ${PROJECT_SOURCE_DIR}/include/ImpGears/SceneGraph/*.h
    ${PROJECT_SOURCE_DIR}/include/ImpGears/Utils/*.h
    )

add_library(impgears SHARED ${SRCS} ${HEADERS})
target_include_directories(impgears PUBLIC ${PROJECT_SOURCE_DIR}/include/ImpGears)


# link to DL
target_link_libraries( impgears ${CMAKE_DL_LIBS} )
