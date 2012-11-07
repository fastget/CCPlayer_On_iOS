#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "Timer.h"
#include "Mutex.h"
#include "RWLock.h"

namespace CCPlayer
{

class CCFrequencyWorker;

class CCSystemClock : public ITimerInterface
{
public:
    CCSystemClock();
    virtual ~CCSystemClock();

public:
    void RegisterSystemAlarm(CCFrequencyWorker* pFrequencyWorker);
    void UnRegisterSystemAlarm(CCFrequencyWorker* pFrequencyWorker);

public:
    void SetRealStartTime(int64_t realStartTime);
    int64_t GetRealStartTime();

private:
    //the actully start time for the begin;
    int64_t m_realStartTime;

public:
    static CCSystemClock* GetInstance();

private:
    CCTimer m_frequencyTimer;
    CCRWLock m_timerRWLock;

public:
    virtual void TimeElapsed(void* pUserData);

private:
    std::vector<CCFrequencyWorker*> m_frequencyWorkerList;
    CCMutex m_frequencyWorkerMutex;

private:
    static CCMutex m_instanceMutex;
    static CCSystemClock* m_pInstance;
};

}

#endif // TIMEMANAGER_H
