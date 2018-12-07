#ifndef FFPLAYER_AUDIO
#define FFPLAYER_AUDIO

#include "ffplayer_general.h"
#define ERROR_STR_SIZE 1024








// just for HE-AAC, not suit to LC-AAC
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







int fetch_audio_LC(char *src_filename, char *dst_filename)
{
    int err_code;
    char errors[1024];

    FILE *dst_fd = NULL;

    int audio_stream_index = -1;
    int len;

    AVFormatContext *ofmt_ctx = NULL;
    AVOutputFormat *output_fmt = NULL;

    AVStream *in_stream = NULL;
    AVStream *out_stream = NULL;

    AVFormatContext *fmt_ctx = NULL;
    //AVFrame *frame = NULL;
    AVPacket pkt;

    av_log_set_level(AV_LOG_DEBUG);

    if(src_filename == NULL || dst_filename == NULL){
        av_log(NULL, AV_LOG_DEBUG, "src or dts file is null, plz check them!\n");
        return -1;
    }

    /*register all formats and codec*/
    av_register_all();

    /*open input media file, and allocate format context*/
    if((err_code = avformat_open_input(&fmt_ctx, src_filename, NULL, NULL)) < 0){
        av_strerror(err_code, errors, 1024);
        av_log(NULL, AV_LOG_DEBUG, "Could not open source file: %s, %d(%s)\n",
               src_filename,
               err_code,
               errors);
        return -1;
    }

    /*retrieve audio stream*/
    if((err_code = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        av_strerror(err_code, errors, 1024);
        av_log(NULL, AV_LOG_DEBUG, "failed to find stream information: %s, %d(%s)\n",
               src_filename,
               err_code,
               errors);
        return -1;
    }

    /*dump input information*/
    av_dump_format(fmt_ctx, 0, src_filename, 0);

    in_stream = fmt_ctx->streams[1];
    AVCodecParameters *in_codecpar = in_stream->codecpar;
    if(in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO){
        av_log(NULL, AV_LOG_ERROR, "The Codec type is invalid!\n");
        exit(1);
    }

    //out file
    ofmt_ctx = avformat_alloc_context();
    output_fmt = av_guess_format(NULL, dst_filename, NULL);
    if(!output_fmt){
        av_log(NULL, AV_LOG_DEBUG, "Cloud not guess file format \n");
        exit(1);
    }

    ofmt_ctx->oformat = output_fmt;

    out_stream = avformat_new_stream(ofmt_ctx, NULL);
    if(!out_stream){
        av_log(NULL, AV_LOG_DEBUG, "Failed to create out stream!\n");
        exit(1);
    }

    if(fmt_ctx->nb_streams<2){
        av_log(NULL, AV_LOG_ERROR, "the number of stream is too less!\n");
        exit(1);
    }


    if((err_code = avcodec_parameters_copy(out_stream->codecpar, in_codecpar)) < 0 ){
        av_strerror(err_code, errors, ERROR_STR_SIZE);
        av_log(NULL, AV_LOG_ERROR,
               "Failed to copy codec parameter, %d(%s)\n",
               err_code, errors);
    }

    out_stream->codecpar->codec_tag = 0;

    if((err_code = avio_open(&ofmt_ctx->pb, dst_filename, AVIO_FLAG_WRITE)) < 0) {
        av_strerror(err_code, errors, 1024);
        av_log(NULL, AV_LOG_DEBUG, "Could not open file %s, %d(%s)\n",
               dst_filename,
               err_code,
               errors);
        exit(1);
    }

    /*
    dst_fd = fopen(dst_filename, "wb");
    if (!dst_fd) {
        av_log(NULL, AV_LOG_DEBUG, "Could not open destination file %s\n", dst_filename);
        return -1;
    }
    */


    /*dump output information*/
    av_dump_format(ofmt_ctx, 0, dst_filename, 1);

    /*
    frame = av_frame_alloc();
    if(!frame){
        av_log(NULL, AV_LOG_DEBUG, "Could not allocate frame\n");
        return AVERROR(ENOMEM);
    }
    */

    /*initialize packet*/
    av_init_packet(&pkt);
    pkt.data = NULL;
    pkt.size = 0;

    /*find best audio stream*/
    audio_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    if(audio_stream_index < 0){
        av_log(NULL, AV_LOG_DEBUG, "Could not find %s stream in input file %s\n",
               av_get_media_type_string(AVMEDIA_TYPE_AUDIO),
               src_filename);
        return AVERROR(EINVAL);
    }

    if (avformat_write_header(ofmt_ctx, NULL) < 0) {
        av_log(NULL, AV_LOG_DEBUG, "Error occurred when opening output file");
        exit(1);
    }

    /*read frames from media file*/
    while(av_read_frame(fmt_ctx, &pkt) >=0 ){
        if(pkt.stream_index == audio_stream_index){
            pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (enum AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
            pkt.dts = pkt.pts;
            pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
            pkt.pos = -1;
            pkt.stream_index = 0;
            av_interleaved_write_frame(ofmt_ctx, &pkt);
            av_packet_unref(&pkt);
        }
    }

    av_write_trailer(ofmt_ctx);

    /*close input media file*/
    avformat_close_input(&fmt_ctx);
    if(dst_fd) {
        fclose(dst_fd);
    }

    avio_close(ofmt_ctx->pb);

    return 0;
}



int fetch_audio_HE(char *srcPath, char *dstPath)
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

