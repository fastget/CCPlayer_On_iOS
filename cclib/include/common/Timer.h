#ifndef TIMER_H
#define TIMER_H

#include "Common.h"

namespace CCPlayer
{

class ITimerInterface
{
public:
    virtual void TimeElapsed(void* pUserData) = 0;
};

class CCTimer
{
public:
    CCTimer();
    virtual ~CCTimer();

public:
    bool SetTimer(int* timerId,
                  unsigned int timerInterval,
                  ITimerInterface* pCallTarget,
                  void* pUserData);
};

}

#endif // TIMER_H
