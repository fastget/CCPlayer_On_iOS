#ifndef RWLOCK_H
#define RWLOCK_H

#include <pthread.h>

namespace CCPlayer
{
class CCRWLock
{
public:
    CCRWLock()
    {
        pthread_rwlock_init(&m_rwLock, NULL);
    }
    ~CCRWLock()
    {
        pthread_rwlock_destroy(&m_rwLock);
    }
public:
    int ReadLock()
    {
        return pthread_rwlock_rdlock(&m_rwLock);
    }

    int WriteLock()
    {
        return pthread_rwlock_wrlock(&m_rwLock);
    }

    int UnLock()
    {
        return pthread_rwlock_unlock(&m_rwLock);
    }
private:
    pthread_rwlock_t m_rwLock;
};
}

#endif
