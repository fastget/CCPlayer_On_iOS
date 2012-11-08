#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "Common.h"

namespace CCPlayer
{

class CCFrequencyWorker;

class CCSystemClock
{
public:
    CCSystemClock();
    virtual ~CCSystemClock();

public:
    void SetRealPlayedTime(int64_t realPlayedTime);
    int64_t GetRealPlayedTime();
    void SetVideoStartRender(bool bVideoStartRender);
    bool GetVideoStartRender();

private:
    //the actully start time for the begin;
    int64_t m_realPlayedTime;
    bool m_bVideoStartRender;

public:
    static CCSystemClock* GetInstance();
    static void DestoryInstance();

private:
    static CCSystemClock* m_pInstance;
};

}

#endif // TIMEMANAGER_H
