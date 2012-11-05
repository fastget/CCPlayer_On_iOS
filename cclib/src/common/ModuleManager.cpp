#include "ModuleManager.h"
#include "MessageCenter.h"

#include "AudioRender.h"
#include "AudioDecoder.h"
#include "VideoRender.h"
#include "VideoDecoder.h"
#include "DataManager.h"

namespace CCPlayer
{

CCModuleManager::CCModuleManager()
{
}

CCModuleManager::~CCModuleManager()
{
}

void CCModuleManager::AddModule(MessageObjectId messageObjectId)
{
    switch(messageObjectId)
    {
        case MESSAGE_OBJECT_ENUM_VIDEO_RENDER:
            {
                CCVideoRender* pVideoRender = new CCVideoRender();
                pVideoRender->Launch();
                CCMessageCenter::GetInstance()->RegisterMessageReceiver(messageObjectId, pVideoRender);
            }
            break;
        case MESSAGE_OBJECT_ENUM_AUDIO_RENDER:
            {
                CCAudioRender* pAudioRender = new CCAudioRender();
                pAudioRender->Launch();
                CCMessageCenter::GetInstance()->RegisterMessageReceiver(messageObjectId, pAudioRender);
            }
            break;
        case MESSAGE_OBJECT_ENUM_VIDEO_DECODER:
            {
                CCVideoDecoder* pVideoDecoder = new CCVideoDecoder();
                pVideoDecoder->Launch();
                CCMessageCenter::GetInstance()->RegisterMessageReceiver(messageObjectId, pVideoDecoder);
            }
            break;
        case MESSAGE_OBJECT_ENUM_AUDIO_DECODER:
            {
                CCAudioDecoder* pAudioDecoder = new CCAudioDecoder();
                pAudioDecoder->Launch();
                CCMessageCenter::GetInstance()->RegisterMessageReceiver(messageObjectId, pAudioDecoder);
            }
            break;
        case MESSAGE_OBJECT_ENUM_DATA_MANAGER:
            {
                CCDataManager* pDataManager = new CCDataManager();
                pDataManager->Launch();
                CCMessageCenter::GetInstance()->RegisterMessageReceiver(messageObjectId, pDataManager);
            }
            break;
        default:
            printf("Unknow message object id\n");
            break;
    }
}

void CCModuleManager::DeleteModule(MessageObjectId messageObjectId)
{
    switch(messageObjectId)
    {
        case MESSAGE_OBJECT_ENUM_VIDEO_RENDER:
            {
                IMessageReceiver* pMessageReceiver = CCMessageCenter::GetInstance()->GetMessageReceiver(messageObjectId);

                CCMessageCenter::GetInstance()->UnRegisterMessageReceiver(messageObjectId);

                if(pMessageReceiver != NULL)
                {
                    delete pMessageReceiver;
                    pMessageReceiver = NULL;
                }
            }
            break;
        case MESSAGE_OBJECT_ENUM_AUDIO_RENDER:
            {
                IMessageReceiver* pMessageReceiver = CCMessageCenter::GetInstance()->GetMessageReceiver(messageObjectId);

                CCMessageCenter::GetInstance()->UnRegisterMessageReceiver(messageObjectId);

                if(pMessageReceiver != NULL)
                {
                    delete pMessageReceiver;
                    pMessageReceiver = NULL;
                }
            }
            break;
        case MESSAGE_OBJECT_ENUM_VIDEO_DECODER:
            {
                IMessageReceiver* pMessageReceiver = CCMessageCenter::GetInstance()->GetMessageReceiver(messageObjectId);

                CCMessageCenter::GetInstance()->UnRegisterMessageReceiver(messageObjectId);

                if(pMessageReceiver != NULL)
                {
                    delete pMessageReceiver;
                    pMessageReceiver = NULL;
                }
            }
            break;
        case MESSAGE_OBJECT_ENUM_AUDIO_DECODER:
            {
                IMessageReceiver* pMessageReceiver = CCMessageCenter::GetInstance()->GetMessageReceiver(messageObjectId);

                CCMessageCenter::GetInstance()->UnRegisterMessageReceiver(messageObjectId);

                if(pMessageReceiver != NULL)
                {
                    delete pMessageReceiver;
                    pMessageReceiver = NULL;
                }
            }
            break;
        case MESSAGE_OBJECT_ENUM_DATA_MANAGER:
            {
                IMessageReceiver* pMessageReceiver = CCMessageCenter::GetInstance()->GetMessageReceiver(messageObjectId);

                CCMessageCenter::GetInstance()->UnRegisterMessageReceiver(messageObjectId);

                if(pMessageReceiver != NULL)
                {
                    delete pMessageReceiver;
                    pMessageReceiver = NULL;
                }

            }
            break;
        default:
            printf("Unknow message object id\n");
            break;
    }
}


}
