/*
 * AudioDriver.cc
 *
 *  Created on: 22 мар. 2021 г.
 *      Author: unyuu
 */

#include "AudioDriver.h"
#include <cassert>

AudioDriver::AudioDriver()
{
	_spec.channels = 1;
	_spec.freq = SAMPLING_RATE;
	_spec.samples = 1024;
	_spec.format = AUDIO_S16;

	_dev = SDL_OpenAudioDevice(nullptr, 0, &_spec, &_spec, 0);
	assert(_dev >= 2);
	SDL_PauseAudioDevice(_dev, 0);
}

AudioDriver::~AudioDriver()
{
	SDL_PauseAudioDevice(_dev, 1);
	SDL_CloseAudioDevice(_dev);
}

void AudioDriver::enqueue(const short *data, size_t length)
{
	SDL_QueueAudio(_dev, data, length * sizeof(short));
}
