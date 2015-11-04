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
    "./audio.cpp"
    "./camera.cpp"
    "./debugger.cpp"
    "./resources/tga.cpp"
    "./resources/png.cpp"
    "./resources/obj.cpp"
    "./resources/glsl.cpp"
    "./resources/as.cpp"
    "./resources/ogg.cpp"
    "../externals/fast_obj_loader/fast_obj_loader.cpp"
    "../externals/as_scriptstdstring/scriptstdstring.cpp"
    )

list(APPEND INCLUDE_DIRS
    "./"
    "../externals/fast_obj_loader/"
    "../externals/as_scriptstdstring"
    )

if(WIN32)
    list(APPEND SOURCE_FILES
        "./windows/notify.cpp"
        )
    list(APPEND INCLUDE_DIRS
        "./windows"
        )
    list(APPEND LIBRARY_DIRS
        ${CMAKE_FIND_ROOT_PATH}/lib
        )
else()
    list(APPEND SOURCE_FILES
        "./linux/notify.cpp"
        )
    list(APPEND INCLUDE_DIRS
        "./linux"
        )
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -std=c++11")
add_definitions("-DGL_GLEXT_PROTOTYPES -DGLM_SWIZZLE")

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

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIR})
link_directories(${SDL2_LIBRARY_DIRS} ${LIBRARY_DIRS})
add_definitions(${SDL2_DEFINITIONS})

PKG_SEARCH_MODULE(OPENAL REQUIRED openal)
PKG_SEARCH_MODULE(VORBISFILE REQUIRED vorbisfile)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME}
        mingw32
        gdi32
        gl3w.a
        OpenAL32.dll.a
        )
    # turn off .rsp files so compile_commands.json will work with autocomplete
    set(CMAKE_C_USE_RESPONSE_FILE_FOR_OBJECTS 0)
    set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS 0)
    set(CMAKE_C_USE_RESPONSE_FILE_FOR_INCLUDES 0)
    set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES 0)
else()
    target_link_libraries(${PROJECT_NAME}
        ${OPENAL_LIBRARIES}
        )
endif()

target_link_libraries(${PROJECT_NAME}
    ${VORBISFILE_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${PNG_LIBRARIES}
    angelscript.a
    )
