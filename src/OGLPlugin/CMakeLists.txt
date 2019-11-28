cmake_minimum_required(VERSION 3.0)

project(libimpgears-OGLPlugin)

set(CMAKE_CXX_STANDARD 17)

add_definitions("-Wall" "-Werror" "-g" "-fvisibility=hidden")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

file(GLOB SRCS 
    ${PROJECT_SOURCE_DIR}/*.cpp
    )

file(GLOB HEADERS 
    ${CMAKE_SOURCE_DIR}/include/OGLPlugin/*.h
    )
    
    
link_directories(AFTER "/usr/lib")
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/include/ImpGears")

add_library(OGLPlugin SHARED ${SRCS} ${HEADERS})

target_link_libraries(OGLPlugin impgears-core )
target_link_libraries(OGLPlugin impgears-geometry )
target_link_libraries(OGLPlugin impgears-graphics )
target_link_libraries(OGLPlugin impgears-scenegraph )
target_link_libraries(OGLPlugin impgears-renderer )
target_link_libraries(OGLPlugin impgears-plugins )
target_link_libraries(OGLPlugin impgears-descriptors )

#link to ogl extension wrangler
find_package( GLEW REQUIRED )
include_directories( ${GLEW_INCLUDE_DIRS} )
target_link_libraries( OGLPlugin ${GLEW_LIBRARIES} )

# link to openGL
find_package( OpenGL REQUIRED )
include_directories( ${OPENGL_INCLUDE_DIR} )
target_link_libraries( OGLPlugin ${OPENGL_LIBRARIES} )




macro(print_all arr)
    message(STATUS "------------------------------------------{")
    foreach (_variableName ${arr})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "------------------------------------------}")
endmacro()


print_all(OPENGL_INCLUDE_DIR)
print_all(OPENGL_LIBRARIES)
