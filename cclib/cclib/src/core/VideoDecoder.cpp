#include "VideoDecoder.h"
#include "MessageCenter.h"
#include "SystemClock.h"

namespace CCPlayer
{

enum VideoDecoderStatus
{
    VIDEO_DECODER_STATUS_ENUM_UNKNOW,
    VIDEO_DECODER_STATUS_ENUM_WORKING,
    VIDEO_DECODER_STATUS_ENUM_SLEEPING,
    VIDEO_DECODER_STATUS_ENUM_DEADING,
    VIDEO_DECODER_STATUS_ENUM_DEADED,
    VIDEO_DECODER_STATUS_ENUM_MAX
};

CCVideoDecoder::CCVideoDecoder()
{
    printf("CCAudioDecoder address %p", this);
}

CCVideoDecoder::~CCVideoDecoder()
{
}

void CCVideoDecoder::PostMessage(MessageObjectId messageSender,
                            MessageObjectId messageReceiver,
                            MessageType msg,
                            Any anyParam)
{
    CCMessageCenter::GetInstance()->PostMessage(messageSender, messageReceiver, msg, anyParam);
}

void CCVideoDecoder::ReceiverMessage(const SmartPtr<Event>& rSmtEvent)
{
    m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    m_spinLockMessageQueue.UnLock();
}

bool CCVideoDecoder::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
{
    bool bGetMsg = false;
    m_spinLockMessageQueue.Lock();
    if(!m_messageQueue.empty())
    {
        rSmtEvent = m_messageQueue.front();
        m_messageQueue.pop();
        bGetMsg = true;
    }
    m_spinLockMessageQueue.UnLock();
    return bGetMsg;
}

void CCVideoDecoder::Run()
{
    VideoDecoderStatus status = VIDEO_DECODER_STATUS_ENUM_UNKNOW;

    AVFormatContext* pAVFormatCtx = NULL;
    AVCodecContext* pVideoCodecCtx = NULL;
    SwsContext* pImageConvertCtx = NULL;
    AVRational videoTimeBase;
    AVFrame *pDecodedFrame;
	AVPicture *pDecodePicture;
    int imgWidth = 0;
    int imgHeight = 0;
	int imgBufferLen = 0;
    int gotFrame = 0;

    int videoFrameQueueSize = 0;

    bool bDataManagerEof = false;

    while(m_bRunning)
    {
        SmartPtr<Event> event;
        if(PopFrontMessage(event))
        {
            switch(event.GetPtr()->type)
            {
                case MESSAGE_TYPE_ENUM_FINDED_VIDEO_STREAM:
                {
                    int vsIndex = -1;

                    std::vector<Any> videoStreamInfo
                                            = any_cast<std::vector<Any> >(event.GetPtr()->anyParams);

                    pAVFormatCtx = any_cast<AVFormatContext*>(videoStreamInfo[0]);
                    vsIndex = any_cast<int>(videoStreamInfo[1]);

                    if(vsIndex != -1)
                    {
                        GetCodecContext(pAVFormatCtx, vsIndex, &pVideoCodecCtx, &videoTimeBase);

                        GetVideoInformation(pVideoCodecCtx,
                                            &pImageConvertCtx,
                                            &imgWidth,
                                            &imgHeight,
                                            &imgBufferLen);

                        std::vector<Any> videoInformartion;
                        videoInformartion.push_back(Any(imgWidth));
                        videoInformartion.push_back(Any(imgHeight));

                        PostMessage(MESSAGE_OBJECT_ENUM_VIDEO_DECODER,
                                    MESSAGE_OBJECT_ENUM_VIDEO_RENDER,
                                    MESSAGE_TYPE_ENUM_GET_VIDEO_INFORMATION,
                                    Any(videoInformartion));

                        pDecodedFrame = avcodec_alloc_frame();
                        pDecodePicture = (AVPicture*)av_mallocz(sizeof(AVPicture));

                        PostMessage(MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                                    MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                    MESSAGE_TYPE_ENUM_VIDEO_DECODER_READY,
                                    Any());

                        //turn the video decoder status to working
                        status = VIDEO_DECODER_STATUS_ENUM_WORKING;
                    }
                }
                break;
                case MESSAGE_TYPE_ENUM_GET_VIDEO_PACKET:
                {
                    SmartPtr<CCPacket> shdPacket
                                            = any_cast<SmartPtr<CCPacket> >(event.GetPtr()->anyParams);

                    m_videoPacketQueue.push(shdPacket);
                }
                break;
                case MESSAGE_TYPE_ENUM_VIDEO_RENDER_A_FRAME:
                {
                    //status = VIDEO_DECODER_STATUS_ENUM_SLEEPING;
                    //we rendered a video frame
                    videoFrameQueueSize --;
                }
                break;
                case MESSAGE_TYPE_ENUM_DATA_MANAGER_EOF:
                {
                    bDataManagerEof = true;
                }
                break;
                case MESSAGE_TYPE_ENUM_CLIENT_STOP:
                {
                    status = VIDEO_DECODER_STATUS_ENUM_DEADED;
                }
                break;
            } // end switch case
        }//end get a message

        switch(status)
        {
            case VIDEO_DECODER_STATUS_ENUM_WORKING:
            {
                //std::cout << "Video Decoder are working" << std::endl;
                if(videoFrameQueueSize < MAX_VIDEO_PACKET_QUEUE_SIZE)
                {
                    if(!m_videoPacketQueue.empty())
                    {
                        SmartPtr<CCPacket> shdPacket
                                                = m_videoPacketQueue.front();
                        m_videoPacketQueue.pop();

                        AVPacket packet = shdPacket.GetPtr()->GetPacket();

                        avcodec_decode_video2(pVideoCodecCtx,
                                              pDecodedFrame,
                                              &gotFrame,
                                              &packet);

                        if (gotFrame)
                        {
                            unsigned char* pScaleBuffer =
                                            (unsigned char*)av_mallocz(imgBufferLen);

                            avpicture_fill(pDecodePicture,
                                           pScaleBuffer,
                                           PIX_FMT_RGBA,
                                           imgWidth,
                                           imgHeight);

                            sws_scale(pImageConvertCtx,
                                      pDecodedFrame->data,
                                      pDecodedFrame->linesize,
                                      0,
                                      pVideoCodecCtx->height,
                                      pDecodePicture->data,
                                      pDecodePicture->linesize);

                            // sychonize the video
                            int64_t iPts = pDecodedFrame->pkt_pts;

                            double dPts = iPts * av_q2d(videoTimeBase);
                            dPts *= AV_TIME_BASE / 1000.0;
                            //dPts += CCSystemClock::GetInstance()->GetRealStartTime();

                            //std::cout << "dPts======" << (int64_t)dPts << std::endl;

                            SmartPtr<VideoFrame> videoFrame(new VideoFrame(pScaleBuffer, imgBufferLen, dPts));
                            PostMessage(MESSAGE_OBJECT_ENUM_VIDEO_DECODER,
                                        MESSAGE_OBJECT_ENUM_VIDEO_RENDER,
                                        MESSAGE_TYPE_ENUM_GET_VIDEO_FRAME,
                                        Any(videoFrame));

                            av_free(pScaleBuffer);

                            //we've got a video frame
                            videoFrameQueueSize ++;
                        }// we get frame

                        PostMessage(MESSAGE_OBJECT_ENUM_VIDEO_DECODER,
                                    MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                    MESSAGE_TYPE_ENUM_VIDEO_DECODER_A_PACKET,
                                    Any());
                    }// end if video packet queue is not empty
                } // end if the audio frame is enough
                else if(bDataManagerEof)
                {
                    status = VIDEO_DECODER_STATUS_ENUM_DEADED;
                }else
                {
                    usleep(10 * 1000);
                    //Sleep(10);
                }
            }
            break;
            case VIDEO_DECODER_STATUS_ENUM_SLEEPING:
            {
                usleep(50 * 1000);
                //Sleep(50);
            }
            break;
            case VIDEO_DECODER_STATUS_ENUM_DEADING:
            {
                //m_bRunning = false;
                //continue;
            }
            break;
            case VIDEO_DECODER_STATUS_ENUM_DEADED:
            {
                PostMessage(MESSAGE_OBJECT_ENUM_VIDEO_DECODER,
                            MESSAGE_OBJECT_ENUM_PLAYER,
                            MESSAGE_TYPE_ENUM_VIDEO_DECODER_DEADED,
                            Any());
                
                m_bRunning = false;
                continue;
            }
            break;
            case VIDEO_DECODER_STATUS_ENUM_UNKNOW:
            {

            }
            break;
        }// end switch case status
    }

    std::cout << "The video decoder is deaded" << std::endl;
}

int CCVideoDecoder::GetCodecContext(AVFormatContext* pFormatCtx,
                                    int streamIndex,
                                    AVCodecContext** ppCodecContext,
                                    AVRational* pAudioTimeBase)
{
    *ppCodecContext = pFormatCtx->streams[streamIndex]->codec;
    *pAudioTimeBase = pFormatCtx->streams[streamIndex]->time_base;
}

int CCVideoDecoder::GetVideoInformation(AVCodecContext* pVideoCtx,
                                        SwsContext** ppImageConvertCtx,
                                        int *pImgWidth,
                                        int *pImgHeight,
                                        int *pImgBufferLen)
{
    AVCodec *pAVCodecVideo = avcodec_find_decoder(pVideoCtx->codec_id);
    //decoder->thread_count = 1;
    //avcodec_thread_init(pVideoCtx, 1);

    if(pAVCodecVideo == NULL)
    {
        return FAILURE;
    }

    if(avcodec_open2(pVideoCtx, pAVCodecVideo, NULL) != 0)
    {
        return FAILURE;
    }

    *pImgWidth = VIDEO_OUTPUT_WIDTH;
    *pImgHeight = VIDEO_OUTPUT_HEIGHT;

    *pImgBufferLen = avpicture_get_size(PIX_FMT_RGBA,
										VIDEO_OUTPUT_WIDTH,
										VIDEO_OUTPUT_HEIGHT);

    *ppImageConvertCtx = sws_getContext(pVideoCtx->width,
                   pVideoCtx->height,
                   pVideoCtx->pix_fmt,
                   VIDEO_OUTPUT_WIDTH,
                   VIDEO_OUTPUT_HEIGHT,
                   PIX_FMT_RGBA,
                   SWS_BICUBIC,
                   NULL,
                   NULL,
                   NULL);
    return SUCCESS;
}

}
