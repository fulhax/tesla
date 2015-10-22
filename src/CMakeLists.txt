cmake_minimum_required(VERSION 2.8)

project(game)

include("../cmake/makelistabs.cmake")

list(APPEND SOURCE_FILES
    "./engine.cpp"
    "./errorhandler.cpp"
    "./main.cpp"
    "./video.cpp"
    "./resource.cpp"
    "./resources/tga.cpp"
    "./resources/png.cpp"
    )

list(APPEND INCLUDE_DIRS
    "./"
    )

list(APPEND LIBRARY_DIRS
    )

set(CMAKE_CXX_FLAGS "-Wall -std=c++11")

makelistabs("${INCLUDE_DIRS}" INCLUDE_DIRS)
makelistabs("${LIBRARY_DIRS}" LIBRARY_DIRS)
include_directories(${INCLUDE_DIRS})

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
link_directories(${OPENGL_LIBRARY_DIRS} ${LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIBRARY_DIRS} ${LIBRARY_DIRS})
add_definitions(${SDL2_DEFINITIONS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${PNG_LIBRARIES}
    )
