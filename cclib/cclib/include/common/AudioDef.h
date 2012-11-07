#ifndef AUDIODEF_H
#define AUDIODEF_H

#include "Common.h"

namespace CCPlayer
{

typedef int CCChannels;
typedef int CCRates;

typedef enum
{
    unknow,
    u8,             // uint8_t
    s16,            // int16_t
    f32,            // float
    d64             // double
}CCType;

#define BASE_AUDIO_BUFFER_SIZE  4096
#define AUDIO_BUFFER_NUMBER     2

//define the limits of audio
#define MAX_AUDIO_FRAME_QUEUE_SIZE  4
#define MAX_AUDIO_PACKET_QUEUE_SIZE 4

class AudioFrame
{
public:
    AudioFrame(const char* pData, int dataSize)
    {
        m_dataSize = dataSize;

        assert(m_dataSize != 0);

        m_pData = new char[m_dataSize];
        memset(m_pData, 0, m_dataSize);
        memcpy(m_pData, pData, m_dataSize);
    }

    AudioFrame(const AudioFrame& audioFrame)
    {
        m_dataSize = audioFrame.m_dataSize;

        assert(m_dataSize != 0);

        m_pData = new char[m_dataSize];
        memset(m_pData, 0, m_dataSize);
        memcpy(m_pData, audioFrame.m_pData, m_dataSize);
    }

    const AudioFrame& operator=(const AudioFrame& audioFrame)
    {
        if(m_pData != NULL)
        {
            delete m_pData;
            m_pData = NULL;
        }

        m_dataSize = audioFrame.m_dataSize;

        assert(m_dataSize != 0);

        m_pData = new char[m_dataSize];
        memset(m_pData, 0, m_dataSize);
        memcpy(m_pData, audioFrame.m_pData, m_dataSize);

        return *this;
    }

    ~AudioFrame()
    {
        if(m_pData != NULL)
        {
            delete m_pData;
            m_pData = NULL;
        }
    }

public:
    int GetFrameSize()
    {
        return m_dataSize;
    }

    char* GetFrameData()
    {
        return m_pData;
    }

private:
    int m_dataSize;
    char* m_pData;
};

}

#endif
