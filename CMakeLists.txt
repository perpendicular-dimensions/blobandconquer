
cmake_minimum_required(VERSION 3.16)

project(blobAndConquer VERSION 1.11.0)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(OpenGL_GL_PREFERENCE LEGACY)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(X11 REQUIRED)
find_package(ZLIB REQUIRED)

include(GNUInstallDirs)

file(GLOB_RECURSE SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

# remove broken and unrelated files
list(REMOVE_ITEM SRC_FILES src/cplusplus/CFeatureDef.cpp)
list(REMOVE_ITEM SRC_FILES src/io/localeLoader.cpp)
list(REMOVE_ITEM SRC_FILES src/pak.cpp)

set(GAME_DATADIR ${CMAKE_INSTALL_DATADIR}/games/${PROJECT_NAME})
set(GAME_DATADIR_FULL ${CMAKE_INSTALL_FULL_DATADIR}/games/${PROJECT_NAME})

add_executable(blobAndConquer ${SRC_FILES})

target_compile_definitions(blobAndConquer
    PRIVATE
        VERSION=${CMAKE_PROJECT_VERSION_MAJOR}-${CMAKE_PROJECT_VERSION_MINOR}
        RELEASE=${CMAKE_PROJECT_VERSION_PATCH}
        LOCALEDIR="${LOCALEDIR}"
        USEPAK=0
        DATADIR="${GAME_DATADIR_FULL}"
)

target_link_libraries(blobAndConquer
    PRIVATE
        X11::X11
        OpenGL::GL
        OpenGL::GLU
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
        ZLIB::ZLIB
)

install(TARGETS blobAndConquer)

foreach(asset IN ITEMS data gfx music sound textures)
    install(DIRECTORY ${asset} DESTINATION ${GAME_DATADIR})
endforeach()
