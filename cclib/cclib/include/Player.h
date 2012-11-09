#ifndef CCPLAER_H
#define CCPLAER_H

#include "Common.h"
#include "Thread.h"
#include "IMessageReceiver.h"
#include "Event.h"
#include "SmartPtr.h"
#include "SpinLock.h"
//#include "FrequencyWorker.h"

namespace CCPlayer
{

class ICommandResponse;
class IGLView;

class CCPlayer : public CCThread, public IMessageReceiver//, public CCFrequencyWorker
{
public:
    CCPlayer(ICommandResponse* pICommandResponse);
    virtual ~CCPlayer();

public:
    void Open(const std::string& loadParams);
    void Pause();
    void Continue();
    void Stop();

public:
    void SetGLRenderView(IGLView* pIGLRenderView);
    void SetVolume(float volume);
    
public:
    //get total display time
    int GetTotalDurationBySecond(int64_t* pTotalDuration);
    int GetCurrentPostionBySecond(int64_t* pCurrPostion);

public:
    void PostMessage(MessageObjectId messageSender,
                            MessageObjectId messageReceiver,
                            MessageType msg,
                            Any anyParam);
    void ReceiverMessage(const SmartPtr<Event>& rSmtEvent);
    bool PopFrontMessage(SmartPtr<Event>& rSmtEvent);

private:
    int FindAudioDecoderContext(AVCodecContext** ppASDecoderCtx, AVFormatContext* pFormatCtx, int asIndex);

public:
    virtual void Run();
    
private:
    void DestructPlayerSystem();

private:
    std::queue<SmartPtr<Event> > m_messageQueue;
    CCSpinLock m_spinLockMessageQueue;

private:
    AVFormatContext* m_pAVFormatCtx;

private:
    ICommandResponse* m_pCommandResponseObject;
};

}

#endif // CCPlayer_H
