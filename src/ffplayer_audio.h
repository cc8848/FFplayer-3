#ifndef FFPLAYER_AUDIO
#define FFPLAYER_AUDIO

#include "ffplayer.h"

#define FUNC_ERROR(err_code) do {\
    char buf[1024];\
    av_strerror(err_code, buf, 1024);\
    av_log(NULL, AV_LOG_ERROR, "DEBUG_ERROR: %s-%04d-->%s.\n",__FUNCTION__, __LINE__, buf);\
    } while(0)













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










int fetch_audio(char *srcPath, char *dstPath)
{
    //5. fetch audio data, *.ts media file is invalue here.
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














#endif // FFPLAYER_AUDIO

