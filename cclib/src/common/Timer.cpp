#include "Timer.h"
#include <dispatch/dispatch.h> 

namespace CCPlayer
{

CCTimer::CCTimer()
{
}

CCTimer::~CCTimer()
{
}

bool CCTimer::SetTimer(int* timerId,
                       unsigned int timerInterval,
                       ITimerInterface* pCallTarget,
                       void* pUserData)
{
    dispatch_source_t timer;
    
    timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0,
                                   dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0));
    dispatch_source_set_timer(timer, DISPATCH_TIME_NOW, 1ull * timerInterval * NSEC_PER_MSEC, 0);
    dispatch_source_set_event_handler(timer, ^{
        pCallTarget->TimeElapsed(pUserData);
    });  
    dispatch_resume(timer);
    
    return true;
}

}
