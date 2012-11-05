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
    DATA_MANAGER_STATUS_ENUM_DEADING,
    DATA_MANAGER_STATUS_ENUM_DEADED,
    DATA_MANAGER_STATUS_ENUM_MAX
};

#define DECODERS_STATUS_ENUM_NONE_READY     0
#define DECODERS_STATUS_ENUM_AUDIO_READY    1
#define DECODERS_STATUS_ENUM_VIDEO_READY    2
#define DECODERS_STATUS_ENUM_ALL_READY      3

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

CCDataManager::CCDataManager()
{
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
    m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    m_spinLockMessageQueue.UnLock();
}

bool CCDataManager::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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

void CCDataManager::Run()
{
    //this is register for all ffmpeg action
    av_register_all();

    //this is for mutli-thread ffmpeg working
    if(av_lockmgr_register(ffmpegLockManager))
    {
        // TODO Failure av_lockmgr_register
    }

    CCSystemClock::GetInstance()->RegisterSystemAlarm(this);

    std::string mediaUrl;
    AVFormatContext *pAVFormatContext = NULL;
    int asIndex = -1;
    int vsIndex = -1;
    DataManagerStatus status = DATA_MANAGER_STATUS_ENUM_INIT;
    unsigned int decodersStatus = DECODERS_STATUS_ENUM_NONE_READY;

    int audioPacketQueueSize = 0;
    int videoPacketQueueSize = 0;

    while(m_bRunning)
    {
        SmartPtr<Event> event;
        if(PopFrontMessage(event))
        {
            //std::cout << "event.GetPtr()->type=" << event.GetPtr()->type << std::endl;

            switch(event.GetPtr()->type)
            {
                case MESSAGE_TYPE_ENUM_OPEN_FILE:
                {
                    mediaUrl = any_cast<std::string>(event.GetPtr()->anyParams);

                    int ret = OpenFile(mediaUrl,
                                       &pAVFormatContext,
                                       &asIndex,
                                       &vsIndex);

                    std::vector<Any> openedParams;
                    openedParams.push_back(Any(ret));
                    openedParams.push_back(Any(pAVFormatContext));
                    openedParams.push_back(Any(asIndex));
                    openedParams.push_back(Any(vsIndex));

                    PostMessage(MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                MESSAGE_OBJECT_ENUM_PLAYER,
                                MESSAGE_TYPE_ENUM_OPENED_FILE,
                                openedParams);
                }
                break;
                case MESSAGE_TYPE_ENUM_AUDIO_DECODER_READY:
                {
                    decodersStatus += DECODERS_STATUS_ENUM_AUDIO_READY;

                    if(decodersStatus == DECODERS_STATUS_ENUM_ALL_READY)
                    {
                        status = DATA_MANAGER_STATUS_ENUM_WORKING;
                    }
                }
                break;
                case MESSAGE_TYPE_ENUM_VIDEO_DECODER_READY:
                {
                    decodersStatus += DECODERS_STATUS_ENUM_VIDEO_READY;

                    if(decodersStatus == DECODERS_STATUS_ENUM_ALL_READY)
                    {
                        status = DATA_MANAGER_STATUS_ENUM_WORKING;
                    }
                }
                break;
                case MESSAGE_TYPE_ENUM_AUDIO_DEOCDER_A_PACKET:
                {
                    //status = DATA_MANAGER_STATUS_ENUM_SLEEPING;
                    //we decoded a audio packet
                    audioPacketQueueSize --;
                }
                break;
                case MESSAGE_TYPE_ENUM_VIDEO_DECODER_A_PACKET:
                {
                    //status = DATA_MANAGER_STATUS_ENUM_SLEEPING;
                    //we decoded a video packet
                    videoPacketQueueSize --;
                }
                break;
                case MESSAGE_TYPE_ENUM_CLIENT_STOP:
                {
                    status = DATA_MANAGER_STATUS_ENUM_DEADED;
                }
                break;
                default:
                    std::cout << "Unknow Data Manager Message" << std::endl;
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
                if(audioPacketQueueSize < MAX_AUDIO_PACKET_QUEUE_SIZE
                   || videoPacketQueueSize < MAX_VIDEO_PACKET_QUEUE_SIZE)
                {
                    SmartPtr<CCPacket> packet(new CCPacket());
                    if(av_read_frame(pAVFormatContext, packet.GetPtr()->GetPacketPointer()) < 0)
                    {
                        PostMessage(MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                    MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                                    MESSAGE_TYPE_ENUM_DATA_MANAGER_EOF,
                                    Any());

                        PostMessage(MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                    MESSAGE_OBJECT_ENUM_VIDEO_DECODER,
                                    MESSAGE_TYPE_ENUM_DATA_MANAGER_EOF,
                                    Any());
                        //std::cout << "endend==========================================endend" << std::endl;
                        m_bRunning = false;
                        continue;
                    }

                    //CCFrequencyWorker::Wait();
                    //Sleep(2);

                    if(packet.GetPtr()->GetPacketPointer()->stream_index
                                == asIndex)
                    {
                        PostMessage(MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                    MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
                                    MESSAGE_TYPE_ENUM_GET_AUDIO_PACKET,
                                    Any(packet));
                        //we got a audio packet
                        audioPacketQueueSize ++;
                    }else if(packet.GetPtr()->GetPacketPointer()->stream_index
                                == vsIndex)
                    {
                        PostMessage(MESSAGE_OBJECT_ENUM_DATA_MANAGER,
                                    MESSAGE_OBJECT_ENUM_VIDEO_DECODER,
                                    MESSAGE_TYPE_ENUM_GET_VIDEO_PACKET,
                                    Any(packet));

                        //we got a video packet
                        videoPacketQueueSize ++;
                    }
                }// if audio packet enough or video packet enough
                else
                {
                    usleep(10 * 1000);
                    //Sleep(10);
                }
            }
            break;
            case DATA_MANAGER_STATUS_ENUM_SLEEPING:
            {
                usleep(50 * 1000);
                //usleep(10);
                //Sleep(50);
                //after we have a reset , we should working
                //status = DATA_MANAGER_STATUS_ENUM_WORKING;
            }
            break;
            case DATA_MANAGER_STATUS_ENUM_DEADING:
            {
            }
            break;
            case DATA_MANAGER_STATUS_ENUM_DEADED:
            {
                m_bRunning = false;
                continue;
            }
            break;
        } // end switch
    }

    CCSystemClock::GetInstance()->UnRegisterSystemAlarm(this);
}

int CCDataManager::OpenFile(const std::string& mediaUrl,
                 AVFormatContext** ppFormatCtx,
                 int* pASIndex,
                 int* pVSIndex)
{
    *pASIndex = -1;
    *pVSIndex = -1;
    if(avformat_open_input(ppFormatCtx,mediaUrl.c_str(), NULL, NULL) != 0)
	{
		std::cout << "can not open media file" << std::endl;
		return FAILURE;
	}else
	{
		std::cout << "open media file" << std::endl;
	}

	if(avformat_find_stream_info(*ppFormatCtx, NULL) < 0)
	{
		std::cout << "can not retrieve file info" << std::endl;
		return FAILURE;
	}else
	{
		std::cout << "retrieve file info" << std::endl;
	}

	av_dump_format(*ppFormatCtx,
			0, mediaUrl.c_str(), 0);

	for(unsigned i=0; i<(*ppFormatCtx)->nb_streams; i++)
	{
		if(*pASIndex != -1
				&& *pVSIndex != -1)
		{
			//We have find the stream
			break;
		}

        if(*pASIndex == -1
                && (*ppFormatCtx)->streams[i]->codec->codec_type
					== AVMEDIA_TYPE_AUDIO)
		{
		    *pASIndex = i;
        }

		if(*pVSIndex == -1
				&& (*ppFormatCtx)->streams[i]->codec->codec_type
					== AVMEDIA_TYPE_VIDEO)
		{
		    *pVSIndex = i;
        }

	}

    int64_t realStartTime = av_gettime() / 1000;
    CCSystemClock::GetInstance()->SetRealStartTime(realStartTime);

    //std::cout << "The total time is : " << (*ppFormatCtx)->duration / 1000000 << std::endl;
	//(*ppFormatCtx)->start_time = av_gettime() / 1000.0;
	//std::cout << "The start time is :" << (*ppFormatCtx)->start_time << std::endl;
	//std::cout << "The total file size is : " << avio_size((*ppFormatCtx)->pb) << std::endl;
    //std::cout << "The Bit_rate is : " << avio_size((*ppFormatCtx)->pb) / ((*ppFormatCtx)->duration / 1000000) << std::endl;
    //std::cout << "The real bit rate is: " << (*ppFormatCtx)->bit_rate / 8 << std::endl;

	//avio_seek((*ppFormatCtx)->pb, 0, SEEK_END);
	//std::cout << "The total file size is : " << avio_tell((*ppFormatCtx)->pb) << std::endl;

	return 0;
}

}
