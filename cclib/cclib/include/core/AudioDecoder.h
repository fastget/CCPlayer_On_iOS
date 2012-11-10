#ifndef AUDIODECODER_H
#define AUDIODECODER_H

#include "Thread.h"
#include "Common.h"
#include "SpinLock.h"
#include "AudioDef.h"
#include "IMessageReceiver.h"

namespace CCPlayer
{

class CCAudioDecoder : public CCThread, public IMessageReceiver
{
public:
    CCAudioDecoder();
    virtual ~CCAudioDecoder();

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
    std::queue<SmartPtr<Event> > m_messageQueue;
    //CCSpinLock m_spinLockMessageQueue;

};

}

#endif // AUDIODECODER_H
