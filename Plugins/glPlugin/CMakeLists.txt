cmake_minimum_required(VERSION 3.0)

project(libimpgears-glPlugin)

add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

file(GLOB SRCS 
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADERS 
    ${PROJECT_SOURCE_DIR}/*.h
    )

add_library(glPlugin SHARED ${SRCS} ${HEADERS})
target_include_directories(glPlugin PRIVATE ${PROJECT_SOURCE_DIR}/.)
target_include_directories(glPlugin PRIVATE ${PROJECT_SOURCE_DIR}/../../include/ImpGears)


# link to openGL
find_package( OpenGL REQUIRED )
include_directories( ${OpenGL_INCLUDE_DIRS} )
target_link_libraries( glPlugin ${OpenGL_LIBRARIES} )

#link to ogl extension wrangler
find_package( GLEW REQUIRED )
include_directories( ${GLEW_INCLUDE_DIRS} )
target_link_libraries( glPlugin ${GLEW_LIBRARIES} )
 
