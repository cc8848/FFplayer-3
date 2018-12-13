#ifndef FFPLAYER_AUDIO
#define FFPLAYER_AUDIO

#include "ffplayer_general.h"
#define ERROR_STR_SIZE 1024
#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
#define MAX_AUDIO_FRAME_SIZE 192000

#if 1       // Fetch audio data from media file.
// just for HE-AAC, not suit to LC-AAC
void adts_header(char *szAdtsHeader, int dataLen)
{
    int audio_object_type = 2;
    int sampling_frequency_index = 7;
    int channel_config = 2;

    int adtsLen = dataLen + 7;

    szAdtsHeader[0] = 0xff;         //syncword:0xfff                          high 8bits
    szAdtsHeader[1] = 0xf0;         //syncword:0xfff                          low 4bits
    szAdtsHeader[1] |= (0 << 3);    //ID, MPEG Version:0 for MPEG-4,1 for MPEG-2  1bit
    szAdtsHeader[1] |= (0 << 1);    //Layer: always:'00'                      2bits
    szAdtsHeader[1] |= 1;           //protection absent:1                     1bit
                                        //Warning,set 1 if there is no CRC, 0 if there is CRC

    szAdtsHeader[2] = (audio_object_type - 1)<<6;            //profile:audio_object_type - 1                      2bits
    szAdtsHeader[2] |= (sampling_frequency_index & 0x0f)<<2; //sampling frequency index:sampling_frequency_index  4bits
    szAdtsHeader[2] |= (0 << 1);                             //private bit:0                                      1bit
    szAdtsHeader[2] |= (channel_config & 0x04)>>2;           //channel configuration:channel_config               high 1bit

                    szAdtsHeader[3] = (channel_config & 0x03)<<6;     //channel configuration:channel_config      low 2bits
    szAdtsHeader[3] |= (0 << 5);                      //original：0                               1bit
    szAdtsHeader[3] |= (0 << 4);                      //home：0                                   1bit
    szAdtsHeader[3] |= (0 << 3);                      //copyright id bit：0                       1bit
    szAdtsHeader[3] |= (0 << 2);                      //copyright id start：0                     1bit
    szAdtsHeader[3] |= ((adtsLen & 0x1800) >> 11);           //frame length：value   high 2bits

    szAdtsHeader[4] = (uint8_t)((adtsLen & 0x7f8) >> 3);     //frame length:value    middle 8bits
    szAdtsHeader[5] = (uint8_t)((adtsLen & 0x7) << 5);       //frame length:value    low 3bits
    szAdtsHeader[5] |= 0x1f;                                 //buffer fullness:0x7ff high 5bits
    szAdtsHeader[6] = 0xfc;
}

int fetch_audio_LC(char *src_filename, char *dst_filename)
{
    int err_code;
    char errors[1024];

    FILE *dst_fd = NULL;

    int audio_stream_index = -1;


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
#endif




#if 1       // encode_audio
/* check that a given sample format is supported by the encoder */
static int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

/* just pick the highest supported samplerate */
static int select_sample_rate(const AVCodec *codec)
{
    const int *p;
    int best_samplerate = 0;

    if (!codec->supported_samplerates)
        return 44100;

    p = codec->supported_samplerates;
    while (*p) {
        if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
            best_samplerate = *p;
        p++;
    }
    return best_samplerate;
}

/* select layout with the highest channel count */
static int select_channel_layout(const AVCodec *codec)
{
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels   = 0;

    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;

    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);

        if (nb_channels > best_nb_channels) {
            best_ch_layout    = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}

int encode_audio(const char *dstPath)
{
    const AVCodec *codec;
    AVCodecContext *c= NULL;
    AVFrame *frame;
    AVPacket pkt;
    int i, j, k, ret, got_output;
    FILE *f;
    uint16_t *samples;
    float t, tincr;

    /* register all the codecs */
    avcodec_register_all();

    /* find the MP2 encoder */
    codec = avcodec_find_encoder(AV_CODEC_ID_MP2);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    /* put sample parameters */
    c->bit_rate = 64000;

    /* check that the encoder supports s16 pcm input */
    c->sample_fmt = AV_SAMPLE_FMT_S16;
    if (!check_sample_fmt(codec, c->sample_fmt)) {
        fprintf(stderr, "Encoder does not support sample format %s",
                av_get_sample_fmt_name(c->sample_fmt));
        exit(1);
    }

    /* select other audio parameters supported by the encoder */
    c->sample_rate    = select_sample_rate(codec);
    c->channel_layout = select_channel_layout(codec);
    c->channels       = av_get_channel_layout_nb_channels(c->channel_layout);

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    f = fopen(dstPath, "wb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", dstPath);
        exit(1);
    }

    /* frame containing input raw audio */
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate audio frame\n");
        exit(1);
    }

    frame->nb_samples     = c->frame_size;
    frame->format         = c->sample_fmt;
    frame->channel_layout = c->channel_layout;

    /* allocate the data buffers */
    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate audio data buffers\n");
        exit(1);
    }

    /* encode a single tone sound */
    t = 0;
    tincr = 2 * M_PI * 440.0 / c->sample_rate;
    for (i = 0; i < 200; i++) {
        av_init_packet(&pkt);
        pkt.data = NULL; // packet data will be allocated by the encoder
        pkt.size = 0;

        /* make sure the frame is writable -- makes a copy if the encoder
         * kept a reference internally */
        ret = av_frame_make_writable(frame);
        if (ret < 0)
            exit(1);
        samples = (uint16_t*)frame->data[0];

        for (j = 0; j < c->frame_size; j++) {
            samples[2*j] = (int)(sin(t) * 10000);

            for (k = 1; k < c->channels; k++)
                samples[2*j + k] = samples[2*j];
            t += tincr;
        }
        /* encode the samples */
        ret = avcodec_encode_audio2(c, &pkt, frame, &got_output);
        if (ret < 0) {
            fprintf(stderr, "Error encoding audio frame\n");
            exit(1);
        }
        if (got_output) {
            fwrite(pkt.data, 1, pkt.size, f);
            av_packet_unref(&pkt);
        }
    }

    /* get the delayed frames */
    for (got_output = 1; got_output; i++) {
        ret = avcodec_encode_audio2(c, &pkt, NULL, &got_output);
        if (ret < 0) {
            fprintf(stderr, "Error encoding frame\n");
            exit(1);
        }

        if (got_output) {
            fwrite(pkt.data, 1, pkt.size, f);
            av_packet_unref(&pkt);
        }
    }
    fclose(f);

    av_frame_free(&frame);
    avcodec_free_context(&c);

    return 0;
}
#endif



#if 1       //decode_audio
int decode_audio(char *srcpath, char *dstPath)
{
    int i, ret;

    int err_code;
    char errors[1024];

    int audiostream_index = -1;

    AVFormatContext *pFormatCtx = NULL;

    const AVCodec *codec;
    AVCodecContext *c= NULL;

    int len;
    FILE *f, *outfile;
    uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];

    AVPacket avpkt;
    AVFrame *decoded_frame = NULL;


    /* register all the codecs */
    av_register_all();

    av_init_packet(&avpkt);

    /* open input file, and allocate format context */
    if ((err_code=avformat_open_input(&pFormatCtx, srcpath, NULL, NULL)) < 0) {
        av_strerror(err_code, errors, 1024);
        fprintf(stderr, "Could not open source file %s, %d(%s)\n", srcpath, err_code, errors);
        return -1;
    }

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        return -1; // Couldn't find stream information


    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, srcpath, 0);

    for(i=0; i<pFormatCtx->nb_streams; i++) {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO) {
            audiostream_index=i;
        }
    }

    /* find the MPEG audio decoder */
    //codec = avcodec_find_decoder_by_name("libfdk_aac");
    //codec = avcodec_find_decoder(pFormatCtx->streams[audiostream_index]->codec->codec_id/*AV_CODEC_ID_MP2*/);
    /*
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }
    */

    c = avcodec_alloc_context3(NULL);
    if (!c) {
        fprintf(stderr, "Could not allocate audio codec context\n");
        exit(1);
    }

    ret = avcodec_parameters_to_context(c, pFormatCtx->streams[audiostream_index]->codecpar);
    if (ret < 0) {
        return -1;
    }

    codec = avcodec_find_decoder(c->codec_id);
    if (!codec) {
        fprintf(stderr, "Codec not found\n");
        exit(1);
    }

    //Out Audio Param
    uint64_t out_channel_layout=AV_CH_LAYOUT_STEREO;

    //AAC:1024  MP3:1152
    int out_nb_samples= c->frame_size;
    //AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;

    int out_sample_rate=44100;
    int out_channels=av_get_channel_layout_nb_channels(out_channel_layout);
    //Out Buffer Size
    int out_buffer_size=av_samples_get_buffer_size(NULL,
                                                   out_channels,
                                                   out_nb_samples,
                                                   AV_SAMPLE_FMT_S16,
                                                   1);

    uint8_t *out_buffer=(uint8_t *)av_malloc(MAX_AUDIO_FRAME_SIZE*2);
    int64_t in_channel_layout=av_get_default_channel_layout(c->channels);

    struct SwrContext *audio_convert_ctx;
    audio_convert_ctx = swr_alloc();
    audio_convert_ctx = swr_alloc_set_opts(audio_convert_ctx,
                                         out_channel_layout,
                                         AV_SAMPLE_FMT_S16,
                                         out_sample_rate,
                                         in_channel_layout,
                                         c->sample_fmt,
                                         c->sample_rate,
                                         0,
                                         NULL);
    swr_init(audio_convert_ctx);

    /* open it */
    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "Could not open codec\n");
        exit(1);
    }

    /*
    f = fopen(srcpath, "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", srcpath);
        exit(1);
    }
    */

    outfile = fopen(dstPath, "wb");
    if (!outfile) {
        av_free(c);
        exit(1);
    }

    /* decode until eof */
    /*
    avpkt.data = inbuf;
    avpkt.size = fread(inbuf, 1, AUDIO_INBUF_SIZE, f);
    */

    while (1) {
        int got_frame = 0;

        if (!decoded_frame) {
            if (!(decoded_frame = av_frame_alloc())) {
                fprintf(stderr, "Could not allocate audio frame\n");
                exit(1);
            }
        }

        if(av_read_frame(pFormatCtx, &avpkt) < 0) {
            if(pFormatCtx->pb->error == 0) {
                usleep(100); /* no error; wait for user input */
                continue;
            } else {
                break;
            }
        }

        if(avpkt.stream_index != audiostream_index){
            av_free_packet(&avpkt);
            continue;
        }

        len = avcodec_decode_audio4(c, decoded_frame, &got_frame, &avpkt);
        if (len < 0) {
            av_strerror(len, errors, 1024);
            fprintf(stderr, "Error while decoding, err_code:%d, err:%s\n", len, errors);
            exit(1);
        }
        if (got_frame) {
            /* if a frame has been decoded, output it */
            int data_size = av_get_bytes_per_sample(c->sample_fmt);
            if (data_size < 0) {
                /* This should not occur, checking just for paranoia */
                fprintf(stderr, "Failed to calculate data size\n");
                exit(1);
            }
            swr_convert(audio_convert_ctx,
                        &out_buffer,
                        MAX_AUDIO_FRAME_SIZE,
                        (const uint8_t **)decoded_frame->data,
                        decoded_frame->nb_samples);

            fwrite(out_buffer, 1, out_buffer_size, outfile);

            /*
            for (i=0; i<decoded_frame->nb_samples; i++)
                for (ch=0; ch<c->channels; ch++)
                    fwrite(decoded_frame->data[ch] + data_size*i, 1, data_size, outfile);
            */
        }
        avpkt.size -= len;
        avpkt.data += len;
        avpkt.dts =
        avpkt.pts = AV_NOPTS_VALUE;

        //if (avpkt.size < AUDIO_REFILL_THRESH) {
            /* Refill the input buffer, to avoid trying to decode
             * incomplete frames. Instead of this, one could also use
             * a parser, or use a proper container format through
             * libavformat. */
        /*
            memmove(inbuf, avpkt.data, avpkt.size);
            avpkt.data = inbuf;
            len = fread(avpkt.data + avpkt.size, 1,
                        AUDIO_INBUF_SIZE - avpkt.size, f);
            if (len > 0)
                avpkt.size += len;
        }
        */
    }

    fclose(outfile);
    //fclose(f);

    avcodec_free_context(&c);
    av_frame_free(&decoded_frame);

    return 0;
}
#endif



#endif // FFPLAYER_AUDIO

