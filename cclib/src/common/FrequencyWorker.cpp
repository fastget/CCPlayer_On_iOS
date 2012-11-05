#include "FrequencyWorker.h"

namespace CCPlayer
{

CCFrequencyWorker::CCFrequencyWorker()
{
    sem_init(&m_semaphore, 0, 1);
}

CCFrequencyWorker::~CCFrequencyWorker()
{
    sem_destroy(&m_semaphore);
}

void CCFrequencyWorker::Post()
{
    sem_post(&m_semaphore);
}

void CCFrequencyWorker::Wait()
{
    sem_wait(&m_semaphore);
}

}
