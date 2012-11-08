#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <pthread.h>

namespace CCPlayer
{

//spin lock is supported on iOS , so we just do it with mutexs
class CCSpinLock
{
public:
    CCSpinLock()
    {
        pthread_mutex_init(&m_mutex, NULL);
    };
    ~CCSpinLock()
    {
        pthread_mutex_destroy(&m_mutex);
    }

public:
    int Lock()
    {
        while(pthread_mutex_trylock(&m_mutex) == EBUSY)
        {
            usleep(10);
        }
        return 0;
    }

    int UnLock()
    {
        pthread_mutex_unlock(&m_mutex);
        return 0;
    }

private:
    pthread_mutex_t m_mutex;
};

}

#endif
