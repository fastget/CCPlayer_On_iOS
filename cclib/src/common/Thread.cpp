#include "Thread.h"

namespace CCPlayer
{

CCThread::CCThread()
{
    m_bRunning = false;
}

CCThread::~CCThread()
{
    m_bRunning = true;
}

int CCThread::Launch()
{
    int Error = pthread_create(&m_threadId,
                               NULL,
                               CCThread::ThreadFunc,
                               this);
    return Error;
}

//implement the Call back function
void* CCThread::ThreadFunc(void *pUserData)
{
    CCThread* pThreadObj = reinterpret_cast<CCThread*>(pUserData);
    if(pThreadObj != NULL)
    {
        // this flag represents the thread whether is running mode or not
        pThreadObj->m_bRunning = true;
        pThreadObj->Run();
    }

    return NULL;
}

} // end namespace CCPlayer
