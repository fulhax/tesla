cmake_minimum_required(VERSION 2.8)

project(game)

include("../cmake/makelistabs.cmake")

list(APPEND SOURCE_FILES
    "./engine.cpp"
    "./errorhandler.cpp"
    "./main.cpp"
    "./video.cpp"
    "./resource.cpp"
    "./shader.cpp"
    "./entity.cpp"
    "./script.cpp"
    "./resources/tga.cpp"
    "./resources/png.cpp"
    "./resources/obj.cpp"
    "./resources/glsl.cpp"
    "./resources/as.cpp"
    "../externals/fast_obj_loader/fast_obj_loader.cpp"
    "../externals/as_scriptstdstring/scriptstdstring.cpp"
    "./linux/notify.cpp"
    )

list(APPEND INCLUDE_DIRS
    "./"
    "../externals/fast_obj_loader/"
    "../externals/as_scriptstdstring"
    "./linux"
    )

list(APPEND LIBRARY_DIRS
    )

set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -std=c++11")
add_definitions("-DGL_GLEXT_PROTOTYPES")

option(USEOPENMP "enable openmp mutlithreading" ON)

if(USEOPENMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    endif()
endif()

makelistabs("${INCLUDE_DIRS}" INCLUDE_DIRS)
makelistabs("${LIBRARY_DIRS}" LIBRARY_DIRS)
include_directories(${INCLUDE_DIRS})

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
link_directories(${OPENGL_LIBRARY_DIRS} ${LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

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
    angelscript
    )
