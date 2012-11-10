#include "DataManager.h"
#include "MessageCenter.h"
#include "SystemClock.h"
#include "AudioDef.h"
#include "VideoDef.h"

namespace CCPlayer
{

enum DataManagerStatus
{
    DATA_MANAGER_STATUS_ENUM_INIT,
    DATA_MANAGER_STATUS_ENUM_WORKING,
    DATA_MANAGER_STATUS_ENUM_SLEEPING,
    DATA_MANAGER_STATUS_ENUM_DEADED,
    DATA_MANAGER_STATUS_ENUM_MAX
};

#define DECODERS_STATUS_ENUM_NONE_READY     0
#define DECODERS_STATUS_ENUM_AUDIO_READY    1
#define DECODERS_STATUS_ENUM_VIDEO_READY    2
#define DECODERS_STATUS_ENUM_ALL_READY      3

CCDataManager::CCDataManager()
{
    printf("CCAudioDecoder address %p", this);
}

CCDataManager::~CCDataManager()
{
}

void CCDataManager::PostMessage(MessageObjectId messageSender,
                            MessageObjectId messageReceiver,
                            MessageType msg,
                            Any anyParam)
{
    CCMessageCenter::GetInstance()->PostMessage(messageSender,
                                                messageReceiver,
                                                msg,
                                                anyParam);
}

void CCDataManager::ReceiverMessage(const SmartPtr<Event>& rSmtEvent)
{
    //m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    //m_spinLockMessageQueue.UnLock();
}

bool CCDataManager::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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

void CCDataManager::Run()
{
    //CCSystemClock::GetInstance()->RegisterSystemAlarm(this);

//    std::string mediaUrl;
    //AVFormatContext *pAVFormatContext = NULL;
//    int asIndex = -1;
//    int vsIndex = -1;
    DataManagerStatus status = DATA_MANAGER_STATUS_ENUM_WORKING;
//    unsigned int decodersStatus = DECODERS_STATUS_ENUM_NONE_READY;

//    int audioPacketQueueSize = 0;
//    int videoPacketQueueSize = 0;

    while(m_bRunning)
    {
        SmartPtr<Event> event;
        if(PopFrontMessage(event))
        {
            switch(event.GetPtr()->type)
            {
                case MESSAGE_TYPE_ENUM_CLIENT_STOP:
                {
                    status = DATA_MANAGER_STATUS_ENUM_DEADED;
                }
                break;
                default:
                    std::cout << "Unknow Data Manager Message" << std::endl;
                //break;
                    printf("the message center is:%d\n",event.GetPtr()->type);
                    break;
            }
        }

        switch(status)
        {
            case DATA_MANAGER_STATUS_ENUM_INIT:
            {
            }
            break;
            case DATA_MANAGER_STATUS_ENUM_WORKING:
            {
                if(g_pPlayerContext->m_audioPacketQueue.size() < MAX_AUDIO_PACKET_QUEUE_SIZE
                   || g_pPlayerContext->m_videoPacketQueue.size() < MAX_VIDEO_PACKET_QUEUE_SIZE)
                {
                    SmartPtr<CCPacket> packet(new CCPacket());
                    if(av_read_frame(g_pPlayerContext->m_pAVFormatContext, packet.GetPtr()->GetPacketPointer()) < 0)
                    {
                        PostMessage(MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                    MESSAGE_OJBECT_ENUM_ALL,
                                    MESSAGE_TYPE_ENUM_DATA_MANAGER_EOF,
                                    Any());
                        
                        status = DATA_MANAGER_STATUS_ENUM_DEADED;
                    }

                    if(packet.GetPtr()->GetPacketPointer()->stream_index
                                == g_pPlayerContext->m_asIndex)
                    {                        
                        g_pPlayerContext->m_audioPacketQueue.push(packet);
                        
                    }else if(packet.GetPtr()->GetPacketPointer()->stream_index
                                == g_pPlayerContext->m_vsIndex)
                    {
                        g_pPlayerContext->m_videoPacketQueue.push(packet);
                    }
                }// if audio packet enough or video packet enough
                else
                {
                    usleep(10 * 1000);
                }
            }
            break;
            case DATA_MANAGER_STATUS_ENUM_SLEEPING:
            {
                usleep(50 * 1000);
            }
            break;
            case DATA_MANAGER_STATUS_ENUM_DEADED:
            {
                PostMessage(MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                            MESSAGE_OBJECT_ENUM_PLAYER,
                            MESSAGE_OBJECT_ENUM_DATA_MANAGER_DEADED,
                            Any());
                
                m_bRunning = false;
                continue;
            }
            break;
        } // end switch
    }

    //CCSystemClock::GetInstance()->UnRegisterSystemAlarm(this);
}

}
