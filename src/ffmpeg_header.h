//
// Created by 许双 on 2020/1/8.
//

#ifndef FFMPEG_GO_FFMPEG_HEADER_H
#define FFMPEG_GO_FFMPEG_HEADER_H

extern "C"{
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>
#include <libavutil/fifo.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/log.h>
#include <SDL2/SDL.h>
};

#include <iostream>

#endif //FFMPEG_GO_FFMPEG_HEADER_H
