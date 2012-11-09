#include "Player.h"
#include "ICommandResponse.h"
#include "MessageCenter.h"
#include "ModuleManager.h"
#include "Any.h"
#include "SystemClock.h"

namespace CCPlayer
{
#define AUDIO_RENDER_DEADED     1
#define AUDIO_DECODER_DEADED    2
#define VIDEO_RENDER_DEADED     3
#define VIDEO_DECODER_DEADED    4
#define DATA_MANAGER_DEADED     5
#define ALL_MODULE_ARE_DEADED   (1 + 2 + 3 + 4 +5)
    
CCPlayer::CCPlayer(ICommandResponse* pICommandResponse)
:m_pAVFormatCtx(NULL)
{
    m_pCommandResponseObject = pICommandResponse;
    
    //this will start the message center thread , and will create the message center object
    CCMessageCenter::GetInstance()->InitMessageCenter();
    CCMessageCenter::GetInstance()->RegisterMessageReceiver(MESSAGE_OBJECT_ENUM_PLAYER, this);

    //This is the first time call , just for create the system clock instance
    CCSystemClock::GetInstance();
    
    Launch();
}

CCPlayer::~CCPlayer()
{
    //we can not unregister the message center here
    //CCMessageCenter::GetInstance()->UnRegisterMessageReceiver(MESSAGE_OBJECT_ENUM_PLAYER);
}

void CCPlayer::SetGLRenderView(IGLView* pIGLRenderView)
{
    CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                                MESSAGE_OBJECT_ENUM_VIDEO_RENDER,
                                                MESSAGE_TYPE_ENUM_INIT_GLRENDER_OBJECT,
                                                Any(pIGLRenderView));
}
    
void CCPlayer::SetVolume(float volume)
{
    CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                                MESSAGE_OBJECT_ENUM_AUDIO_RENDER,
                                                MESSAGE_TYPE_ENUM_SET_VOLUME,
                                                Any(volume));
}

void CCPlayer::Open(const std::string& loadParams)
{
    CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_CLIENT,
                                                MESSAGE_OBJECT_ENUM_PLAYER,
                                                COMMAND_TYPE_ENUM_OPEN,
                                                Any(loadParams));
}

void CCPlayer::Pause()
{
    CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_CLIENT,
                                                MESSAGE_OBJECT_ENUM_PLAYER,
                                                COMMAND_TYPE_ENUM_PAUSE,
                                                Any());
}

void CCPlayer::Continue()
{
    CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_CLIENT,
                                                MESSAGE_OBJECT_ENUM_PLAYER,
                                                COMMAND_TYPE_ENUM_CONTINUE,
                                                Any());
}

void CCPlayer::Stop()
{
    CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_CLIENT,
                                                MESSAGE_OBJECT_ENUM_PLAYER,
                                                COMMAND_TYPE_ENUM_STOP,
                                                Any());
}

//get total display time
int CCPlayer::GetTotalDurationBySecond(int64_t* pTotalDuration)
{
    if(m_pAVFormatCtx != NULL)
    {
        *pTotalDuration = avio_size(m_pAVFormatCtx->pb) / (m_pAVFormatCtx->bit_rate / 8);
        return 0;
    }
    return -1;
}

int CCPlayer::GetCurrentPostionBySecond(int64_t* pCurrPostion)
{
    if(m_pAVFormatCtx != NULL)
    {
        std::cout << "current postion is : " << avio_tell(m_pAVFormatCtx->pb) << std::endl;

        *pCurrPostion = avio_tell(m_pAVFormatCtx->pb) / (m_pAVFormatCtx->bit_rate / 8);
        return 0;
    }
    return -1;
}

void CCPlayer::PostMessage(MessageObjectId messageSender,
                            MessageObjectId messageReceiver,
                            MessageType msg,
                            Any anyParam)
{
    CCMessageCenter::GetInstance()->PostMessage(messageSender,
                                                messageReceiver,
                                                msg,
                                                anyParam);
}

void CCPlayer::ReceiverMessage(const SmartPtr<Event>& rSmtEvent)
{
    m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    m_spinLockMessageQueue.UnLock();
}

bool CCPlayer::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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

void CCPlayer::Run()
{
    int isAllModuleAreDeaded = 0;
    while(m_bRunning)
    {
        SmartPtr<Event> event;
        if(PopFrontMessage(event))
        {
            switch(event.GetPtr()->type)
            {
                case COMMAND_TYPE_ENUM_OPEN:
                {
                    std::string mediaUrl = any_cast<std::string>(event.GetPtr()->anyParams);

                    CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_DATA_MANAGER);

                    PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                MESSAGE_TYPE_ENUM_OPEN_FILE,
                                Any(mediaUrl));
                }
                break;
                case COMMAND_TYPE_ENUM_CONTINUE:
                {
                    CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                                                MESSAGE_OBJECT_ENUM_AUDIO_RENDER,
                                                                MESSAGE_TYPE_ENUM_AUDIO_CONTINUE,
                                                                Any());

                    CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                                                MESSAGE_OBJECT_ENUM_VIDEO_RENDER,
                                                                MESSAGE_TYPE_ENUM_VIDEO_CONTINUE,
                                                                Any());
                }
                break;
                case COMMAND_TYPE_ENUM_PAUSE:
                {
                        CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                                                    MESSAGE_OBJECT_ENUM_AUDIO_RENDER,
                                                                    MESSAGE_TYPE_ENUM_AUDIO_PAUSE,
                                                                    Any());

                        CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                                                    MESSAGE_OBJECT_ENUM_VIDEO_RENDER,
                                                                    MESSAGE_TYPE_ENUM_VIDEO_PAUSE,
                                                                    Any());
                }
                break;
                //the user stop the player
                case COMMAND_TYPE_ENUM_STOP:
                {
                    PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                MESSAGE_OJBECT_ENUM_ALL,
                                MESSAGE_TYPE_ENUM_CLIENT_STOP,
                                Any());
                }
                break;
                case MESSAGE_TYPE_ENUM_OPENED_FILE:
                {
                    std::vector<Any> openedParams = any_cast<std::vector<Any> >(event.GetPtr()->anyParams);
                    int ret = any_cast<int>(openedParams[0]);
                    m_pAVFormatCtx = any_cast<AVFormatContext*>(openedParams[1]);
                    int asIndex = any_cast<int>(openedParams[2]);
                    int vsIndex = any_cast<int>(openedParams[3]);

                    if(asIndex != -1)
                    {
                        CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_AUDIO_DECODER);

                        std::vector<Any> audioStreamInfo;
                        audioStreamInfo.push_back(Any(m_pAVFormatCtx));
                        audioStreamInfo.push_back(Any(asIndex));

                        PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                    MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                                    MESSAGE_TYPE_ENUM_FINDED_AUDIO_STREAM,
                                    Any(audioStreamInfo));

                        CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_AUDIO_RENDER);
                    }

                    if(vsIndex != -1)
                    {
                        CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_VIDEO_DECODER);

                        std::vector<Any> videoStreamInfo;
                        videoStreamInfo.push_back(Any(m_pAVFormatCtx));
                        videoStreamInfo.push_back(Any(vsIndex));

                        PostMessage(MESSAGE_OBJECT_ENUM_PLAYER,
                                    MESSAGE_OBJECT_ENUM_VIDEO_DECODER,
                                    MESSAGE_TYPE_ENUM_FINDED_VIDEO_STREAM,
                                    Any(videoStreamInfo));

                        CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_VIDEO_RENDER);
                    }

                    if(m_pCommandResponseObject != NULL)
                    {
                        m_pCommandResponseObject->OnCommandOpen(ret);
                    }
                }
                break;
                case MESSAGE_TYPE_ENUM_DATA_MANAGER_EOF:
                {
//                    CCModuleManager::DeleteModule(MESSAGE_OBJECT_ENUM_DATA_MANAGER);
//                    
//                    isAllModuleAreDeaded += DATA_MANAGER_DEADED;
//                    
//                    if (isAllModuleAreDeaded == ALL_MODULE_ARE_DEADED)
//                    {
//                        DestructPlayerSystem();
//                    }
                    //m_bRunning = false;
                    //continue;
                }
                break;
                case MESSAGE_TYPE_ENUM_AUDIO_RENDER_DEADED:
                {
                    CCModuleManager::DeleteModule(MESSAGE_OBJECT_ENUM_AUDIO_RENDER);
                    
                    isAllModuleAreDeaded += AUDIO_RENDER_DEADED;
                    
                    if (isAllModuleAreDeaded == ALL_MODULE_ARE_DEADED)
                    {
                        DestructPlayerSystem();
                    }
                }
                break;
                case MESSAGE_TYPE_ENUM_AUDIO_DECODER_DEADED:
                {
                    CCModuleManager::DeleteModule(MESSAGE_OBJECT_ENUM_AUDIO_DECODER);
                    
                    isAllModuleAreDeaded += AUDIO_DECODER_DEADED;
                    
                    if (isAllModuleAreDeaded == ALL_MODULE_ARE_DEADED)
                    {
                        DestructPlayerSystem();
                    }
                }
                break;
                case MESSAGE_TYPE_ENUM_VIDEO_RENDER_DEADED:
                {
                    CCModuleManager::DeleteModule(MESSAGE_OBJECT_ENUM_VIDEO_RENDER);
                    
                    isAllModuleAreDeaded += VIDEO_RENDER_DEADED;
                    
                    if (isAllModuleAreDeaded == ALL_MODULE_ARE_DEADED)
                    {
                        DestructPlayerSystem();
                    }
                }
                break;
                case MESSAGE_TYPE_ENUM_VIDEO_DECODER_DEADED:
                {
                    CCModuleManager::DeleteModule(MESSAGE_OBJECT_ENUM_VIDEO_DECODER);
                    
                    isAllModuleAreDeaded += VIDEO_DECODER_DEADED;
                    
                    if (isAllModuleAreDeaded == ALL_MODULE_ARE_DEADED)
                    {
                        DestructPlayerSystem();
                    }
                }
                    break;
                case MESSAGE_OBJECT_ENUM_DATA_MANAGER_DEADED:
                {
                    CCModuleManager::DeleteModule(MESSAGE_OBJECT_ENUM_DATA_MANAGER);
                    
                    isAllModuleAreDeaded += DATA_MANAGER_DEADED;
                    
                    if (isAllModuleAreDeaded == ALL_MODULE_ARE_DEADED)
                    {
                        DestructPlayerSystem();
                    }
                }
                break;
            }
        }

        //std::cout << "The play is running" << std::endl;
    }
}
    
void CCPlayer::DestructPlayerSystem()
{
    CCSystemClock::DestoryInstance();
    
    //this fuction will wait util the message center thread is deaded
    CCMessageCenter::DestoryInstance();
    
    m_pCommandResponseObject->OnCommandStop(0);
}

}
