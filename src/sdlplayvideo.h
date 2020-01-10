//
// Created by 许双 on 2020/1/8.
//

#ifndef FFMPEG_GO_SDLVIDEO_H
#define FFMPEG_GO_SDLVIDEO_H

#include "ffmpeg_header.h"

#define WINDOW_WIDTH 540
#define WINDOW_HEIGHT 960






/** 1.初始化SDL2 **/
void initSDL2();

/** 2.初始化FFmpeg  **/
void preparDecodec(const char *url);

/** 3.解码播放 **/
void decodecFrame();

/** 4.释放资源 **/
void freeContext();

/** 3.1 绘制一帧数据 在 decodecFrame() 中调用 **/
void drawFrame(AVFrame *frame);



/** 播放视频 （外部调用的总方法）**/
void playVideo(const char *url);


#endif //FFMPEG_GO_SDLVIDEO_H
