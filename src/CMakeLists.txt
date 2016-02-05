cmake_minimum_required(VERSION 3.4)

project(game)

include("../cmake/makelistabs.cmake")

list(APPEND SOURCE_FILES
    "./engine.cpp"
    "./errorhandler.cpp"
    "./eventhandler.cpp"
    "./main.cpp"
    "./video.cpp"
    "./resource.cpp"
    "./shader.cpp"
    "./entity.cpp"
    "./script.cpp"
    "./audio.cpp"
    "./camera.cpp"
    "./config.cpp"
    "./debugger.cpp"
    "./shapes.cpp"
    "./physics.cpp"
    "./ui.cpp"
    "./sha256.cpp"
    "./resources/tga.cpp"
    "./resources/png.cpp"
    "./resources/obj.cpp"
    "./resources/glsl.cpp"
    "./resources/as.cpp"
    "./resources/ogg.cpp"
    "./resources/freetype.cpp"
    "./resources/ogex.cpp"
    "../externals/fast_obj_loader/fast_obj_loader.cpp"
    "../externals/as_scriptbuilder/scriptbuilder.cpp"
    "../externals/as_scriptstdstring/scriptstdstring.cpp"
    )

list(APPEND INCLUDE_DIRS
    "./"
    "../externals/fast_obj_loader"
    "../externals/as_scriptbuilder"
    "../externals/as_scriptstdstring"
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
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

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas")
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
link_directories(${LIBRARY_DIRS})

find_package(OpenGL REQUIRED)

include_directories(${OPENGL_INCLUDE_DIR})
link_directories(${OPENGL_LIBRARY_DIRS})
add_definitions(${OPENGL_DEFINITIONS})

find_package(Bullet REQUIRED)

include_directories(${BULLET_INCLUDE_DIR})
link_directories(${BULLET_LIBRARY_DIRS})
add_definitions(${BULLET_DEFINITIONS})

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBRARY_DIRS})
add_definitions(${SDL2_DEFINITIONS})

PKG_SEARCH_MODULE(OPENAL REQUIRED openal)
PKG_SEARCH_MODULE(VORBISFILE REQUIRED vorbisfile)

PKG_SEARCH_MODULE(HARFBUZZ REQUIRED harfbuzz)
include_directories(${HARFBUZZ_INCLUDE_DIRS})
link_directories(${HARFBUZZ_LIBRARY_DIRS})
add_definitions(${HARFBUZZ_DEFINITIONS})

PKG_SEARCH_MODULE(FREETYPE2 REQUIRED freetype2)
include_directories(${FREETYPE2_INCLUDE_DIRS})
link_directories(${FREETYPE2_LIBRARY_DIRS})
add_definitions(${FREETYPE2_DEFINITIONS})

PKG_SEARCH_MODULE(OPENDDL REQUIRED openddlparser)
include_directories(${OPENDDL_INCLUDE_DIRS})
link_directories(${OPENDDL_LIBRARY_DIRS})
add_definitions(${OPENDDL_DEFINITIONS})


add_executable(${PROJECT_NAME}
    ${SOURCE_FILES}
    "${CMAKE_CURRENT_BINARY_DIR}/generated/sha256versionfile.cpp"
    )
set_source_files_properties(
    "${CMAKE_CURRENT_BINARY_DIR}/generated/sha256versionfile.cpp"
    PROPERTIES GENERATED TRUE)

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
    ${HARFBUZZ_LIBRARIES}
    ${FREETYPE2_LIBRARIES}
    ${OPENDDL_LIBRARIES}
    ${BULLET_LIBRARIES}
    )

if(DEBUG_PATHS)
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    foreach(dir ${dirs})
        message(STATUS "include dir='${dir}'")
    endforeach()
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY LINK_DIRECTORIES)
    foreach(dir ${dirs})
        message(STATUS "lib dir='${dir}'")
    endforeach()
endif()

set(SHA256FILES
    "./resources/ogex.cpp"
    )
# this is for including the "version" of the sourcefile
# in certain resource loaders for example where you cache
# a binary version of the resource

add_custom_command(
    OUTPUT generated/sha256versionfile.h
    OUTPUT generated/sha256versionfile.cpp
    COMMAND ${CMAKE_COMMAND}
    -DOUTPUTDIR="${CMAKE_CURRENT_BINARY_DIR}"
    -DSOURCES="${SHA256FILES}"
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/version.cmake
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${SHA256FILES})
