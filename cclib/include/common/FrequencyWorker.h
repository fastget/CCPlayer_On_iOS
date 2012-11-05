#ifndef FREQUENCYWORKER_H
#define FREQUENCYWORKER_H

#include <pthread.h>
#include <semaphore.h>

namespace CCPlayer
{

class CCFrequencyWorker
{
public:
    CCFrequencyWorker();
    virtual ~CCFrequencyWorker();

public:
    void Post();
    void Wait();

private:
    sem_t m_semaphore;
};

}
#endif // FREQUENCYWORKER_H
