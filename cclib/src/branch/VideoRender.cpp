#include "VideoRender.h"
#include "MessageCenter.h"
#include "GLWrapper.h"
#include "SystemClock.h"

namespace CCPlayer
{

enum VideoRenderStatus
{
    VIDEO_RENDER_STATUS_ENUM_UNKNOW,
    VIDEO_RENDER_STATUS_ENUM_INITTED,
    VIDEO_RENDER_STATUS_ENUM_UPDATING,
    VIDEO_RENDER_STATUS_ENUM_SLEEPING,
    VIDEO_RENDER_STATUS_ENUM_DEADED,
    VIDEO_RENDER_STATUS_ENUM_MAX
};

CCVideoRender::CCVideoRender()
{
}

CCVideoRender::~CCVideoRender()
{
}

void CCVideoRender::PostMessage(MessageObjectId messageSender,
                            MessageObjectId messageReceiver,
                            MessageType msg,
                            Any anyParam)
{
    CCMessageCenter::GetInstance()->PostMessage(messageSender, messageReceiver, msg, anyParam);
}

void CCVideoRender::ReceiverMessage(const SmartPtr<Event>& rSmtEvent)
{
    m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    m_spinLockMessageQueue.UnLock();
}

bool CCVideoRender::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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

void CCVideoRender::Run()
{
    CCSystemClock::GetInstance()->RegisterSystemAlarm(this);

    CCGLWrapper glWrapper;
    int imgWidth = 0;
    int imgHeight = 0;

    VideoRenderStatus status
                        = VIDEO_RENDER_STATUS_ENUM_UNKNOW;
    bool bDataManagerEof = false;

    while(m_bRunning)
    {
        SmartPtr<Event> event;
        if(PopFrontMessage(event))
        {
            switch(event.GetPtr()->type)
            {
                case MESSAGE_TYPE_ENUM_INIT_GLRENDER_OBJECT:
                {
                    IGLView* pGLRenderView =
                                    any_cast<IGLView*>(event.GetPtr()->anyParams);

                    glWrapper.SetIGLRenderView(pGLRenderView);

                    status = VIDEO_RENDER_STATUS_ENUM_INITTED;
                }
                break;
                case MESSAGE_TYPE_ENUM_GET_VIDEO_INFORMATION:
                {
                    std::vector<Any> videoInformartion
                                        =any_cast<std::vector<Any> >(event.GetPtr()->anyParams);
                    imgWidth = any_cast<int>(videoInformartion[0]);
                    imgHeight = any_cast<int>(videoInformartion[1]);
                }
                break;
                case  MESSAGE_TYPE_ENUM_GET_VIDEO_FRAME:
                {
                    SmartPtr<VideoFrame> shrdVideoFrame
                                            = any_cast<SmartPtr<VideoFrame> >(event.GetPtr()->anyParams);
                    m_videoFrameQueue.push(shrdVideoFrame);
                }
                break;
                case MESSAGE_TYPE_ENUM_DATA_MANAGER_EOF:
                {
                    bDataManagerEof = true;
                }
                break;
                case MESSAGE_TYPE_ENUM_VIDEO_PAUSE:
                {
                    status = VIDEO_RENDER_STATUS_ENUM_SLEEPING;
                }
                break;
                case MESSAGE_TYPE_ENUM_VIDEO_CONTINUE:
                {
                    status = VIDEO_RENDER_STATUS_ENUM_INITTED;
                }
                break;
                case MESSAGE_TYPE_ENUM_CLIENT_STOP:
                {
                    status = VIDEO_RENDER_STATUS_ENUM_DEADED;
                }
                break;
            } // end of case event
        } // end of get a message

        switch(status)
        {
            case VIDEO_RENDER_STATUS_ENUM_INITTED:
            {
                //std::cout << "Video Render are working" << std::endl;

                if(!m_videoFrameQueue.empty())
                {
                    
                    //static int videoFrame = 0;
                    //printf("vide frame count%d\n", ++videoFrame);
                    
                    SmartPtr<VideoFrame> shrdVideoFrame =
                                            m_videoFrameQueue.front();

                    // div it with 1000.0 to prevent the number is to large
                    double currTime = av_gettime() / 1000.0;
                    //std::cout << " the show time is:" << (int64_t)shrdVideoFrame.GetPtr()->GetShowTime() << std::endl;
                    //std::cout << " the curr time is:=====" << (int64_t)currTime << std::endl;

                    if(shrdVideoFrame.GetPtr()->GetShowTime() < currTime)
                    {
                        //std::cout << " the show time is:" << (int64_t)shrdVideoFrame.GetPtr()->GetShowTime() << std::endl;
                        //std::cout << " the curr time is:=====" << (int64_t)currTime << std::endl;

                        m_videoFrameQueue.pop();
                        
                        glWrapper.DrawFrame(shrdVideoFrame.GetPtr(), imgWidth, imgHeight);
                                       
                        PostMessage(MESSAGE_OBJECT_ENUM_VIDEO_RENDER,
                                    MESSAGE_OBJECT_ENUM_VIDEO_DECODER,
                                    MESSAGE_TYPE_ENUM_VIDEO_RENDER_A_FRAME,
                                    Any());
                    } // end of if get show time
                }else if(bDataManagerEof)
                {
                    PostMessage(MESSAGE_OBJECT_ENUM_VIDEO_RENDER,
                                MESSAGE_OBJECT_ENUM_PLAYER,
                                MESSAGE_TYPE_ENUM_VIDEO_DEADED,
                                Any());
                    m_bRunning = false;
                    continue;
                }

                CCFrequencyWorker::Wait();
            }
            break;
            case VIDEO_RENDER_STATUS_ENUM_UPDATING:
            {
                //pGLRenderObject->DrawFrame();
            }
            break;
            case VIDEO_RENDER_STATUS_ENUM_SLEEPING:
            {
                usleep(100 * 1000);
                //Sleep(100);
            }
            break;
            case VIDEO_RENDER_STATUS_ENUM_DEADED:
            {
                m_bRunning = false;
                continue;
            }
            break;
        } // end of the render status
    }

    std::cout << "The vide render is deaded" << std::endl;

    CCSystemClock::GetInstance()->UnRegisterSystemAlarm(this);
}

}
