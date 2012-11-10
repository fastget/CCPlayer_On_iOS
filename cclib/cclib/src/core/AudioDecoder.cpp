#include "AudioDecoder.h"
#include "MessageCenter.h"

namespace CCPlayer
{

enum AudioDecoderStatus
{
    AUDIO_DECODER_STATUS_ENUM_UNKNOW,
    AUDIO_DECODER_STATUS_ENUM_WORKING,
    AUDIO_DECODER_STATUS_ENUM_SLEEPING,
    AUDIO_DECODER_STATUS_ENUM_DEADED,
    AUDIO_DECODER_STATUS_ENUM_MAX
};

CCAudioDecoder::CCAudioDecoder()
{
    printf("CCAudioDecoder address %p", this);
}

CCAudioDecoder::~CCAudioDecoder()
{
}

void CCAudioDecoder::PostMessage(MessageObjectId messageSender,
                            MessageObjectId messageReceiver,
                            MessageType msg,
                            Any anyParam)
{
    CCMessageCenter::GetInstance()->PostMessage(messageSender, messageReceiver, msg, anyParam);
}

void CCAudioDecoder::ReceiverMessage(const SmartPtr<Event>& rSmtEvent)
{
    //m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    //m_spinLockMessageQueue.UnLock();
}

bool CCAudioDecoder::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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

void CCAudioDecoder::Run()
{
    AudioDecoderStatus status = AUDIO_DECODER_STATUS_ENUM_WORKING;

    //AVFormatContext* pAVFormatCtx = NULL;
//    AVCodecContext* pAudioCodecCtx = NULL;
//    AVRational audioTimeBase;

//    AVFrame* pDecodedFrame = NULL;
    int gotFrame = 0;
    int decodedLen = 0;

    int audioFrameQueueSize = 0;

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
                    status = AUDIO_DECODER_STATUS_ENUM_DEADED;
                }
                break;
            } // end switch case
        }// end if get a message

        //working in somethings
        switch(status)
        {
            case AUDIO_DECODER_STATUS_ENUM_WORKING:
            {
                //std::cout << "Audio Decoder are working" << std::endl;

                if(g_pPlayerContext->m_audioFrameQueue.size() < MAX_AUDIO_FRAME_QUEUE_SIZE)
                {
                    if(!g_pPlayerContext->m_audioPacketQueue.empty())
                    {
                        SmartPtr<CCPacket> shdPacket = g_pPlayerContext->m_audioPacketQueue.front();
                        g_pPlayerContext->m_audioPacketQueue.pop();

                        AVPacket packet = shdPacket.GetPtr()->GetPacket();

                        while(packet.size > 0)
                        {
                            avcodec_get_frame_defaults(g_pPlayerContext->m_pAudioDecodedFrame);

                            decodedLen = avcodec_decode_audio4(g_pPlayerContext->m_pAudioCodecCtx,
                                                               g_pPlayerContext->m_pAudioDecodedFrame,
                                                               &gotFrame,
                                                               &packet);

                            packet.data += decodedLen;
                            packet.size -= decodedLen;

                            if(gotFrame)
                            {
                                //std::cout << "Get a frame" << std::endl;
                                //count = 0;
                                //increment the queue size count
                                audioFrameQueueSize ++;

                                int decodedDataSize = av_samples_get_buffer_size(NULL,
                                                                        g_pPlayerContext->m_pAudioCodecCtx->channels,
                                                                        g_pPlayerContext->m_pAudioDecodedFrame->nb_samples,
                                                                        g_pPlayerContext->m_pAudioCodecCtx->sample_fmt,
                                                                        1);

                                SmartPtr<AudioFrame> audioFrame(new AudioFrame(reinterpret_cast<const char*>(
                                                                                    g_pPlayerContext->m_pAudioDecodedFrame->data[0]),
                                                                               decodedDataSize));
                                g_pPlayerContext->m_audioFrameQueue.push(audioFrame);
//                                PostMessage(MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
//                                            MESSAGE_OBJECT_ENUM_AUDIO_RENDER,
//                                            MESSAGE_TYPE_ENUM_GET_AUDIO_FRAME,
//                                            Any(audioFrame));
                            }
                        }// end while decoder packet


                        //tell the datamanager we have decoded a packet
                        PostMessage(MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                                    MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                    MESSAGE_TYPE_ENUM_AUDIO_DEOCDER_A_PACKET,
                                    Any());
                    }// end the packet queue is not empty
                }// end not enough audio frame
                else if(bDataManagerEof)//there is no data for data manager
                {
                    status = AUDIO_DECODER_STATUS_ENUM_DEADED;
                }else
                {
                    usleep(10 * 1000);
                    
                    //Sleep(10);
                }
            }
            break;
            case AUDIO_DECODER_STATUS_ENUM_SLEEPING:
            {
                usleep(50 * 1000);
                
                //Sleep(50);
            }
            break;
            case AUDIO_DECODER_STATUS_ENUM_DEADED:
            {
                //case some thing , I just dead , before this , I should tell the player
                PostMessage(MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                            MESSAGE_OBJECT_ENUM_PLAYER,
                            MESSAGE_TYPE_ENUM_AUDIO_DECODER_DEADED,
                            Any());
                
                m_bRunning = false;
                continue;
            }
            break;
            case AUDIO_DECODER_STATUS_ENUM_UNKNOW:
            {
                
            }
            break;
        }// end switch case
    }

    std::cout << "The audio decoder is deaded" << std::endl;
}

}
