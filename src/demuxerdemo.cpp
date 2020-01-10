//
// Created by 许双 on 2020/1/8.
//

#include "demuxerdemo.h"


/**
 *
 * * 解复用的流程：
 * 1。初始化AVFormatContext
 * 2。打开输入流
 * 3。读取视频信息
 * 4。找到要读取流的index
 * 5。读取流信息，写入文件
 * 6。读取结束，释放资源
 *
 * @param file 文件路径或者网络地址
 * @return
 */
void demuxerMp4ToH264(const char *filePath){

    FILE *output = fopen("../videos/nianhui.h264", "wb+");

    //如果视频来源网络
    avformat_network_init();


    int ret;

    //创建mp4 to annexb的filter
    const AVBitStreamFilter *avBitStreamFilter = av_bsf_get_by_name("h264_mp4toannexb");
    AVBSFContext *avbsfContext;


    //初始化AVFormatContext
    AVFormatContext *avFormatContext = avformat_alloc_context();

    if (!avFormatContext){
        av_log(NULL,AV_LOG_ERROR,"avformat alloc error");
        goto failed;
    }

    //打开输入文件
    ret = avformat_open_input(&avFormatContext,filePath,NULL,NULL);

    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"avformat open file error:%s",av_err2str(AVERROR(ret)));
        goto failed;
    }

    //读取视频信息
    ret = avformat_find_stream_info(avFormatContext,NULL);

    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"avformat find stream info error:%s",av_err2str(AVERROR(ret)));
        goto failed;
    }

    //寻找指定视频流
    int index;
    index = av_find_best_stream(avFormatContext,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);

    if (index<0){
        av_log(NULL,AV_LOG_ERROR,"find stream index error:%s",av_err2str(AVERROR(ret)));
        goto failed;
    }


    //alloc bsf
    ret = av_bsf_alloc(avBitStreamFilter,&avbsfContext);
    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"av bsf alloc error:%s",av_err2str(AVERROR(ret)));
        goto failed;
    }

    //复制解码器参数到BSFContext
    avcodec_parameters_copy(avbsfContext->par_in,avFormatContext->streams[index]->codecpar);
    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"bsf copy parameters error:%s",av_err2str(AVERROR(ret)));
        goto failed;
    }
    //同步timebase
    avbsfContext->time_base_in = avFormatContext->streams[index]->time_base;

    //初始化bsf
    ret = av_bsf_init(avbsfContext);
    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"bsf init error:%s",av_err2str(AVERROR(ret)));
        goto failed;
    }


    //读取packet
    AVPacket *avPacket;
    //初始化avpacket
    avPacket = av_packet_alloc();

    while (av_read_frame(avFormatContext,avPacket) == 0){
        //判断是否是指定流的packet,不是直接continue
        if (avPacket->stream_index != index){
            continue;
        }

        //发送packet 到bsf filter
        ret = av_bsf_send_packet(avbsfContext,avPacket);
        if (ret <0){
            av_log(NULL,AV_LOG_ERROR,"bsf send packet error:%s",av_err2str(AVERROR(ret)));
            goto failed;
        }

        //接受添加sps pps头的packet
        while ((ret = av_bsf_receive_packet(avbsfContext, avPacket)) == 0) {
            //写入到文件
            fwrite(avPacket->data, 1, avPacket->size, output);
            av_packet_unref(avPacket);
        }

        //需要输入数据
        if (ret == AVERROR(EAGAIN)) {
            av_log(NULL,AV_LOG_ERROR,"[debug] BSF EAGAIN");
            av_packet_unref(avPacket);
            continue;
        }

        //已经读取到结尾
        if (ret == AVERROR_EOF) {
            av_log(NULL,AV_LOG_ERROR,"[debug] BSF EOF");
            break;
        }

        if (ret < 0) {
            av_log(NULL,AV_LOG_ERROR,"[error] BSF receive packet failed :%s",av_err2str(AVERROR(ret)));
            goto failed;
        }

    }

    //flush
    ret = av_bsf_send_packet(avbsfContext,NULL);
    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"[error] BSF flush send packet failed:%s",av_err2str(AVERROR(ret)));
        goto failed;
    }
    while ((ret = av_bsf_receive_packet(avbsfContext, avPacket)) == 0) {
        fwrite(avPacket->data, 1, avPacket->size, output);
    }

    if (ret != AVERROR_EOF) {
        av_log(NULL,AV_LOG_ERROR,"[debug] BSF flush EOF");
        goto failed;
    }


    failed:
    //释放packet
    av_packet_free(&avPacket);
    //关闭网络流
    avformat_network_deinit();
    //释放AVFormatContext
    avformat_close_input(&avFormatContext);
    //释放BSFContext
    av_bsf_free(&avbsfContext);
    //关闭文件流
    fclose(output);

}