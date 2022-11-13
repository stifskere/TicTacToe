cmake_minimum_required(VERSION 3.23)
project(TicTacToe)

add_definitions(-DSFML_STATIC)

set(CMAKE_EXE_LINKER_FLAGS "-O3 -static-libgcc -static-libstdc++ -static")
set(CMAKE_CXX_STANDARD 17)

link_directories(lib)
include_directories(include)

add_executable(TicTacToe WIN32 main.cpp)
target_link_libraries(TicTacToe sfml-graphics-s sfml-main sfml-window-s sfml-audio-s sfml-system-s openal32 flac vorbisenc vorbisfile vorbis ogg winmm opengl32 freetype)