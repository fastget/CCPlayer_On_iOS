#include "Player.h"
#include "MessageCenter.h"
#include "ModuleManager.h"
#include "Any.h"
#include "SystemClock.h"
#include "IPlayerDelegate.h"
#include "ALWrapper.h"

namespace CCPlayer
{
#define AUDIO_RENDER_DEADED     1
#define AUDIO_DECODER_DEADED    2
#define VIDEO_RENDER_DEADED     3
#define VIDEO_DECODER_DEADED    4
#define DATA_MANAGER_DEADED     5
#define ALL_MODULE_ARE_DEADED   (1 + 2 + 3 + 4 +5)

//we just declare the global player context
CCPlayerContext* g_pPlayerContext;
    
static int ffmpegLockManager( void **mtx, enum AVLockOp op )
{
    switch( op )
    {
        case AV_LOCK_CREATE:
        {
            *mtx =(pthread_mutex_t*) malloc( sizeof(pthread_mutex_t) );
                
            if( !*mtx ) {
                return 1;
            }
                
            return !!pthread_mutex_init( (pthread_mutex_t*)*mtx, NULL );
        }
                
        case AV_LOCK_OBTAIN:
        {
            return !!pthread_mutex_lock( (pthread_mutex_t*) *mtx );
        }
                
        case AV_LOCK_RELEASE:
        {
            return !!pthread_mutex_unlock( (pthread_mutex_t*)*mtx );
        }
                
        case AV_LOCK_DESTROY:
        {
            pthread_mutex_destroy( (pthread_mutex_t*)*mtx );
            free( *mtx );
            return 0;
        }
    }
        
    return 1;
}
    
CCPlayer::CCPlayer(IPlayerDelegate* pIPlayerDelegate)
:m_pAVFormatCtx(NULL)
{
    m_pIPlayerDelegate = pIPlayerDelegate;
    
    //this will start the message center thread , and will create the message center object
    CCMessageCenter::GetInstance()->InitMessageCenter();
    CCMessageCenter::GetInstance()->RegisterMessageReceiver(MESSAGE_OBJECT_ENUM_PLAYER, this);

    //This is the first time call , just for create the system clock instance
    CCSystemClock::GetInstance();
    
    //this is register for all ffmpeg action
    av_register_all();
    
    //this is for mutli-thread ffmpeg working
    if(av_lockmgr_register(ffmpegLockManager))
    {
        // TODO Failure av_lockmgr_register
    }
    
    m_pALWrapper = new CCALWrapper();
    
    Launch();
}

CCPlayer::~CCPlayer()
{
}
    
void CCPlayer::SetVolume(float volume)
{
    CCMessageCenter::GetInstance()->PostMessage(MESSAGE_OBJECT_ENUM_CLIENT,
                                                MESSAGE_OBJECT_ENUM_PLAYER,
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
    //m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    //m_spinLockMessageQueue.UnLock();
}

bool CCPlayer::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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
                    sleep(5);
                    
                    std::string mediaUrl = any_cast<std::string>(event.GetPtr()->anyParams);
                    
                    int ret = OpenFile(mediaUrl);
                    
                    if (ret == SUCCESS)
                    {
                        CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_DATA_MANAGER);
                    }
                    
                    if(ret == SUCCESS && g_pPlayerContext->m_asIndex != -1)
                    {
                        CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_AUDIO_DECODER);
                        CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_AUDIO_RENDER);
                    }
                    
                    if(ret == SUCCESS && g_pPlayerContext->m_vsIndex != -1)
                    {
                        CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_VIDEO_DECODER);
                        CCModuleManager::AddModule(MESSAGE_OBJECT_ENUM_VIDEO_RENDER);
                    }
                    
                    if(m_pIPlayerDelegate != NULL)
                    {
                        m_pIPlayerDelegate->OnCommandOpen(ret);
                    }
                    
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
                case MESSAGE_TYPE_ENUM_SET_VOLUME:
                {
                    float volume = any_cast<float>(event.GetPtr()->anyParams);
                    g_pPlayerContext->m_pALWrapper->SetVolume(volume);
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
        }else // end if get a message
        {
            //if nothing to do , just wait 10 ms , don't warry
            usleep(10 * 1000);
        }
    } // end of switch case
}
    
int CCPlayer::OpenFile(const std::string& mediaUrl)
{
    //AVFormatContext *pFormatCtx = NULL;
    
    g_pPlayerContext = new CCPlayerContext(m_pALWrapper);
    
    if(avformat_open_input(&g_pPlayerContext->m_pAVFormatContext,mediaUrl.c_str(), NULL, NULL) != 0)
	{
		std::cout << "can not open media file" << std::endl;
		return FAILURE;
	}else
	{
		std::cout << "open media file" << std::endl;
	}
    
    if(avformat_find_stream_info(g_pPlayerContext->m_pAVFormatContext, NULL) < 0)
	{
		std::cout << "can not retrieve file info" << std::endl;
		return FAILURE;
	}else
	{
		std::cout << "retrieve file info" << std::endl;
	}
    
	av_dump_format(g_pPlayerContext->m_pAVFormatContext,0, mediaUrl.c_str(), 0);
    
    g_pPlayerContext->FindContextInfomation(m_pIPlayerDelegate);
    
    //we just create the player context
    //g_pPlayerContext = new CCPlayerContext(pFormatCtx, m_pALWrapper, m_pIPlayerDelegate);
    
    return SUCCESS;
}
    
void CCPlayer::DestructPlayerSystem()
{
    g_pPlayerContext->ReleaseContextInformation();
    
    //close the format
    avformat_close_input(&g_pPlayerContext->m_pAVFormatContext);
    
    CCSystemClock::DestoryInstance();
    
    //this fuction will wait util the message center thread is deaded
    CCMessageCenter::DestoryInstance();
    
    if (m_pALWrapper != NULL)
    {
        //release the al subsystem
        delete m_pALWrapper;
        m_pALWrapper = NULL;
    }
    
    if (g_pPlayerContext != NULL)
    {
        delete g_pPlayerContext;
        g_pPlayerContext = NULL;
    }
    
    m_pIPlayerDelegate->OnCommandStop(0);
}

}
