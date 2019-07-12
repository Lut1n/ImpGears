cmake_minimum_required(VERSION 3.0)

project(libimpgears)

# add_definitions("-Wall" "-Werror" "-g" "-std=c++11 -fvisibility='hidden'" "-g")
# add_definitions("-DIMPGEARS_BUILD_DLL" "-DIMPGEARS_DEBUG")


add_subdirectory(src/ImpGears/Core)
add_subdirectory(src/ImpGears/Geometry)
add_subdirectory(src/ImpGears/Graphics)
add_subdirectory(src/ImpGears/SceneGraph)
add_subdirectory(src/ImpGears/Renderer)
add_subdirectory(src/ImpGears/Descriptors)
add_subdirectory(src/ImpGears/Plugins)

# target_include_directories(impgears PUBLIC ${PROJECT_SOURCE_DIR}/include/ImpGears)

# link to DL
# target_link_libraries( impgears ${CMAKE_DL_LIBS} )


set( CMAKE_INSTALL_PREFIX /usr )
message( "CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}" )
