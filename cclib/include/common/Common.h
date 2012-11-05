#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <limits>
#include <cassert>
#include <ctime>
#include <algorithm>

#include <unistd.h>
typedef unsigned int UIObjectHandle;

#define SUCCESS     0
#define FAILURE     -1

extern "C"
{
#include "libavutil/mem.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

namespace CCPlayer
{

enum MessageObjectId
{
    MESSAGE_OJBECT_ENUM_ALL,
    MESSAGE_OBJECT_ENUM_VIDEO_RENDER,
    MESSAGE_OBJECT_ENUM_AUDIO_RENDER,
    MESSAGE_OBJECT_ENUM_VIDEO_DECODER,
    MESSAGE_OBJECT_ENUM_AUDIO_DECODER,
    MESSAGE_OBJECT_ENUM_DATA_MANAGER,
    MESSAGE_OBJECT_ENUM_PLAYER,
    MESSAGE_OBJECT_ENUM_CLIENT,
    MESSAGE_OBJECT_MAX
};

class CCPacket
{
public:
    CCPacket()
    {
        //av_init_packet(&packet);
        m_pPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
        av_init_packet(m_pPacket);
    }
    ~CCPacket()
    {
        av_free_packet(m_pPacket);
        av_free(m_pPacket);
    }

public:
    AVPacket* GetPacketPointer()
    {
        return m_pPacket;
    }

    AVPacket GetPacket()
    {
        return *m_pPacket;
    }

private:
    AVPacket* m_pPacket;
};

}

#endif
