#include "shared/VideoPlayer/VideoPlayer.hpp"
extern "C" {
    #include "ffmpeg/libavformat/avformat.h"
    #include "ffmpeg/libavcodec/avcodec.h"
    #include "ffmpeg/libavcodec/avcodec.h"
    #include "ffmpeg/libswscale/swscale.h"
}
#include "UnityEngine/Transform.hpp"

DEFINE_TYPE(Cinema, VideoPlayer);

void Cinema::VideoPlayer::Play()
{
    playing = true;
}

static void save_gray_frame(unsigned char *buf, int wrap, int xsize, int ysize, char *filename)
{
    FILE *f;
    int i;
    f = fopen(filename,"w");
    // writing the minimal required header for a pgm file format
    // portable graymap format -> https://en.wikipedia.org/wiki/Netpbm_format#PGM_example
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

    // writing line by line
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

static int decode_packet(AVPacket * pPacket, AVCodecContext * pCodecContext, AVFrame * pFrame) {
    // Supply raw packet data as input to a decoder
    // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga58bc4bf1e0ac59e27362597e467efff3
    int response = avcodec_send_packet(pCodecContext, pPacket);

    if (response < 0) {
        getLogger().error("Error while sending a packet to the decoder: %s", av_err2str(response));
        return response;
    }

    while (response >= 0) {
        // Return decoded output data (into a frame) from a decoder
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga11e6542c4e66d3028668788a1a74217c
        response = avcodec_receive_frame(pCodecContext, pFrame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            break;
        } else if (response < 0) {
            getLogger().error("Error while receiving a frame from the decoder: %s", av_err2str(response));
            return response;
        }

        if (response >= 0) {
            getLogger().debug("Frame %d (type=%c, size=%d bytes, format=%d) pts %ld key_frame %d [DTS %d]", pCodecContext -> frame_number, av_get_picture_type_char(pFrame -> pict_type), pFrame->pkt_size, pFrame->format, pFrame->pts, pFrame->key_frame, pFrame->coded_picture_number);

            if (pFrame->format != AV_PIX_FMT_YUV420P)
            {
                getLogger().error("Warning: the generated file may not be a grayscale image, but could e.g. be just the R component if the video format is RGB");
            }
            save_gray_frame(pFrame->data[0], pFrame->linesize[0], pFrame->width, pFrame->height, frame_filename);
        }
    }
    return 0;
}

void Cinema::VideoPlayer::Preload(std::string filename, bool _audio)
{
    audio = _audio;
    playing = false;

    AVFormatContext *pFormatContext = avformat_alloc_context();
    if (!pFormatContext) {
        getLogger().error("ERROR could not allocate memory for Format Context");
        return;
    }

    getLogger().debug("opening the input file (%s) and loading format (container) header", filename.c_str());

    if (avformat_open_input(&pFormatContext, filename.c_str(), NULL, NULL) != 0) {
        getLogger().error("ERROR could not open the file");
        return;
    }

    getLogger().debug("format %s, duration %ld us, bit_rate %ld", pFormatContext->iformat->name, pFormatContext->duration / AV_TIME_BASE, pFormatContext->bit_rate);

    if (avformat_find_stream_info(pFormatContext,  NULL) < 0) {
        getLogger().error("ERROR could not get the stream info");
        return;
    }

    AVCodec *pCodec = NULL;
    AVCodecParameters *pCodecParameters =  NULL;
    int video_stream_index = -1;

    for (int i = 0; i < pFormatContext->nb_streams; i++)
    {
        AVCodecParameters *pLocalCodecParameters =  NULL;
        pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
        getLogger().debug("AVStream->time_base before open coded %d/%d", pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
        getLogger().debug("AVStream->r_frame_rate before open coded %d/%d", pFormatContext->streams[i]->r_frame_rate.num, pFormatContext->streams[i]->r_frame_rate.den);
        getLogger().debug("AVStream->start_time %" PRId64, pFormatContext->streams[i]->start_time);
        getLogger().debug("AVStream->duration %" PRId64, pFormatContext->streams[i]->duration);

        getLogger().debug("finding the proper decoder (CODEC)");

        AVCodec *pLocalCodec = NULL;

        pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        if (pLocalCodec==NULL) {
            getLogger().error("ERROR unsupported codec!");
            // In this example if the codec is not found we just skip it
            continue;
        }

        if (pLocalCodecParameters -> codec_type == AVMEDIA_TYPE_VIDEO) {
            if (video_stream_index == -1) {
                video_stream_index = i;
                pCodec = pLocalCodec;
                pCodecParameters = pLocalCodecParameters;
            }

            getLogger().debug("Video Codec: resolution %d x %d", pLocalCodecParameters -> width, pLocalCodecParameters -> height);
            auto transform = get_transform();
            transform->set_localScale(UnityEngine::Vector3(10*(pLocalCodecParameters->height/pLocalCodecParameters->width), 10, 1));
        } 
        else if (pLocalCodecParameters -> codec_type == AVMEDIA_TYPE_AUDIO) {
            getLogger().debug("Audio Codec: %d channels, sample rate %d", pLocalCodecParameters -> channels, pLocalCodecParameters -> sample_rate);
        }

        getLogger().debug("\tCodec %s ID %d bit_rate %ld", pLocalCodec->name, pLocalCodec->id, pLocalCodecParameters->bit_rate);
    }
    
    if (video_stream_index == -1) {
        getLogger().error("File %s does not contain a video stream!", filename.c_str());
        return;
    }

    // https://ffmpeg.org/doxygen/trunk/structAVCodecContext.html
    AVCodecContext * pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext) {
        getLogger().error("failed to allocated memory for AVCodecContext");
        return;
    }

    // Fill the codec context based on the values from the supplied codec parameters
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#gac7b282f51540ca7a99416a3ba6ee0d16
    if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0) {
        getLogger().error("failed to copy codec params to codec context");
        return;
    }

    // Initialize the AVCodecContext to use the given AVCodec.
    // https://ffmpeg.org/doxygen/trunk/group__lavc__core.html#ga11f785a188d7d9df71621001465b0f1d
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0) {
        getLogger().error("failed to open codec through avcodec_open2");
        return;
    }

    // https://ffmpeg.org/doxygen/trunk/structAVFrame.html
    AVFrame * pFrame = av_frame_alloc();
    if (!pFrame) {
        getLogger().error("failed to allocated memory for AVFrame");
        return;
    }
    // https://ffmpeg.org/doxygen/trunk/structAVPacket.html
    AVPacket * pPacket = av_packet_alloc();
    if (!pPacket) {
        getLogger().error("failed to allocated memory for AVPacket");
        return;
    }

    int response = 0;
    int how_many_packets_to_process = 8;

    while (av_read_frame(pFormatContext, pPacket) >= 0) {
        // if it's the video stream
        if (pPacket -> stream_index == video_stream_index) {
            getLogger().debug("AVPacket->pts %"
                PRId64, pPacket -> pts);
            response = decode_packet(pPacket, pCodecContext, pFrame);
            if (response < 0)
                break;
            // stop it, otherwise we'll be saving hundreds of frames
            if (--how_many_packets_to_process <= 0) break;
        }
        // https://ffmpeg.org/doxygen/trunk/group__lavc__packet.html#ga63d5a489b419bd5d45cfd09091cbcbc2
        av_packet_unref(pPacket);
    }

    avformat_close_input(&pFormatContext);
    av_packet_free(&pPacket);
    av_frame_free(&pFrame);
    avcodec_free_context(&pCodecContext);
}