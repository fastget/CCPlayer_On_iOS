#include "AudioRender.h"
#include "MessageCenter.h"
#include "ALWrapper.h"
#include "SystemClock.h"

namespace CCPlayer
{

enum AudioRenderStatus
{
    AUDIO_RENDER_STATUS_ENUM_UNKNOW,
    AUDIO_RENDER_STATUS_ENUM_INITTED,
    AUDIO_RENDER_STATUS_ENUM_UPDATING,
    AUDIO_RENDER_STATUS_ENUM_SLEEPING,
    AUDIO_REDNER_STATUS_ENUM_DEADING,
    AUDIO_RENDER_STATUS_ENUM_DEADED,
    AUDIO_RENDER_STATUS_ENUM_MAX
};

CCAudioRender::CCAudioRender()
{
    printf("CCAudioDecoder address %p", this);
}

CCAudioRender::~CCAudioRender()
{
}

void CCAudioRender::PostMessage(MessageObjectId messageSender,
                            MessageObjectId messageReceiver,
                            MessageType msg,
                            Any anyParam)
{
    CCMessageCenter::GetInstance()->PostMessage(messageSender, messageReceiver, msg, anyParam);
}

void CCAudioRender::ReceiverMessage(const SmartPtr<Event>& rSmtEvent)
{
    //m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    //m_spinLockMessageQueue.UnLock();
}

bool CCAudioRender::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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

void CCAudioRender::Run()
{
    AudioRenderStatus status = AUDIO_RENDER_STATUS_ENUM_INITTED;
    bool bDataManagerEof = false;
    
//    ALenum audFormat = GetAudioFormat(g_pPlayerContext->m_channels, g_pPlayerContext->m_type);
    
    g_pPlayerContext->m_pALWrapper->SetAudioCtx(g_pPlayerContext->m_channels,
                          g_pPlayerContext->m_rates,
                          g_pPlayerContext->m_audFormat);

    while(m_bRunning)
    {
        SmartPtr<Event> event;

        if(PopFrontMessage(event))
        {
            switch(event.GetPtr()->type)
            {
                case MESSAGE_TYPE_ENUM_SET_VOLUME:
                {
                    float volume = any_cast<float>(event.GetPtr()->anyParams);
                    g_pPlayerContext->m_pALWrapper->SetVolume(volume);
                }
                break;
                case MESSAGE_TYPE_ENUM_DATA_MANAGER_EOF:
                {
                    bDataManagerEof = true;
                }
                break;
                case MESSAGE_TYPE_ENUM_AUDIO_PAUSE:
                {
                    status = AUDIO_RENDER_STATUS_ENUM_SLEEPING;
                }
                break;
                case MESSAGE_TYPE_ENUM_AUDIO_CONTINUE:
                {
                    status = AUDIO_RENDER_STATUS_ENUM_UPDATING;
                }
                break;
                case MESSAGE_TYPE_ENUM_CLIENT_STOP:
                {
                    status = AUDIO_RENDER_STATUS_ENUM_DEADED;
                }
            } // end switch case
        } // end if get a message


        switch(status)
        {
            case AUDIO_RENDER_STATUS_ENUM_UNKNOW:
            {
                //do nothing at all
            }
            break;
            case AUDIO_RENDER_STATUS_ENUM_INITTED:
            {
                if(g_pPlayerContext->m_audioFrameQueue.size() >= AUDIO_BUFFER_NUMBER)
                {
                    std::cout << "Init the audio buffers" << std::endl;
                    for(int i=0; i<AUDIO_BUFFER_NUMBER;i++)
                    {
                        SmartPtr<AudioFrame> shrdAudioFrame = g_pPlayerContext->m_audioFrameQueue.front();
                        g_pPlayerContext->m_audioFrameQueue.pop();

                        g_pPlayerContext->m_pALWrapper->InitAudioFrame(shrdAudioFrame.GetPtr(), i);
                    }

                    g_pPlayerContext->m_pALWrapper->Play();
                    
                    //usleep(30 * 1000);
                    CCSystemClock::GetInstance()->SetVideoStartRender(true);

                    status = AUDIO_RENDER_STATUS_ENUM_UPDATING;
                }
            } // case AUDIO_RENDER_STATUS_ENUM_INITTED
            break;
            case AUDIO_RENDER_STATUS_ENUM_UPDATING:
            {
                CCSystemClock::GetInstance()->SetRealPlayedTime(g_pPlayerContext->m_pALWrapper->GetReadPlayedTime());
                usleep(20 * 1000);
                
                //std::cout << "audio frame empty()" << m_audioFrameQueue.empty()<< std::endl;
                if(g_pPlayerContext->m_pALWrapper->NeedData() && !g_pPlayerContext->m_audioFrameQueue.empty())
                {
                    //std::cout << "audio render data" << std::endl;
                    SmartPtr<AudioFrame> shrdAudioFrame = g_pPlayerContext->m_audioFrameQueue.front();
                    g_pPlayerContext->m_audioFrameQueue.pop();

                    g_pPlayerContext->m_pALWrapper->UpdateAudioFrame(shrdAudioFrame.GetPtr());

                    PostMessage(MESSAGE_OBJECT_ENUM_AUDIO_RENDER,
                                MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                                MESSAGE_TYPE_ENUM_AUDIO_RENDER_A_FRAME,
                                Any());
                }else if(bDataManagerEof)
                {
                    status = AUDIO_RENDER_STATUS_ENUM_DEADED;
                }
                
                g_pPlayerContext->m_pALWrapper->Play();
                
            } //end case AUDIO_RENDER_STATUS_ENUM_UPDATING
            break;
            case AUDIO_RENDER_STATUS_ENUM_SLEEPING:
            {
                usleep(100 * 1000);
                //Sleep(100);
            }
            break;
            case AUDIO_REDNER_STATUS_ENUM_DEADING:
            {
                m_bRunning = false;
                continue;
            }
            break;
            case AUDIO_RENDER_STATUS_ENUM_DEADED:
            {
                PostMessage(MESSAGE_OBJECT_ENUM_AUDIO_RENDER,
                            MESSAGE_OBJECT_ENUM_PLAYER,
                            MESSAGE_TYPE_ENUM_AUDIO_RENDER_DEADED,
                            Any());
                
                m_bRunning = false;
                continue;
            }
            break;
        } // end switch(status)
    }

    std::cout << "The audio render is deaded" << std::endl;
}

}
