PROJECT(gameEngine)

cmake_minimum_required(VERSION 2.8)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(Boost COMPONENTS filesystem REQUIRED)

set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
set(LIBS ${LIBS} ${OPENGL_LIBRARY} ${SDL2_LIBRARIES} ${GLEW_LIBRARIES} ${Boost_LIBRARIES})

#specify include directory
include_directories(INCLUDE_DIRS ${INCLUDE_DIRS} include)

set( SOURCES
    main.cpp
    camera.cpp
    ShaderProgram.hpp
    objloader.cpp
)

add_executable( ${PROJECT_NAME} ${SOURCES} )

target_link_libraries(${PROJECT_NAME} ${LIBS} SDL2)

#Now copy shaders to build directory
add_custom_target(copy-runtime-files ALL
    COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
    COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    DEPENDS ${MY_TARGET})
