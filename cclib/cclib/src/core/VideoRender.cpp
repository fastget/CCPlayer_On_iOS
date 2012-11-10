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
    printf("CCAudioDecoder address %p", this);
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
    //m_spinLockMessageQueue.Lock();
    m_messageQueue.push(rSmtEvent);
    //m_spinLockMessageQueue.UnLock();
}

bool CCVideoRender::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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

void CCVideoRender::Run()
{
    int imgWidth = 0;
    int imgHeight = 0;

    VideoRenderStatus status
                        = VIDEO_RENDER_STATUS_ENUM_INITTED;
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
              if(!g_pPlayerContext->m_videoFrameQueue.empty())
                {
                    SmartPtr<VideoFrame> shrdVideoFrame =
                                            g_pPlayerContext->m_videoFrameQueue.front();

                    if (CCSystemClock::GetInstance()->GetVideoStartRender()
                        && shrdVideoFrame.GetPtr()->GetShowTime() < CCSystemClock::GetInstance()->GetRealPlayedTime())
                    {
                        g_pPlayerContext->m_videoFrameQueue.pop();
                             
                        g_pPlayerContext->m_pGLWrapper->DrawFrame(shrdVideoFrame.GetPtr(), imgWidth, imgHeight);

                    } // end of if get show time
                }else if(bDataManagerEof)
                {
                    status = VIDEO_RENDER_STATUS_ENUM_DEADED;
                }
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
                PostMessage(MESSAGE_OBJECT_ENUM_VIDEO_RENDER,
                            MESSAGE_OBJECT_ENUM_PLAYER,
                            MESSAGE_TYPE_ENUM_VIDEO_RENDER_DEADED,
                            Any());
                m_bRunning = false;
                continue;
            }
            break;
        } // end of the render status
    }

    g_pPlayerContext->m_pGLWrapper->ClearGLRenderView();
    std::cout << "The vide render is deaded" << std::endl;
}

}
