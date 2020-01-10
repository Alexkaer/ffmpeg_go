//
// Created by 许双 on 2020/1/7.
//

#include "transfmt.h"


/**
 * 首先要明确转换格式的流程
 *
 * 1。输入文件
 * 2。解复用
 * 3。解码
 * 4。编码
 * 5。复用
 * 6。输出文件
 */

int transMp4ToFlv(const char * input_file){

    const AVBitStreamFilter *avBitStreamFilter=NULL;

    //定义输入输出AVFormatContext
    AVFormatContext *infmtc=NULL, *outfmtc=NULL;
    AVOutputFormat *outputFormat;

    AVPacket *avPacket=av_packet_alloc();

    const char *output_file;

    int ret;

    int frame_index=0;

    output_file = "../videos/test.flv";

    //初始化infmtc
    infmtc = avformat_alloc_context();
    //打开输入文件
    ret = avformat_open_input(&infmtc,input_file,NULL,NULL);
    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"open file failed");
        return 0;
    }
    //获取视频流信息，为AVFormatContext->streams填充正确的数据
    ret = avformat_find_stream_info(infmtc,NULL);
    if(ret<0){
        av_log(NULL,AV_LOG_ERROR,"get stream info failed");
        return 0;
    }


    //FFMPEG转码器在封装成FLV或者MP4的时候，会用到h264_mp4toannexb、aac_adtstoasc这两个filter。老版本的bsf使用需要特别注意，一个不小心，就造成的内存泄漏，不容易察觉。
    //MP4 中使用的是 H.264 编码，而 H. 264 编码有两种封装模式
    //一种是 annexb 模式，它是传统模式，有 startcode, SPS 和 PPS 在 Element Stream
    //中;另一种是 mp4 模式， 一般 MP4、 MKV、 AVI 都没有 startcode, SPS 和 PPS 以及其他信息被封装在容器中
    //每一帧前面是这一l帧的长度值，很多解码器只支持 annexb 模式，因此需要对 MP4 模式做
    //转换在 FFmpeg 中用 h264 mp4toannexb filter 可以进行模式转换;使用命令
    //II- bsf h264 mp4toannexb 就可实现转换

    //通过名字获取BitStreamFilter
    avBitStreamFilter = av_bsf_get_by_name("h264_mp4toannexb");
    if(!avBitStreamFilter)
    {
        av_log(NULL,AV_LOG_ERROR,"Unkonw bitstream filter");
    }
    //初始化AVBSFContext
    AVBSFContext *avbsfContext;
    ret = av_bsf_alloc(avBitStreamFilter,&avbsfContext);
    if(ret<0){
        av_log(NULL,AV_LOG_ERROR,"av_bsf_alloc failed");
        return 0;
    }
    //打印infmtc->streams里面的信息，用于调试
    av_dump_format(infmtc,0,input_file,0);

    //初始化输出视频码流的AVFormatContext
    ret = avformat_alloc_output_context2(&outfmtc,NULL,NULL,output_file);
    if(ret<0){
        av_log(NULL,AV_LOG_ERROR,"init output AVFormatContext failed");
        return 0;
    }
    outputFormat = outfmtc->oformat;


    //遍历流信息（音频流，视频流，字母流）
    for (int j = 0; j < infmtc->nb_streams; ++j) {
        //获取输入流
        AVStream *in_stream = infmtc->streams[j];

        //获取输入流的codec parameters
        AVCodecParameters *avCodecParameters = in_stream->codecpar;

        //通过codec parameters 里面的codec id获取AVCodec
        AVCodec *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);

        //通过avcodec初始化AVCodecContext
        AVCodecContext *avCodecContext = avcodec_alloc_context3(avCodec);

        //创建新的输出流
        AVStream *out_stream = avformat_new_stream(outfmtc,avCodec);
        if(!out_stream){
            av_log(NULL,AV_LOG_ERROR,"failed alloc output stream");
        }
        //填充数据到AVCodecContext
        ret = avcodec_parameters_to_context(avCodecContext,avCodecParameters);
        if (ret<0){
            av_log(NULL,AV_LOG_ERROR,"failed to init AVCodecContext");
        }
        //将输入流数据拷贝到输出流
        ret =avcodec_parameters_copy(out_stream->codecpar,in_stream->codecpar);
        if (ret<0){
            av_log(NULL,AV_LOG_ERROR,"failed to copy codec parameters ");
        }
        out_stream->codecpar->codec_tag=0;
        out_stream->time_base = in_stream->time_base;


    }
    //输出output format context 信息
    av_dump_format(outfmtc,0,output_file,1);

    if (!(outputFormat->flags & AVFMT_NOFILE)) {    // TODO: 研究AVFMT_NOFILE标志
        // 2.4 创建并初始化一个AVIOContext，用以访问URL(out_filename)指定的资源
        ret = avio_open(&outfmtc->pb, output_file, AVIO_FLAG_WRITE);
        if (ret < 0) {
            av_log(NULL,AV_LOG_ERROR,"Could not open output file '%s",output_file);
        }
    }


    //写文件头
    ret = avformat_write_header(outfmtc,NULL);
    if (ret<0){
        av_log(NULL,AV_LOG_ERROR,"Error occurred when opening output file\n");
    }

    av_bsf_init(avbsfContext);

    int video_index = av_find_best_stream(infmtc,AVMEDIA_TYPE_VIDEO,-1,-1,NULL,0);

    AVStream *in_stream, *out_stream;
    while((ret = av_read_frame(infmtc,avPacket))==0){
        //获取一个avpacket
        if (ret<0){
            break;
        }
        in_stream =infmtc->streams[avPacket->stream_index];

        out_stream = outfmtc->streams[avPacket->stream_index];

        /* copy packet */
        // 3.3 更新packet中的pts和dts
        // 关于AVStream.time_base的说明：
        // 输入：输入流中含有time_base，在avformat_find_stream_info()中可取到每个流中的time_base
        // 输出：avformat_write_header()会根据输出的封装格式确定每个流的time_base并写入文件中
        // AVPacket.pts和AVPacket.dts的单位是AVStream.time_base，不同的封装格式其AVStream.time_base不同
        // 所以输出文件中，每个packet需要根据输出封装格式重新计算pts和dts
        av_packet_rescale_ts(avPacket, in_stream->time_base, out_stream->time_base);
        avPacket->pos = -1;


        //mp4转annexb模式

        if (avPacket->stream_index == video_index){//第一个流，为视频流？
            //把pkt数据推送到filter中去
            ret = av_bsf_send_packet(avbsfContext,avPacket);
            if (ret<0){
                av_log(NULL,AV_LOG_ERROR,"send packet failed");
            }
            //获取处理后的数据，需要是同一个packet
            ret = av_bsf_receive_packet(avbsfContext,avPacket);
            if (ret<0){
                av_log(NULL,AV_LOG_ERROR,"receive packet failed");
            }
            av_bsf_free(&avbsfContext);
        }

        // 3.4 将packet写入输出
        ret = av_interleaved_write_frame(outfmtc, avPacket);
        if (ret < 0) {
            printf("Error muxing packet\n");
            break;
        }
        av_log(NULL,AV_LOG_ERROR,"write %8d frames to output file",frame_index);
        av_packet_unref(avPacket);
        frame_index++;
    }

    //写输出文件尾
    av_write_trailer(outfmtc);

    //close input context
    avformat_close_input(&infmtc);

    /* close output */
    if (outfmtc && !(outfmtc->flags & AVFMT_NOFILE)){
        avio_closep(&outfmtc->pb);
    }
    avformat_free_context(outfmtc);


    return 0;

}