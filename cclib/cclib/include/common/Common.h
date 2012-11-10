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

//just for usleep
#include <unistd.h>

#define SUCCESS     0
#define FAILURE     -1

extern "C"
{
#include "libavutil/mem.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>

#include "AudioDef.h"
#include "VideoDef.h"
#include "SmartPtr.h"

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
        av_init_packet(&m_pPacket);
    }
    ~CCPacket()
    {
        av_free_packet(&m_pPacket);
    }

public:
    AVPacket* GetPacketPointer()
    {
        return &m_pPacket;
    }

    AVPacket GetPacket()
    {
        return m_pPacket;
    }

private:
    AVPacket m_pPacket;
};
    
//all the data field are public

class IPlayerDelegate;
class CCGLWrapper;
class CCALWrapper;
    
class CCPlayerContext
{
public:
    CCPlayerContext(CCALWrapper* pALWrapper);
    ~CCPlayerContext();
    
public:
    void FindContextInfomation(IPlayerDelegate* pIPlayerDelegate);
    void ReleaseContextInformation();
    
public:
    AVFormatContext *m_pAVFormatContext;
    
    int m_asIndex;
    ALint m_channels;
    ALint m_rates;
    //CCType m_type;
    ALenum m_audFormat;
    AVCodecContext* m_pAudioCodecCtx;
    AVRational m_audioTimeBase;
    AVFrame* m_pAudioDecodedFrame;
    std::queue<SmartPtr<CCPacket> > m_audioPacketQueue;
    std::queue<SmartPtr<AudioFrame> > m_audioFrameQueue;
    CCALWrapper* m_pALWrapper;
    
    int m_vsIndex;
    AVCodecContext* m_pVideoCodecCtx;
    SwsContext* m_pImageConvertCtx;
    AVRational m_videoTimeBase;
    AVFrame *m_pVideoDecodedFrame;
    AVPicture *m_pDecodedPicture;
    int m_imgWidth;
    int m_imgHeight;
	int m_imgBufferLen;
    std::queue<SmartPtr<CCPacket> > m_videoPacketQueue;
    std::queue<SmartPtr<VideoFrame> > m_videoFrameQueue;
    CCGLWrapper* m_pGLWrapper;
};

extern CCPlayerContext* g_pPlayerContext;
    
}

#endif
