#ifndef MESSAGECENTER_H
#define MESSAGECENTER_H

#include "Common.h"
#include "SpinLock.h"
#include "RWLock.h"
#include "Thread.h"
#include "Event.h"
#include "SmartPtr.h"
#include "FrequencyWorker.h"

namespace CCPlayer
{

class IMessageReceiver;

class CCMessageCenter: public CCThread, public CCFrequencyWorker
{
private:
    CCMessageCenter()
    {
    }
    CCMessageCenter(const CCMessageCenter& messageCenter)
    {
    }

public:
    virtual ~CCMessageCenter()
    {
    }
public:
    void InitMessageCenter();
    void RegisterMessageReceiver(MessageObjectId receiverObjectId, IMessageReceiver* pMessageReceiver);
    void UnRegisterMessageReceiver(MessageObjectId receiverObjectId);
    IMessageReceiver* GetMessageReceiver(MessageObjectId messageObjectId);

public:
    void Run();

private:
    std::map<MessageObjectId, IMessageReceiver*> m_mapMessageReceiver;
    CCRWLock m_rwLockMessageReceiver;

private:
    std::queue<SmartPtr<Event> > m_messageQueue;
    CCSpinLock m_spinLockMessageQueue;
    void PushBackMessage(MessageObjectId messageSender,
                         MessageObjectId messageReceiver,
                         MessageType msg,
                         Any anyParam);
    bool PopFrontMessage(SmartPtr<Event>& rSmtEvent);
    void NotifyToAllMessageObject(const SmartPtr<Event>& rSmtEvent);

public:
    static void PostMessage(MessageObjectId messageSender,
                            MessageObjectId messageReceiver,
                            MessageType msg,
                            Any anyParam);

public:
    static CCMessageCenter* GetInstance();
    static void DestoryInstance();
    
private:
    static CCSpinLock m_spinLockInstance;
    static CCMessageCenter* m_pSharedInstance;
};

}

#endif // MESSAGECENTER_H
