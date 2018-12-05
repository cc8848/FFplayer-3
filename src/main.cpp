#include "ffplayer.h"
#include <QApplication>
#include <stdio.h>
#include <QDir>

#include <SDL.h>
#undef main



#define FUNC_ERROR(err_code) do {\
    char buf[1024];\
    av_strerror(err_code, buf, 1024);\
    av_log(NULL, AV_LOG_ERROR, "DEBUG_ERROR: %s-%04d-->%s.\n",__FUNCTION__, __LINE__, buf);\
    } while(0)

void adts_header(char *szAdtsHeader, int dataLen);
int leishen3(void);
int video_decode(char *filepath);
int fetch_audio(char *srcPath, char *dstPath);
int sdl2_test(char *filepath);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FFPlayer w;

    //video_decode("../testData/Titanic.ts");
    //sdl2_test("../testData/Hello_World.bmp");
    //fetch_audio("../testData/Forrest_Gump_IMAX.mp4", "../testData/Forrest_Gump_IMAX.aac");

    return 0;    //a.exec()
}


int fetch_audio(char *srcPath, char *dstPath)
{
    //5. fetch audio data, the .ts media file is invalue here.
    AVFormatContext *pFormatCtx = NULL;
    AVPacket *packet = NULL;

    FILE *pDstFile = NULL;
    int ret = -1;
    int audioIndex = -1;

    av_register_all();
    //5.1   open media file and dest file
    ret = avformat_open_input(&pFormatCtx, srcPath, NULL, NULL);
    if(ret < 0)
    {
        FUNC_ERROR(ret);
        return -1;
    }

    pDstFile = fopen(dstPath, "wb");
    if(!pDstFile)
    {
        perror("fopen failed: ");
        goto FAIL_FOPEN;
    }
    //5.2   fetch audio stream
    // avformat_find_stream_info(pFormatCtx, NULL);
    ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if(ret < 0)
    {
        FUNC_ERROR(ret);
        goto FAIL_AFBS;
    }
    audioIndex = ret;
    av_dump_format(pFormatCtx, audioIndex, "av_dump_format", 0);
    packet = av_packet_alloc();
    if(!packet)
    {
        qDebug() <<"av_packet_alloc() failed";
        goto FAIL_APA;
    }
    //5.3   write audio data to aac file.
    while(av_read_frame(pFormatCtx, packet) >= 0)
    {
        if(packet->stream_index == audioIndex)
        {
            char adts_header_buf[7];
            adts_header(adts_header_buf, packet->size);
            fwrite(adts_header_buf, 1, 7, pDstFile);

            ret = fwrite(packet->data, 1, packet->size, pDstFile);
            if(ret != packet->size)
            {
                qDebug() <<"warning, ret != packet->size";
            }
        }
        av_packet_unref(packet);
    }
    //5.4   exit.
    av_packet_free(&packet);
FAIL_APA:
FAIL_AFBS:
    fclose(pDstFile);
FAIL_FOPEN:
    avformat_close_input(&pFormatCtx);

    return 0;
}


void adts_header(char *szAdtsHeader, int dataLen)
{

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

int video_decode(char *filepath)
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
        videoindex = -1;
        for(i=0; i<pFormatCtx->nb_streams; i++)
            if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
                videoindex = i;
                break;
            }
        if(videoindex == -1){
            printf("Didn't find a video stream.\n");
            return -1;
        }
        pCodecCtx = pFormatCtx->streams[videoindex]->codec;
        pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
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
    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
    // use av_image_fill_arrays() instead.
    avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    //Output Info-----------------------------
    printf("--------------- File Information ----------------\n");
    av_dump_format(pFormatCtx,0,filepath,0);
    printf("-------------------------------------------------\n");
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

    frame_cnt=0;
    while(av_read_frame(pFormatCtx, packet)>=0){
        if(packet->stream_index == videoindex){
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


int sdl2_test(char *filepath)
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
    gHelloWorld = SDL_LoadBMP(filepath);//加载图片
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








