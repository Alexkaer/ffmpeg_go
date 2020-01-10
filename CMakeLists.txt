cmake_minimum_required(VERSION 3.10)
project(ffmpeg_go)

set(CMAKE_CXX_STANDARD 11)

# 添加头文件搜索路径
include_directories(include/)

# 指定target_link_libraries 的库的目录，否则会搜索全路径
link_directories(libs/)

aux_source_directory(src/ SRC_DIR)

add_executable(ffmpeg_go ${SRC_DIR})






target_link_libraries(
        ffmpeg_go

        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
        SDL2
)

add_executable(dumpinfo ./dumpinfo/dumpinfo.cpp)

target_link_libraries(
        dumpinfo

        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
        SDL2
)



add_executable(extractaudio ./extractaudio/extractAAC.cpp)

target_link_libraries(
        extractaudio

        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
        SDL2
)

add_executable(hellosdl ./sdl2demo/hellosdl2.cpp)

target_link_libraries(
        hellosdl
        SDL2
)


add_executable(sdlvideo sdlvideo/sdlplayvideo.cpp)
target_link_libraries(
        sdlvideo

        avcodec
        avdevice
        avfilter
        avformat
        avresample
        avutil
        postproc
        swresample
        swscale
        SDL2
)