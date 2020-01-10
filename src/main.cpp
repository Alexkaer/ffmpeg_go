//#include <iostream>
//#include "demuxerdemo.h"
//#include "transfmt.h"

//extern "C"{
//#include <libavformat/avformat.h>
//
//}
#include "sdlplayvideo.h"

int main() {

    //设置日志过滤器
//    av_log_set_level(AV_LOG_DEBUG);

    //打印配置信息
//    std::string configuration = avcodec_configuration();
//    std::cout<<configuration<<"\n";


    //打印版本信息
//    int version = avcodec_version();
//    std::cout<<"version="<<version<<"\n";
//    av_log(NULL,AV_LOG_ERROR,"version = %d\n",version);


//    const char *input_file = "../videos/nianhui.mp4";
//    const char *http_url = "http://clips.vorwaerts-gmbh.de/big_buck_bunny.mp4";

//    demuxerMp4ToH264(input_file);

//    transMp4ToFlv(input_file);

    const char *url = "../videos/nianhui.mp4";
    playVideo(url);

    //删除文件
//    avpriv_io_delete("../videos/nianhui.h264");

//    avpriv_io_move("../videos/bbb.flv","../src/aaa.mp4");

//    AVIODirContext *avioDirContext = NULL;
//    AVIODirEntry *avioDirEntry =NULL;
//    avio_open_dir(&avioDirContext,"../src/",NULL);
//    while (true){
//        int ret = avio_read_dir(avioDirContext,&avioDirEntry);
//        if (ret<0 || !avioDirEntry){
//            break;
//        }
//        av_log(NULL,AV_LOG_ERROR,"%d == %s \n",avioDirEntry->size,avioDirEntry->name);
//        avio_free_directory_entry(&avioDirEntry);
//    }
//    avio_close_dir(&avioDirContext);

    return 0;
}