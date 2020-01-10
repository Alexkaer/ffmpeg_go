//
// Created by 许双 on 2020/1/10.
//提取视频用sdl播放
//

extern "C"{
#include <libavformat/avformat.h>
#include <SDL2/SDL.h>
}


#define WINDOW_WIDTH 540
#define WINDOW_HEIGHT 960


/**
 * SDL 相关
 */
SDL_Window *sdlWindow = nullptr;
SDL_Renderer *sdlRenderer = nullptr;
SDL_Texture *sdlTexture = nullptr;
//SDL_Rect rect;


AVFormatContext *avFormatContext = nullptr;
AVCodecContext *avCodecContext = nullptr;
AVPacket *avPacket = nullptr;
AVFrame *avFrame = nullptr;
int video_index;


void initSDL(){

    SDL_Init(SDL_INIT_EVERYTHING);

    sdlWindow = SDL_CreateWindow("VideoPlayer",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH,
                                 WINDOW_HEIGHT, SDL_WINDOW_OPENGL);

    sdlRenderer = SDL_CreateRenderer(sdlWindow,-1, 0);

    sdlTexture = SDL_CreateTexture(sdlRenderer,SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

//    rect.x = 0;
//    rect.y = 0;
//    rect.w = WINDOW_WIDTH;
//    rect.h = WINDOW_HEIGHT;
}


void prepareDecoder(const char* fileUrl){


//    avFormatContext = avformat_alloc_context();

    avformat_open_input(&avFormatContext,fileUrl, nullptr, nullptr);

//    avformat_find_stream_info(avFormatContext, nullptr);

    video_index = av_find_best_stream(avFormatContext,AVMEDIA_TYPE_VIDEO,-1,-1, nullptr,0);

    AVCodecParameters *avCodecParameters = avFormatContext->streams[video_index]->codecpar;

    AVCodec *avCodec = avcodec_find_decoder(avCodecParameters->codec_id);

    avCodecContext = avcodec_alloc_context3(avCodec);

    avcodec_parameters_to_context(avCodecContext,avCodecParameters);

    avcodec_open2(avCodecContext,avCodec, nullptr);

    avPacket = av_packet_alloc();
    avFrame = av_frame_alloc();


}

void drawFrame(AVFrame *frame){
    SDL_UpdateYUVTexture(sdlTexture, nullptr,frame->data[0],frame->linesize[0],frame->data[1],frame->linesize[1],frame->data[2],frame->linesize[2]);

    SDL_RenderClear(sdlRenderer);
    SDL_RenderCopy(sdlRenderer,sdlTexture, nullptr, nullptr);
    SDL_RenderPresent(sdlRenderer);
    SDL_Delay(50);
}

void decodeFrame(){


    while (av_read_frame(avFormatContext,avPacket) >= 0){

        if (avPacket->stream_index == video_index){
            avcodec_send_packet(avCodecContext,avPacket);

            while(avcodec_receive_frame(avCodecContext,avFrame) >= 0){
                drawFrame(avFrame);
            }
        }
        av_packet_unref(avPacket);
    }
}

void freeContext(){
    if (avFormatContext != nullptr) avformat_close_input(&avFormatContext);
    if (avCodecContext != nullptr) avcodec_free_context(&avCodecContext);
    if (avPacket != nullptr) av_packet_free(&avPacket);
    if (avFrame != nullptr) av_frame_free(&avFrame);

}

void playVideo(const char* filePath){
    initSDL();
    prepareDecoder(filePath);
    decodeFrame();
    freeContext();
}


int main(){
    playVideo("../videos/nianhui.mp4");
}
