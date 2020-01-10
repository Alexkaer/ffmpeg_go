//
// Created by 许双 on 2020/1/9.
//


/**
 * 打印音视频信息
 */

extern "C"{
#include <libavformat/avformat.h>
#include <libavutil/log.h>
}

int main(){


    int ret;

    AVFormatContext *avFormatContext=NULL;

    ret = avformat_open_input(&avFormatContext,"../videos/test.mp4",NULL,NULL);

    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"avformat open input error:%s",av_err2str(ret));
    }

    int index = av_find_best_stream(avFormatContext,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);

    //dump 出avformatcontext中的音视频流信息
    av_dump_format(avFormatContext,index,"../videos/test.mp4",0);

    avformat_close_input(&avFormatContext);






}