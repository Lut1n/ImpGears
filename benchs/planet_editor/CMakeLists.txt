cmake_minimum_required(VERSION 3.0)

project(PlantEditor)

option(PROFILING_OPT "Build with the profiling option (to be used with gprof)" OFF)

set(CMAKE_CXX_STANDARD 17)
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")


# add_definitions("-lgdi32" "-limm32" "-lstdc++fs")

# profiling opt
if(PROFILING_OPT)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -pg")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -pg")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g -pg")
endif()

# imgui stuff
set(IMGUI_PATH imgui-master)
set(IMGUI_EXAMPLES_PATH ${IMGUI_PATH}/examples)
set(IMGUI_SOURCES
    ${IMGUI_PATH}/imgui.cpp
   # ${IMGUI_PATH}/imgui_demo.cpp
    ${IMGUI_PATH}/imgui_draw.cpp
    ${IMGUI_PATH}/imgui_widgets.cpp
    ${IMGUI_EXAMPLES_PATH}/imgui_impl_glfw.cpp
    ${IMGUI_EXAMPLES_PATH}/imgui_impl_opengl3.cpp
    # ${IMGUI_EXAMPLES_PATH}/libs/gl3w/GL/gl3w.c
)
include_directories(${IMGUI_PATH})
include_directories(${IMGUI_EXAMPLES_PATH})
#include_directories("/mingw64/include")
add_definitions("-DIMGUI_IMPL_OPENGL_LOADER_GLEW")

add_definitions("-DIMGUI_USE_STB_SPRINTF")

# project sources
set(SRCS 
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/vegetal.h
    ${PROJECT_SOURCE_DIR}/rock.h
)

# search directories
# link_directories(AFTER "/usr/lib")
include_directories(AFTER ${IMPGEARS_INCLUDE_DIRS})
# include_directories("${CMAKE_SOURCE_DIR}/include")
# include_directories("/usr/include/ImpGears")
#include_directories("C:/libimpgears/include")
#include_directories("C:/libimpgears/include/ImpGears")

# executable
add_executable(plantGenerator ${SRCS} ${IMGUI_SOURCES})

# link to impgears
# target_link_libraries(plantGenerator impgears-core )
# target_link_libraries(plantGenerator impgears-geometry )
# target_link_libraries(plantGenerator impgears-graphics )
# target_link_libraries(plantGenerator impgears-scenegraph )
# target_link_libraries(plantGenerator impgears-renderer )
# target_link_libraries(plantGenerator impgears-plugins )
# target_link_libraries(plantGenerator impgears-descriptors )
target_link_libraries(plantGenerator ${IMPGEARS_CORE_LIBRARIES})
target_link_libraries(plantGenerator ${IMPGEARS_DESC_LIBRARIES})
target_link_libraries(plantGenerator ${IMPGEARS_GEOMETRY_LIBRARIES})
target_link_libraries(plantGenerator ${IMPGEARS_GRAPHIC_LIBRARIES})
target_link_libraries(plantGenerator ${IMPGEARS_GRAPH_LIBRARIES})
target_link_libraries(plantGenerator ${IMPGEARS_PLUGIN_LIBRARIES})
target_link_libraries(plantGenerator ${IMPGEARS_RENDERER_LIBRARIES})
target_link_libraries(plantGenerator ${IMPGEARS_GL_LIBRARIES})


message(STATUS ${IMPGEARS_CORE_LIBRARIES})
message(STATUS ${IMPGEARS_DESC_LIBRARIES})
message(STATUS ${IMPGEARS_GEOMETRY_LIBRARIES})
message(STATUS ${IMPGEARS_GRAPHIC_LIBRARIES})
message(STATUS ${IMPGEARS_GRAPH_LIBRARIES})
message(STATUS ${IMPGEARS_PLUGIN_LIBRARIES})
message(STATUS ${IMPGEARS_RENDERER_LIBRARIES})
message(STATUS ${IMPGEARS_GL_LIBRARIES})

# need :
# -lglfw3 -lgdi32 -lopengl32 -limm32 -lstdc++fs
# CXXFLAGS += `pkg-config --cflags glfw3`

#link to glfw
# find_package( GLFW REQUIRED )
include_directories( ${GLFW_INCLUDE_DIRS} )
target_link_libraries( plantGenerator ${GLFW_LIBRARIES} )
# target_link_libraries( plantGenerator glfw3 )
# target_link_libraries( plantGenerator gdi32 )
# target_link_libraries( plantGenerator imm32 )
# target_link_libraries( plantGenerator stdc++fs )

#link to ogl extension wrangler
#find_package( GLEW REQUIRED )
include_directories( ${GLEW_INCLUDE_DIRS} )
target_link_libraries( plantGenerator ${GLEW_LIBRARIES} )

# link to openGL
find_package( OpenGL REQUIRED )
include_directories( ${OPENGL_INCLUDE_DIR} )
target_link_libraries( plantGenerator ${OPENGL_LIBRARIES} )
