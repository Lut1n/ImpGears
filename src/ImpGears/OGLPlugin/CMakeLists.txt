cmake_minimum_required(VERSION 3.0)

set(IG_GL ImpGears/OGLPlugin)

file(GLOB SRCS 
    ${PROJECT_SOURCE_DIR}/src/${IG_GL}/*.cpp
    )

file(GLOB IG_GL_H 
    ${PROJECT_SOURCE_DIR}/include/${IG_GL}/*.h
    )
    
    
#link_directories(AFTER "/usr/lib")
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/include/ImpGears")

add_library(OGLPlugin SHARED ${SRCS} ${IG_GL_H})

target_link_libraries(OGLPlugin impgears-core )
target_link_libraries(OGLPlugin impgears-geometry )
target_link_libraries(OGLPlugin impgears-graphics )
target_link_libraries(OGLPlugin impgears-scenegraph )
target_link_libraries(OGLPlugin impgears-renderer )
target_link_libraries(OGLPlugin impgears-plugins )
target_link_libraries(OGLPlugin impgears-descriptors )


include_directories("${CMAKE_SOURCE_DIR}/thirdparty/glad/include")
include_directories("${CMAKE_SOURCE_DIR}/thirdparty/glad/src")

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

install(TARGETS OGLPlugin DESTINATION lib)
install(FILES ${IG_GL_H} DESTINATION include/ImpGears/${IG_GL})