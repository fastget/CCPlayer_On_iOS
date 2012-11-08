#include "SystemClock.h"

namespace CCPlayer
{

CCSystemClock* CCSystemClock::m_pInstance = NULL;

CCSystemClock::CCSystemClock()
{
}

CCSystemClock::~CCSystemClock()
{
    //dtor
}

CCSystemClock* CCSystemClock::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CCSystemClock();
    }

    return m_pInstance;
}
    
void CCSystemClock::DestoryInstance()
{
    if (m_pInstance != NULL)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
}

void CCSystemClock::SetRealPlayedTime(int64_t realPlayedTime)
{
    m_realPlayedTime = realPlayedTime;
}

int64_t CCSystemClock::GetRealPlayedTime()
{
    return m_realPlayedTime;
}
    
void CCSystemClock::SetVideoStartRender(bool bVideoStartRender)
{
    m_bVideoStartRender = bVideoStartRender;
}
    
bool CCSystemClock::GetVideoStartRender()
{
    return m_bVideoStartRender;
}

}
