/*
 * ALWrapper.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: root
 */

#include "ALWrapper.h"

namespace CCPlayer
{

#define	NUM_BUFFERS	6
#define BUFFER_SIZE 4096

ALWrapper::ALWrapper(const std::string &deviceName)
{
	//Create the audio context
	Create(deviceName);
}

ALWrapper::~ALWrapper()
{
	//close the al context
	alcDestroyContext(m_audioContext);

	//close the al device
	alcCloseDevice(m_audioDevice);
}

void ALWrapper::SetAudioCtx(ALenum channels, ALuint rate, ALenum format)
{
    m_audChannels = channels;
    m_audRate = rate;
    m_audFormat = format;
}

void ALWrapper::Create(const std::string &deviceName)
{
	// Wait for the audio decoder init
	if(deviceName.empty())
	{
		m_audioDevice = alcOpenDevice(NULL);
	}else{
		m_audioDevice = alcOpenDevice(deviceName.data());
	}

	if(m_audioDevice == NULL)
	{
		std::cout << "Cannot open audio device" << std::endl;
		return ;
	}

	m_audioContext = alcCreateContext(m_audioDevice, NULL);
	if(m_audioContext == NULL)
	{
		std::cout << "Cannot open audio context" << std::endl;
		return ;
	}

	if(alcMakeContextCurrent(m_audioContext) == ALC_FALSE)
	{
		std::cout << "Cannot make current context" << std::endl;
		return ;
	}

    alGenSources(1, &m_audSource);
	assert(alGetError() == AL_NO_ERROR && "Could not create sources");

    alGenBuffers(AUDIO_BUFFER_NUMBER, m_audBuffers);
	assert(alGetError() == AL_NO_ERROR && "Could not create buffers");

	alSource3i(m_audSource, AL_POSITION, 0, 0, -1);
	alSourcei(m_audSource, AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcei(m_audSource, AL_ROLLOFF_FACTOR, 0);
	assert(alGetError() == AL_NO_ERROR && "Could not set source parameters");

	alSourceRewind(m_audSource);
	alSourcei(m_audSource, AL_BUFFER, 0);
	assert(alGetError() == AL_NO_ERROR && "Could not rewind buffers");
    
    alSourcef(m_audSource, AL_GAIN, 0.0f);
}

void ALWrapper::InitAudioFrame(AudioFrame* pAudioFrame, int index)
{
    alBufferData(m_audBuffers[index],
                 m_audFormat,
                 pAudioFrame->GetFrameData(),
                 pAudioFrame->GetFrameSize(),
                 m_audRate);
    alSourceQueueBuffers(m_audSource, 1, &m_audBuffers[index]);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cout << "Init audio render." << std::endl;
    }
}

bool ALWrapper::NeedData()
{
    //std::cout << "NeedData called" << std::endl;
    ALint processed = 0;
    alGetSourcei(m_audSource, AL_BUFFERS_PROCESSED, &processed);
    if (alGetError() != AL_NO_ERROR)
    {
        //std::cout << "Cannot check OpenAL source state." << std::endl;
    }
    if (processed == 0)
    {
//        ALint state = AL_SOURCE_STATE;
//        alGetSourcei(m_audSource, AL_SOURCE_STATE, &state);
//        if (alGetError() != AL_NO_ERROR)
//        {
//            //std::cout << "Cannot check OpenAL source state." << std::endl;
//        }
//        if (state != AL_PLAYING)
//        {
//            alSourcePlay(m_audSource);
//            if (alGetError() != AL_NO_ERROR)
//            {
//                //std::cout << "Cannot restart OpenAL source playback." << std::endl;
//            }
//        }
        return false;
    }
    else
    {
        return true;
    }
}


void ALWrapper::UpdateAudioFrame(AudioFrame* pAudioFrame)
{
    ALuint updateBuffer = 0;
    alSourceUnqueueBuffers(m_audSource, 1, &updateBuffer);
    assert(updateBuffer != 0);
    alBufferData(updateBuffer,
                 m_audFormat,
                 pAudioFrame->GetFrameData(),
                 pAudioFrame->GetFrameSize(),
                 m_audRate);

    alSourceQueueBuffers(m_audSource, 1, &updateBuffer);
    if (alGetError() != AL_NO_ERROR)
    {
        std::cout << "Cannot buffer OpenAL data." << std::endl;
    }
}

void ALWrapper::Play()
{
    ALint state = AL_SOURCE_STATE;
    alGetSourcei(m_audSource, AL_SOURCE_STATE, &state);
    if (alGetError() != AL_NO_ERROR)
    {
        //std::cout << "Cannot check OpenAL source state." << std::endl;
    }
    if (state != AL_PLAYING)
    {
        alSourcePlay(m_audSource);
        if (alGetError() != AL_NO_ERROR)
        {
            //std::cout << "Cannot restart OpenAL source playback." << std::endl;
        }
    }
//    alSourcePlay(m_audSource);
}

} /* namespace cc */
