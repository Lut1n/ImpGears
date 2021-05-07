cmake_minimum_required(VERSION 3.0)

set(IG_GL ImpGears/RendererGL)

file(GLOB SRCS 
    ${PROJECT_SOURCE_DIR}/src/${IG_GL}/*.cpp
    )

file(GLOB IG_GL_H 
    ${PROJECT_SOURCE_DIR}/include/${IG_GL}/*.h
    )

#link_directories(AFTER "/usr/lib")
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/include/ImpGears")

add_library(impgears-renderer-gl SHARED ${SRCS} ${IG_GL_H})

target_link_libraries(impgears-renderer-gl impgears-core )
target_link_libraries(impgears-renderer-gl impgears-geometry )
target_link_libraries(impgears-renderer-gl impgears-graphics )
target_link_libraries(impgears-renderer-gl impgears-scenegraph )
target_link_libraries(impgears-renderer-gl impgears-renderer )
target_link_libraries(impgears-renderer-gl impgears-plugins )
target_link_libraries(impgears-renderer-gl impgears-descriptors )


include_directories("${CMAKE_SOURCE_DIR}/thirdparty/glad/include")
include_directories("${CMAKE_SOURCE_DIR}/thirdparty/glad/src")

# link to openGL
find_package( OpenGL REQUIRED )
include_directories( ${OPENGL_INCLUDE_DIR} )
target_link_libraries( impgears-renderer-gl ${OPENGL_LIBRARIES} )

install(TARGETS impgears-renderer-gl DESTINATION lib)
install(FILES ${IG_GL_H} DESTINATION include/ImpGears/${IG_GL})