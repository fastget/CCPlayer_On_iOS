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
    m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    m_spinLockMessageQueue.UnLock();
}

bool CCAudioDecoder::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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

void CCAudioDecoder::Run()
{
    AudioDecoderStatus status = AUDIO_DECODER_STATUS_ENUM_UNKNOW;

    AVFormatContext* pAVFormatCtx = NULL;
    AVCodecContext* pAudioCodecCtx = NULL;
    AVRational audioTimeBase;

    AVFrame* pDecodedFrame = NULL;
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
                case MESSAGE_TYPE_ENUM_FINDED_AUDIO_STREAM:
                {
                    CCChannels channels = -1;
                    CCRates rates = -1 ;
                    CCType type = CCType::unknow;
                    int asIndex = -1;

                    std::vector<Any> audioStreamInfo
                                        = any_cast<std::vector<Any> >(event.GetPtr()->anyParams);
                    pAVFormatCtx = any_cast<AVFormatContext*>(audioStreamInfo[0]);
                    asIndex = any_cast<int>(audioStreamInfo[1]);

                    GetCodecContext(pAVFormatCtx, asIndex, &pAudioCodecCtx, &audioTimeBase);
                    int ret = GetAudioInformation(pAudioCodecCtx, &channels, &rates, &type);

                    if(ret == 0)
                    {
                        std::vector<Any> audioInformartion;
                        audioInformartion.push_back(Any(channels));
                        audioInformartion.push_back(Any(rates));
                        audioInformartion.push_back(Any(type));

                        PostMessage(MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                                    MESSAGE_OBJECT_ENUM_AUDIO_RENDER,
                                    MESSAGE_TYPE_ENUM_GET_AUDIO_INFORMATION,
                                    Any(audioInformartion));
                        //crate the frame buffer size
                        pDecodedFrame = avcodec_alloc_frame();

                        PostMessage(MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                                    MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                    MESSAGE_TYPE_ENUM_AUDIO_DECODER_READY,
                                    Any());

                        //turn the audio decoder status to working
                        status = AUDIO_DECODER_STATUS_ENUM_WORKING;
                    }
                }
                break;
                case MESSAGE_TYPE_ENUM_GET_AUDIO_PACKET:
                {
                    SmartPtr<CCPacket> shdPacket =
                                            any_cast<SmartPtr<CCPacket> >(event.GetPtr()->anyParams);
                    m_audioPacketQueue.push(shdPacket);
                }
                break;
                case MESSAGE_TYPE_ENUM_AUDIO_RENDER_A_FRAME:
                {
                    //status = AUDIO_DECODER_STATUS_ENUM_SLEEPING;
                    //update
                    audioFrameQueueSize --;
                }
                break;
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

                if(audioFrameQueueSize < MAX_AUDIO_FRAME_QUEUE_SIZE)
                {
                    if(!m_audioPacketQueue.empty())
                    {
                        //static int count = 0;
                        //std::cout << "decoder a audio packet" << ++count << std::endl;

                        SmartPtr<CCPacket> shdPacket = m_audioPacketQueue.front();
                        m_audioPacketQueue.pop();

                        AVPacket packet = shdPacket.GetPtr()->GetPacket();

                        while(packet.size > 0)
                        {
                            avcodec_get_frame_defaults(pDecodedFrame);

                            decodedLen = avcodec_decode_audio4(pAudioCodecCtx,
                                                               pDecodedFrame,
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
                                                                        pAudioCodecCtx->channels,
                                                                        pDecodedFrame->nb_samples,
                                                                        pAudioCodecCtx->sample_fmt,
                                                                        1);

                                SmartPtr<AudioFrame> audioFrame(new AudioFrame(reinterpret_cast<const char*>(pDecodedFrame->data[0]),
                                                                               decodedDataSize));
                                PostMessage(MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                                            MESSAGE_OBJECT_ENUM_AUDIO_RENDER,
                                            MESSAGE_TYPE_ENUM_GET_AUDIO_FRAME,
                                            Any(audioFrame));
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

int CCAudioDecoder::GetCodecContext(AVFormatContext* pFormatCtx,
                                    int streamIndex,
                                    AVCodecContext** ppCodecContext,
                                    AVRational* pAudioTimeBase)
{
    *ppCodecContext = pFormatCtx->streams[streamIndex]->codec;
    *pAudioTimeBase = pFormatCtx->streams[streamIndex]->time_base;
}

int CCAudioDecoder::GetAudioInformation(AVCodecContext *pAudioCtx,
                                        CCChannels* pChannels,
                                        CCRates* pRates,
                                        CCType* pType)
{
    AVCodec *pAVCodecAudio = avcodec_find_decoder(pAudioCtx->codec_id);

    if(pAVCodecAudio == NULL)
    {
        return FAILURE;
    }

    if(avcodec_open2(pAudioCtx, pAVCodecAudio, NULL) != 0)
    {
        return FAILURE;
    }

    if (pAudioCtx->channels < 1
            || pAudioCtx->channels > 8
            || pAudioCtx->channels == 3
            || pAudioCtx->channels == 5)
    {
        std::cout << "Unsupported channels " << std::endl;
        return FAILURE;
    }

    *pChannels = pAudioCtx->channels;
    *pRates = pAudioCtx->sample_rate;
    if (pAudioCtx->sample_fmt == AV_SAMPLE_FMT_U8)
    {
        *pType = CCType::u8;
    }
    else if (pAudioCtx->sample_fmt == AV_SAMPLE_FMT_S16)
    {
        *pType = CCType::s16;
    }
    else if (pAudioCtx->sample_fmt == AV_SAMPLE_FMT_FLT)
    {
        *pType = CCType::f32;
    }
    else if (pAudioCtx->sample_fmt == AV_SAMPLE_FMT_DBL)
    {
        *pType = CCType::d64;
    }
    else if (pAudioCtx->sample_fmt == AV_SAMPLE_FMT_S32
            && sizeof(int32_t) == sizeof(float))
    {
        // we need to convert this to AV_SAMPLE_FMT_FLT after decoding
        *pType = CCType::f32;
    }
    else
    {
        std::cout << "Unsupported the format " << std::endl;
        return FAILURE;
    }

    return 0;
}

}
