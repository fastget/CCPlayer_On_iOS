#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>

namespace CCPlayer
{
class CCMutex
{
public:
    CCMutex()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
    ~CCMutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }
public:
    int Lock()
    {
        return pthread_mutex_lock(&m_mutex);
    }
    int UnLock()
    {
        return pthread_mutex_unlock(&m_mutex);
    }
private:
    pthread_mutex_t m_mutex;
};

}

#endif
