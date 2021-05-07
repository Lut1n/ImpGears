cmake_minimum_required(VERSION 3.0)

project(libimpgears)

option(BUILD_BENCH_FOLDER "Build bench folder" OFF)
option(BUILD_TEST_FOLDER "Build test folder" OFF)
option(BUILD_EXAMPLE_FOLDER "Build example folder" OFF)
option(BUILD_GLPLUGIN "Build the OpenGL plugin" ON)
option(USE_DLFCN_WIN32 "Use the dlfcn-win32 for Mingw" OFF)
#option(USE_EXTERNAL_GLEW_LIB "Use an external glew library" OFF)
option(PROFILING_OPT "Build with the profiling option (to be used with gprof)" OFF)

set(CMAKE_CXX_STANDARD 17)

if(PROFILING_OPT)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()

add_definitions("-Wall" "-g" "-fvisibility=hidden")
add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")

include_directories(${PROJECT_SOURCE_DIR}/include)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src/ImpGears/Core)
add_subdirectory(src/ImpGears/Geometry)
add_subdirectory(src/ImpGears/Graphics)
add_subdirectory(src/ImpGears/SceneGraph)
add_subdirectory(src/ImpGears/Renderer)
add_subdirectory(src/ImpGears/Descriptors)
add_subdirectory(src/ImpGears/Plugins)

file(GLOB IMPGEARS_API_H
${PROJECT_SOURCE_DIR}/include/ImpGears/*.h
)

install(FILES ${IMPGEARS_API_H} DESTINATION include/ImpGears/)

macro(IG_SETUP_EXE IG_EXE_NAME IG_SRC_DIR SRCS)

    include_directories(AFTER ${CMAKE_SOURCE_DIR}/include)

    # executable
    add_executable(${EXE_NAME} ${SRCS})
    set_property(TARGET ${EXE_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${IG_SRC_DIR})

    # link to impgears
    target_link_libraries(${EXE_NAME} impgears-core)
    target_link_libraries(${EXE_NAME} impgears-descriptors)
    target_link_libraries(${EXE_NAME} impgears-geometry)
    target_link_libraries(${EXE_NAME} impgears-graphics)
    target_link_libraries(${EXE_NAME} impgears-plugins)
    target_link_libraries(${EXE_NAME} impgears-renderer)
    target_link_libraries(${EXE_NAME} impgears-scenegraph)
    target_link_libraries(${EXE_NAME} OGLPlugin)
    
    # glad (gl extensions loader used. Can be used to blit texture when rendering offscreen)
    include_directories("${CMAKE_SOURCE_DIR}/thirdparty/glad/include")
    include_directories("${CMAKE_SOURCE_DIR}/thirdparty/glad/src")

    ## In most cases better set in the CMake cache
    #set(SFML_DIR "${PROJECT_SOURCE_DIR}/thirdparty/SFML-2.5.1/lib/cmake/SFML")
    ## If you want to link SFML statically
    #set(SFML_STATIC_LIBRARIES TRUE)
    #find_package(SFML 2.5.1 COMPONENTS graphics REQUIRED)
    #target_link_libraries(${EXE_NAME} sfml-graphics)
    #set(EXTERNAL_LIBS ${PROJECT_SOURCE_DIR}/thirdparty/SFML-2.5.1/bin)

    #link to glfw
    # Help to find glfw3 config on Windows (May need to build it from source)
    set(glfw3_DIR "C:/Program Files (x86)/glfw/lib/cmake/glfw3")
    find_package(glfw3 3.3 REQUIRED CONFIG)
    target_link_libraries( ${EXE_NAME} glfw )

    # link to openGL
    find_package( OpenGL REQUIRED )
    include_directories( ${OPENGL_INCLUDE_DIR} )
    target_link_libraries( ${EXE_NAME} ${OPENGL_LIBRARIES} )

    # eg. commands: copy_directory, copy_if_different
    #add_custom_command(TARGET ${EXE_NAME} POST_BUILD
    #    COMMAND ${CMAKE_COMMAND} -E copy_directory
    #        ${EXTERNAL_LIBS}
    #        $<TARGET_FILE_DIR:${EXE_NAME}>)
endmacro()

if(BUILD_GLPLUGIN)
    add_subdirectory(src/ImpGears/OGLPlugin)
endif()

if(BUILD_BENCH_FOLDER)
    add_subdirectory(benchs/warehouse)
    add_subdirectory(benchs/imgProcessing)
    add_subdirectory(benchs/cpuRendering)
    add_subdirectory(benchs/terrain)
    add_subdirectory(benchs/polygon)
    add_subdirectory(benchs/freecam)
    add_subdirectory(benchs/fovTest)
    add_subdirectory(benchs/particules)
    add_subdirectory(benchs/RTRenderer)
endif()

if(BUILD_TEST_FOLDER)
    add_subdirectory(tests/grids)
    add_subdirectory(tests/image)
    add_subdirectory(tests/matrix)
endif()

if(BUILD_EXAMPLE_FOLDER)
    add_subdirectory(examples/cpuCube)
    add_subdirectory(examples/shadertoyLike)
endif()

message( "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
