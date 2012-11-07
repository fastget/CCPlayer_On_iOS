#include "SystemClock.h"
#include "FrequencyWorker.h"

namespace CCPlayer
{

CCMutex CCSystemClock::m_instanceMutex;
CCSystemClock* CCSystemClock::m_pInstance = NULL;

CCSystemClock::CCSystemClock()
{
    //ctor
    //every 15 ms tell the worker
    int timeId = 0;
    m_frequencyTimer.SetTimer(&timeId, 2500, this, NULL);
}

CCSystemClock::~CCSystemClock()
{
    //dtor
}

CCSystemClock* CCSystemClock::GetInstance()
{
    m_instanceMutex.Lock();
    if(m_pInstance == NULL)
    {
        m_pInstance = new CCSystemClock();
    }
    m_instanceMutex.UnLock();

    return m_pInstance;
}


void CCSystemClock::RegisterSystemAlarm(CCFrequencyWorker* pFrequencyWorker)
{
    m_frequencyWorkerMutex.Lock();
    m_frequencyWorkerList.push_back(pFrequencyWorker);
    m_frequencyWorkerMutex.UnLock();
}

void CCSystemClock::UnRegisterSystemAlarm(CCFrequencyWorker* pFrequencyWorker)
{
    m_frequencyWorkerMutex.Lock();

    std::vector<CCFrequencyWorker*>::iterator pos = m_frequencyWorkerList.begin();
    while(pos != m_frequencyWorkerList.end())
    {
        if(*pos == pFrequencyWorker)
        {
            break;
        }
        pos ++;
    }

    m_frequencyWorkerList.erase(pos);

    m_frequencyWorkerMutex.UnLock();
}

void CCSystemClock::SetRealStartTime(int64_t realStartTime)
{
    m_realStartTime = realStartTime;
}

int64_t CCSystemClock::GetRealStartTime()
{
    return m_realStartTime;
}

void CCSystemClock::TimeElapsed(void* pUserData)
{
    m_frequencyWorkerMutex.Lock();

//    std::vector<CCFrequencyWorker*>::iterator pos = m_frequencyWorkerList.begin();
//    while(pos != m_frequencyWorkerList.end())
//    {
//        //Wakeup the thread
//        (*pos)->Post();
//        pos ++;
//    }

    m_frequencyWorkerMutex.UnLock();
}

}
