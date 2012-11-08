#include "MessageCenter.h"
#include "IMessageReceiver.h"
#include "SmartPtr.h"
#include "SystemClock.h"

namespace CCPlayer
{

CCMessageCenter* CCMessageCenter::m_pSharedInstance = NULL;

CCMessageCenter* CCMessageCenter::GetInstance()
{
    if(m_pSharedInstance == NULL)
    {
        m_pSharedInstance = new CCMessageCenter();
    }
    return m_pSharedInstance;
}

void CCMessageCenter::DestoryInstance()
{
    m_pSharedInstance->m_bRunning = false;
    
    //just wait the message center deaded
    pthread_join(m_pSharedInstance->m_threadId, NULL);
    
    if (m_pSharedInstance != NULL)
    {
        delete m_pSharedInstance;
        m_pSharedInstance = NULL;
    }
}
    
void CCMessageCenter::RegisterMessageReceiver(MessageObjectId receiverObjectId, IMessageReceiver* pMessageReceiver)
{
    m_rwLockMessageReceiver.WriteLock();
    m_mapMessageReceiver.insert(std::map<MessageObjectId, IMessageReceiver*>::value_type(receiverObjectId, pMessageReceiver));
    m_rwLockMessageReceiver.UnLock();
}

void CCMessageCenter::UnRegisterMessageReceiver(MessageObjectId receiverObjectId)
{
    m_rwLockMessageReceiver.WriteLock();
    m_mapMessageReceiver.erase(receiverObjectId);
    m_rwLockMessageReceiver.UnLock();
}

IMessageReceiver* CCMessageCenter::GetMessageReceiver(MessageObjectId messageObjectId)
{
    IMessageReceiver* pMessageObject = NULL;
    m_rwLockMessageReceiver.ReadLock();
    std::map<MessageObjectId, IMessageReceiver*>::const_iterator pos = m_mapMessageReceiver.find(messageObjectId);
    if(pos != m_mapMessageReceiver.end())
    {
        pMessageObject = pos->second;
    }
    m_rwLockMessageReceiver.UnLock();
    return pMessageObject;
}

void CCMessageCenter::PostMessage(MessageObjectId messageSender,
                                  MessageObjectId messageReceiver,
                                  MessageType msg,
                                  Any anyParam)
{
    CCMessageCenter* pMessageCenter = CCMessageCenter::GetInstance();
    pMessageCenter->PushBackMessage(messageSender, messageReceiver, msg, anyParam);
}

void CCMessageCenter::PushBackMessage(MessageObjectId messageSender,
                         MessageObjectId messageReceiver,
                         MessageType msg,
                         Any anyParam)
{
    m_spinLockMessageQueue.Lock();
    Event event;
    event.pSendModule = GetMessageReceiver(messageSender);
    event.pReceiveModule = GetMessageReceiver(messageReceiver);
    event.type = msg;
    event.anyParams = anyParam;
    m_messageQueue.push(SmartPtr<Event>(new Event(event)));
    m_spinLockMessageQueue.UnLock();
}

bool CCMessageCenter::PopFrontMessage(SmartPtr<Event>& rSmtEvent)
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

void CCMessageCenter::NotifyToAllMessageObject(const SmartPtr<Event>& rSmtEvent)
{
    m_spinLockMessageQueue.Lock();

    std::map<MessageObjectId, IMessageReceiver*>::const_iterator cit
                                                    = m_mapMessageReceiver.begin();

    while(cit != m_mapMessageReceiver.end())
    {
        //发送除自己以外的所有消息对象。
        if (rSmtEvent.GetPtr()->pSendModule != cit->second)
        {
            SmartPtr<Event> shdEvent = rSmtEvent;
            rSmtEvent.GetPtr()->pReceiveModule = cit->second;
            cit->second->ReceiverMessage(rSmtEvent);
            
            printf("the receive id is : %d\n",cit->first);
        }
        cit ++;
    }

    m_spinLockMessageQueue.UnLock();
}

void CCMessageCenter::InitMessageCenter()
{
    m_pSharedInstance->Launch();
}

void CCMessageCenter::Run()
{
    //CCSystemClock::GetInstance()->RegisterSystemAlarm(this);

    while(m_bRunning)
    {
        SmartPtr<Event> event;
        if(PopFrontMessage(event))
        {
            if(event.GetPtr()->pReceiveModule != NULL)
            {
                event.GetPtr()->pReceiveModule->ReceiverMessage(event);
            }
            else
            {
                NotifyToAllMessageObject(event);
            }
        }else
        {
//            CCFrequencyWorker::Wait();
            usleep(10 * 1000);
        }
    }

    //CCSystemClock::GetInstance()->UnRegisterSystemAlarm(this);
}

}
