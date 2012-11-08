#ifndef ALWRAPPER_H
#define ALWRAPPER_H

#include "Common.h"
#include "AudioDef.h"

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

namespace CCPlayer
{

class ALWrapper {
public:
	ALWrapper(const std::string &deviceName = "");
	virtual ~ALWrapper();

public:
    void SetAudioCtx(ALenum channels, ALuint rate, ALenum format);
    void InitAudioFrame(AudioFrame* pAudioFrame, int index);
    bool NeedData();
    void UpdateAudioFrame(AudioFrame* pAudioFrame);

public:
    void Play();
    int64_t GetReadPlayedTime();

private:
	void Create(const std::string &deviceName);
    
private:
	ALenum m_audChannels;
	ALenum m_audFormat;
	ALuint m_audRate;

private:
    ALuint m_audSource;
    ALuint m_audBuffers[AUDIO_BUFFER_NUMBER];

private:
	ALCdevice *m_audioDevice;
	ALCcontext *m_audioContext;
};

} /* namespace cc */
#endif /* ALWRAPPER_HPP_ */
