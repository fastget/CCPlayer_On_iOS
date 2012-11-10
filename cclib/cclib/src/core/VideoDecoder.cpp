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
    //m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    //m_spinLockMessageQueue.UnLock();
}

bool CCVideoDecoder::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
{
    bool bGetMsg = false;
    //m_spinLockMessageQueue.Lock();
    if(!m_messageQueue.empty())
    {
        rSmtEvent = m_messageQueue.front();
        m_messageQueue.pop();
        bGetMsg = true;
    }
    //m_spinLockMessageQueue.UnLock();
    return bGetMsg;
}

void CCVideoDecoder::Run()
{
    VideoDecoderStatus status = VIDEO_DECODER_STATUS_ENUM_WORKING;

    int gotFrame = 0;

    bool bDataManagerEof = false;

    while(m_bRunning)
    {
        SmartPtr<Event> event;
        if(PopFrontMessage(event))
        {
            switch(event.GetPtr()->type)
            {
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
                if(g_pPlayerContext->m_videoFrameQueue.size() < MAX_VIDEO_PACKET_QUEUE_SIZE)
                {
                    if(!g_pPlayerContext->m_videoPacketQueue.empty())
                    {
                        SmartPtr<CCPacket> shdPacket
                                                = g_pPlayerContext->m_videoPacketQueue.front();
                        g_pPlayerContext->m_videoPacketQueue.pop();

                        AVPacket packet = shdPacket.GetPtr()->GetPacket();

                        avcodec_decode_video2(g_pPlayerContext->m_pVideoCodecCtx,
                                              g_pPlayerContext->m_pVideoDecodedFrame,
                                              &gotFrame,
                                              &packet);

                        if (gotFrame)
                        {
                            unsigned char* pScaleBuffer =
                                            (unsigned char*)av_mallocz(g_pPlayerContext->m_imgBufferLen);

                            avpicture_fill(g_pPlayerContext->m_pDecodedPicture,
                                           pScaleBuffer,
                                           PIX_FMT_RGBA,
                                           g_pPlayerContext->m_imgWidth,
                                           g_pPlayerContext->m_imgHeight);

                            sws_scale(g_pPlayerContext->m_pImageConvertCtx,
                                      g_pPlayerContext->m_pVideoDecodedFrame->data,
                                      g_pPlayerContext->m_pVideoDecodedFrame->linesize,
                                      0,
                                      g_pPlayerContext->m_pVideoCodecCtx->height,
                                      g_pPlayerContext->m_pDecodedPicture->data,
                                      g_pPlayerContext->m_pDecodedPicture->linesize);

                            // sychonize the video
                            int64_t iPts = g_pPlayerContext->m_pVideoDecodedFrame->pkt_pts;

                            double dPts = iPts * av_q2d(g_pPlayerContext->m_videoTimeBase);
                            dPts *= AV_TIME_BASE / 1000.0;
                            //dPts += CCSystemClock::GetInstance()->GetRealStartTime();

                            //std::cout << "dPts======" << (int64_t)dPts << std::endl;

                            SmartPtr<VideoFrame> videoFrame(new VideoFrame(pScaleBuffer, g_pPlayerContext->m_imgBufferLen, dPts));
                            g_pPlayerContext->m_videoFrameQueue.push(videoFrame);

                            av_free(pScaleBuffer);
                        }// we get frame
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

}
