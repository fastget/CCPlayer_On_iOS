//
//  Common.cpp
//  cclib
//
//  Created by maclion on 11/10/12.
//  Copyright (c) 2012 cc. All rights reserved.
//

#include "Common.h"
#include "GLWrapper.h"

namespace CCPlayer
{

CCPlayerContext::CCPlayerContext(CCALWrapper* pALWrapper)
:m_pAVFormatContext(NULL)
,m_pGLWrapper(NULL)
,m_pALWrapper(pALWrapper)
{
}
    
CCPlayerContext::~CCPlayerContext()
{
    if (m_pGLWrapper != NULL)
    {
        delete m_pGLWrapper;
        m_pGLWrapper = NULL;
    }
}

void CCPlayerContext::FindContextInfomation(IPlayerDelegate* pIPlayerDelegate)
{
    m_asIndex = -1;
    m_vsIndex = -1;
    
    for(unsigned i=0; i<m_pAVFormatContext->nb_streams; i++)
	{
		if(m_asIndex != -1
           && m_vsIndex != -1)
		{
			//We have find the stream
			break;
		}
        
        if(m_asIndex == -1
           && m_pAVFormatContext->streams[i]->codec->codec_type
           == AVMEDIA_TYPE_AUDIO)
		{
		    m_asIndex = i;
        }
        
		if(m_vsIndex == -1
           && m_pAVFormatContext->streams[i]->codec->codec_type
           == AVMEDIA_TYPE_VIDEO)
		{
		    m_vsIndex = i;
        }
	}
    
    //find the audio stream
    if (m_asIndex != -1)
    {
        m_pAudioCodecCtx = m_pAVFormatContext->streams[m_asIndex]->codec;
        m_audioTimeBase = m_pAVFormatContext->streams[m_asIndex]->time_base;
        
        
        AVCodec *pAVCodecAudio = avcodec_find_decoder(m_pAudioCodecCtx->codec_id);
        
        if(avcodec_open2(m_pAudioCodecCtx, pAVCodecAudio, NULL) != 0)
        {
            std::cout << "Can not open the audio information" << std::endl;
        }
        
        if (m_pAudioCodecCtx->channels < 1
            || m_pAudioCodecCtx->channels > 8
            || m_pAudioCodecCtx->channels == 3
            || m_pAudioCodecCtx->channels == 5)
        {
            std::cout << "Unsupported channels " << std::endl;
        }
        
        m_channels = m_pAudioCodecCtx->channels;
        m_rates = m_pAudioCodecCtx->sample_rate;
        if (m_pAudioCodecCtx->sample_fmt == AV_SAMPLE_FMT_U8)
        {
            if (m_channels == 1)
            {
                m_audFormat = AL_FORMAT_MONO8;
            }
            else if (m_channels == 2)
            {
                m_audFormat = AL_FORMAT_STEREO8;
            }
            else if (alIsExtensionPresent("AL_EXT_MCFORMATS"))
            {
                if (m_channels == 4)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_QUAD8");
                }
                else if (m_channels == 6)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_51CHN8");
                }
                else if (m_channels == 7)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_71CHN8");
                }
                else if (m_channels == 8)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_81CHN8");
                }
            }
        }
        else if (m_pAudioCodecCtx->sample_fmt == AV_SAMPLE_FMT_S16)
        {
            if (m_channels == 1)
            {
                m_audFormat = AL_FORMAT_MONO16;
            }
            else if (m_channels == 2)
            {
                m_audFormat = AL_FORMAT_STEREO16;
            }
            else if (alIsExtensionPresent("AL_EXT_MCFORMATS"))
            {
                if (m_channels == 4)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_QUAD16");
                }
                else if (m_channels == 6)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_51CHN16");
                }
                else if (m_channels == 7)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_61CHN16");
                }
                else if (m_channels == 8)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_71CHN16");
                }
            }
        }
        else if (m_pAudioCodecCtx->sample_fmt == AV_SAMPLE_FMT_FLT
                 || (m_pAudioCodecCtx->sample_fmt == AV_SAMPLE_FMT_S32
                     && sizeof(int32_t) == sizeof(float)))
        {
            if (alIsExtensionPresent("AL_EXT_float32"))
            {
                if (m_channels == 1)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_MONO_FLOAT32");
                }
                else if (m_channels == 2)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_STEREO_FLOAT32");
                }
                else if (alIsExtensionPresent("AL_EXT_MCFORMATS"))
                {
                    if (m_channels == 4)
                    {
                        m_audFormat = alGetEnumValue("AL_FORMAT_QUAD32");
                    }
                    else if (m_channels == 6)
                    {
                        m_audFormat = alGetEnumValue("AL_FORMAT_51CHN32");
                    }
                    else if (m_channels == 7)
                    {
                        m_audFormat = alGetEnumValue("AL_FORMAT_61CHN32");
                    }
                    else if (m_channels == 8)
                    {
                        m_audFormat = alGetEnumValue("AL_FORMAT_71CHN32");
                    }
                }
            }
        }
        else if (m_pAudioCodecCtx->sample_fmt == AV_SAMPLE_FMT_DBL)
        {
            if (alIsExtensionPresent("AL_EXT_double"))
            {
                if (m_channels == 1)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_MONO_DOUBLE_EXT");
                }
                else if (m_channels == 2)
                {
                    m_audFormat = alGetEnumValue("AL_FORMAT_STEREO_DOUBLE_EXT");
                }
            }
        }
        else
        {
            std::cout << "Unsupported the format " << std::endl;
        }
        
        m_pAudioDecodedFrame = avcodec_alloc_frame();
        
        //before we can change the value
        //m_pALWrapper = new CCALWrapper();
    }
    
    //we got the video stream
    if (m_vsIndex != -1)
    {
        m_pVideoCodecCtx = m_pAVFormatContext->streams[m_vsIndex]->codec;
        m_videoTimeBase = m_pAVFormatContext->streams[m_vsIndex]->time_base;
        
        AVCodec *pAVCodecVideo = avcodec_find_decoder(m_pVideoCodecCtx->codec_id);

        if(avcodec_open2(m_pVideoCodecCtx, pAVCodecVideo, NULL) != 0)
        {
            std::cout << "Can not open the video information" << std::endl;
        }
        
        m_imgWidth = VIDEO_OUTPUT_WIDTH;
        m_imgHeight = VIDEO_OUTPUT_HEIGHT;
        
        m_imgBufferLen = avpicture_get_size(PIX_FMT_RGBA,
                                            VIDEO_OUTPUT_WIDTH,
                                            VIDEO_OUTPUT_HEIGHT);
        
        m_pImageConvertCtx = sws_getContext(m_pVideoCodecCtx->width,
                                            m_pVideoCodecCtx->height,
                                            m_pVideoCodecCtx->pix_fmt,
                                            VIDEO_OUTPUT_WIDTH,
                                            VIDEO_OUTPUT_HEIGHT,
                                            PIX_FMT_RGBA,
                                            SWS_BICUBIC,
                                            NULL,
                                            NULL,
                                            NULL);
        
        m_pVideoDecodedFrame = avcodec_alloc_frame();
        m_pDecodedPicture = (AVPicture*)av_mallocz(sizeof(AVPicture));
        
        m_pGLWrapper = new CCGLWrapper(pIPlayerDelegate);
    }
}

void CCPlayerContext::ReleaseContextInformation()
{
    avcodec_close(m_pAudioCodecCtx);
    avcodec_close(m_pVideoCodecCtx);
}

}