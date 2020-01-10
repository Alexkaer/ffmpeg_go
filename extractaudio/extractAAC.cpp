//
// Created by 许双 on 2020/1/9.
//


extern "C"{
#include <libavformat/avformat.h>
#include <libavutil/log.h>
#include <stdio.h>
}



#define ADTS_HEADER_LEN  7;

/**
 * 为AAC添加ADTS头
 * @param szAdtsHeader
 * @param dataLen
 */
void adts_header(char *szAdtsHeader, int dataLen){

    int audio_object_type = 2;
    int sampling_frequency_index = 7;
    int channel_config = 2;

    int adtsLen = dataLen + 7;

    szAdtsHeader[0] = 0xff;         //syncword:0xfff                          高8bits
    szAdtsHeader[1] = 0xf0;         //syncword:0xfff                          低4bits
    szAdtsHeader[1] |= (0 << 3);    //MPEG Version:0 for MPEG-4,1 for MPEG-2  1bit
    szAdtsHeader[1] |= (0 << 1);    //Layer:0                                 2bits
    szAdtsHeader[1] |= 1;           //protection absent:1                     1bit

    szAdtsHeader[2] = (audio_object_type - 1)<<6;            //profile:audio_object_type - 1                      2bits
    szAdtsHeader[2] |= (sampling_frequency_index & 0x0f)<<2; //sampling frequency index:sampling_frequency_index  4bits
    szAdtsHeader[2] |= (0 << 1);                             //private bit:0                                      1bit
    szAdtsHeader[2] |= (channel_config & 0x04)>>2;           //channel configuration:channel_config               高1bit

    szAdtsHeader[3] = (channel_config & 0x03)<<6;     //channel configuration:channel_config      低2bits
    szAdtsHeader[3] |= (0 << 5);                      //original：0                               1bit
    szAdtsHeader[3] |= (0 << 4);                      //home：0                                   1bit
    szAdtsHeader[3] |= (0 << 3);                      //copyright id bit：0                       1bit
    szAdtsHeader[3] |= (0 << 2);                      //copyright id start：0                     1bit
    szAdtsHeader[3] |= ((adtsLen & 0x1800) >> 11);           //frame length：value   高2bits

    szAdtsHeader[4] = (uint8_t)((adtsLen & 0x7f8) >> 3);     //frame length:value    中间8bits
    szAdtsHeader[5] = (uint8_t)((adtsLen & 0x7) << 5);       //frame length:value    低3bits
    szAdtsHeader[5] |= 0x1f;                                 //buffer fullness:0x7ff 高5bits
    szAdtsHeader[6] = 0xfc;
}


void extractAAC(const char * input, const char * out){

    FILE *out_file = fopen(out,"wb");
    if (!out_file){
        av_log(NULL,AV_EF_IGNORE_ERR,"can not open file");
        return;
    }

    AVFormatContext *avFormatContext= NULL;
    int audio_index;

    AVPacket *avPacket = av_packet_alloc();//和av_init_packet()的区别

    int ret;

    //打开
    ret = avformat_open_input(&avFormatContext,input,NULL,NULL);
    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"avformat open input error:%s",av_err2str(ret));
        goto failed;
    }

    //获取音频流的index
    audio_index = av_find_best_stream(avFormatContext,AVMEDIA_TYPE_AUDIO,-1,-1,NULL,0);

    if (audio_index<0){
        av_log(NULL,AV_LOG_ERROR,"av find audio stream failed:%s",av_err2str(ret));
        goto failed;
    }

    //获取流信息到context
    ret = avformat_find_stream_info(avFormatContext,NULL);

    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"avformat find stream info error:%s",av_err2str(ret));
        goto failed;
    }

    //解包数据
    while((ret = av_read_frame(avFormatContext,avPacket)) == 0){
        //如果是音频流
        if (avPacket->stream_index == audio_index){

            //写adts头
            char adts_header_buf[7];
            adts_header(adts_header_buf, avPacket->size);
            fwrite(adts_header_buf, 1, 7, out_file);

            //写裸流
            fwrite(avPacket->data,1,avPacket->size,out_file);
        }
        //减引用计数
        av_packet_unref(avPacket);
    }

    failed:
        avformat_close_input(&avFormatContext);
        fclose(out_file);

}




int main(){
    const char* input_file = "../videos/nianhui.mp4";
    const char* output_file = "../videos/nianhui.aac";

    extractAAC(input_file,output_file);
}