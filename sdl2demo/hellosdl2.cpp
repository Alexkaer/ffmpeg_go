//
// Created by 许双 on 2020/1/10.
//


extern "C"{
#include <SDL2/SDL.h>
}

#include <iostream>

int main(){

    int ret;
    SDL_Window *sdlWindow = NULL;
    SDL_Renderer *sdlRenderer = NULL;
    SDL_Surface *sdlSurface=NULL;
    SDL_Texture *sdlTexture=NULL;

    //初始化
    ret = SDL_Init(SDL_INIT_EVERYTHING);
    if (ret<0){
        std::cout << SDL_GetError() << std::endl;
    }
    //  创建窗口
    sdlWindow = SDL_CreateWindow("hellosdl",200,200,640,480,SDL_WINDOW_SHOWN);

    if (!sdlWindow){
        std::cout<<SDL_GetError()<<"\n";
    }
    //创建render
    sdlRenderer = SDL_CreateRenderer(sdlWindow,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!sdlRenderer){
        std::cout<<SDL_GetError()<<"\n";
    }
    //创建surface
    sdlSurface = SDL_LoadBMP("/Users/xushuang/cspace/ffmpeg_go/videos/hello.bmp");

    if (!sdlSurface){
        std::cout<<SDL_GetError()<<"\n";
    }
    //通过surface创建texture
    sdlTexture = SDL_CreateTextureFromSurface(sdlRenderer,sdlSurface);
    //释放sdl surface
    SDL_FreeSurface(sdlSurface);

    //清空屏幕
    SDL_RenderClear(sdlRenderer);
    //绘制texture
    SDL_RenderCopy(sdlRenderer,sdlTexture,NULL,NULL);
    //更新屏幕
    SDL_RenderPresent(sdlRenderer);

    //循环事件
    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = true;
            }
        }
    }

    //释放资源
    SDL_DestroyTexture(sdlTexture);
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
    return 0;

}