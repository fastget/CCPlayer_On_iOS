#ifndef AUDIORENDER_H
#define AUDIORENDER_H

#include "Thread.h"
#include "Common.h"
#include "SpinLock.h"
#include "ALWrapper.h"
#include "AudioDef.h"
#include "IMessageReceiver.h"
#include "FrequencyWorker.h"

namespace CCPlayer
{

class CCAudioRender: public CCThread, public IMessageReceiver, public CCFrequencyWorker
{
public:
    CCAudioRender();
    virtual ~CCAudioRender();

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
    ALenum GetAudioFormat(CCChannels channels, CCType type);

private:
    std::queue<SmartPtr<AudioFrame> > m_audioFrameQueue;

private:
    std::queue<SmartPtr<Event> > m_messageQueue;
    CCSpinLock m_spinLockMessageQueue;
};

}

#endif // AUDIORENDER_H
