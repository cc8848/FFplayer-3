#include "ffplayer.h"
#include <QApplication>
#include <QDir>

#include <SDL.h>
#undef main


#define FUNC_ERROR(err_code) {\
    char buf[1024];\
    av_strerror(err_code, buf, 1024);\
    av_log(NULL, AV_LOG_ERROR, "DEBUG_ERROR: %s-%04d-->%s.\n",__FUNCTION__, __LINE__, buf);\
    return AV_LOG_ERROR;\
    }

int leishen3(void);
int sdl2_test(void);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FFPlayer w;

    // rename url
    int err_code = -1;
    err_code = avpriv_io_move("../testData/Titfesanic.ts", "../testData/Romafgewfentic.ts");
    if(err_code < 0)
        FUNC_ERROR(err_code)


    // delete url
    err_code = avpriv_io_delete("../testData/111.txt");
    if(err_code < 0)
        FUNC_ERROR(err_code)



    return 0;    //a.exec()
}











int leishen3(void)
{
    AVFormatContext	*pFormatCtx = NULL;
    int				i, videoindex;
    AVCodecContext	*pCodecCtx;
    AVCodec			*pCodec;
    AVFrame	*pFrame,*pFrameYUV;
    uint8_t *out_buffer;
    AVPacket *packet;
    int ret, got_picture;
    struct SwsContext *img_convert_ctx;
    //输入文件路径
    char filepath[]="../testData/Titanic.ts";

    int frame_cnt;

    av_register_all();  //Initialize libavformat and register all the muxers, demuxers and protocols.
    do
    {
        ret = avformat_open_input(&pFormatCtx,filepath,NULL,NULL);
        if(ret != 0)
            break;
        ret = avformat_find_stream_info(pFormatCtx,NULL);
        if(ret < 0)
            break;
        videoindex=-1;
        for(i=0; i<pFormatCtx->nb_streams; i++)
            if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
                videoindex=i;
                break;
            }
        if(videoindex==-1){
            printf("Didn't find a video stream.\n");
            return -1;
        }
        pCodecCtx=pFormatCtx->streams[videoindex]->codec;
        pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
        if(pCodec == NULL)
            break;
        ret = avcodec_open2(pCodecCtx, pCodec,NULL);
        if(ret < 0)
            break;
    }while(0);
    if((ret != 0) || (pCodec == NULL))
    {
        char buf[1024];
        av_strerror(ret, buf, 1024);
        av_log(NULL, AV_LOG_ERROR, "%s-%4d: %d, %s\n", __FUNCTION__, __LINE__,
               ret, buf);
    }

    /*
     * 在此处添加输出视频信息的代码
     * 取自于pFormatCtx，使用fprintf()
     */
    pFrame=av_frame_alloc();
    pFrameYUV=av_frame_alloc();
    out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
    packet=(AVPacket *)av_malloc(sizeof(AVPacket));
    //Output Info-----------------------------
    printf("--------------- File Information ----------------\n");
    av_dump_format(pFormatCtx,0,filepath,0);
    printf("-------------------------------------------------\n");
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    frame_cnt=0;
    while(av_read_frame(pFormatCtx, packet)>=0){
        if(packet->stream_index==videoindex){
                /*
                 * 在此处添加输出H264码流的代码
                 * 取自于packet，使用fwrite()
                 */
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
            if(ret < 0){
                printf("Decode Error.\n");
                return -1;
            }
            if(got_picture){
                sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                    pFrameYUV->data, pFrameYUV->linesize);
                printf("Decoded frame index: %d\n",frame_cnt);

                /*
                 * 在此处添加输出YUV的代码
                 * 取自于pFrameYUV，使用fwrite()
                 */

                frame_cnt++;

            }
        }
        av_free_packet(packet);
    }

    sws_freeContext(img_convert_ctx);

    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    return ret;
}


int sdl2_test(void)
{
    using namespace std;
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 480;

    //The window we'll be rendering to
    SDL_Window* gWindow = NULL;

    //The surface contained by the window
    SDL_Surface* gScreenSurface = NULL;

    //The image we will load and show on the screen
    SDL_Surface* gHelloWorld = NULL;

    //首先初始化   初始化SD视频子系统
    if(SDL_Init(SDL_INIT_VIDEO)<0)
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    //创建窗口
    gWindow=SDL_CreateWindow("SHOW BMP",//窗口标题
                            SDL_WINDOWPOS_UNDEFINED,//窗口位置设置
                            SDL_WINDOWPOS_UNDEFINED,
                            SCREEN_WIDTH,//窗口的宽度
                            SCREEN_HEIGHT,//窗口的高度
                            SDL_WINDOW_SHOWN//显示窗口
                            );
    if(gWindow==NULL)
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        return false;
    }
    //Use this function to get the SDL surface associated with the window.
    //获取窗口对应的surface
    gScreenSurface=SDL_GetWindowSurface(gWindow);

    //加载图片
    gHelloWorld = SDL_LoadBMP("../testData/Hello_World.bmp");//加载图片
    if( gHelloWorld == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", "Hello_World.bmp", SDL_GetError() );
        return false;
    }
    //Use this function to perform a fast surface copy to a destination surface.
    //surface的快速复制
    //下面函数的参数分别为： SDL_Surface* src ,const SDL_Rect* srcrect , SDL_Surface* dst ,  SDL_Rect* dstrect
    SDL_BlitSurface( gHelloWorld ,     NULL ,                     gScreenSurface ,          NULL);
    SDL_UpdateWindowSurface(gWindow);//更新显示copy the window surface to the screen
    SDL_Delay(8000);//延时2000毫秒

    //释放内存
    SDL_FreeSurface( gHelloWorld );//释放空间
    gHelloWorld = NULL;

    SDL_DestroyWindow(gWindow);//销毁窗口
    gWindow = NULL ;

    SDL_Quit();//退出SDL
}








