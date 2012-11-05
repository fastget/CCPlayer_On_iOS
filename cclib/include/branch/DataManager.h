#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include "Thread.h"
#include "Common.h"
#include "SpinLock.h"
#include "IMessageReceiver.h"
#include "FrequencyWorker.h"

namespace CCPlayer
{

class CCDataManager : public CCThread, public IMessageReceiver, public CCFrequencyWorker
{
public:
    CCDataManager();
    virtual ~CCDataManager();

public:
    void PostMessage(MessageObjectId messageSender,
                            MessageObjectId messageReceiver,
                            MessageType msg,
                            Any anyParam);
    void ReceiverMessage(const SmartPtr<Event>& rSmtEvent);
    bool PopFrontMessage(SmartPtr<Event>& rSmtEvent);

public:
    virtual void Run();

private:
    int OpenFile(const std::string& mediaUrl,
                 AVFormatContext** ppFormatCtx,
                 int* pASIndex,
                 int* pVSIndex);

private:
    std::queue<SmartPtr<Event> > m_messageQueue;
    CCSpinLock m_spinLockMessageQueue;
};

}

#endif // DATAMANAGER_H
