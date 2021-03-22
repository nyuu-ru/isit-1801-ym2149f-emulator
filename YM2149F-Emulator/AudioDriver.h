/*
 * AudioDriver.h
 *
 *  Created on: 22 мар. 2021 г.
 *      Author: unyuu
 */

#ifndef AUDIODRIVER_H_
#define AUDIODRIVER_H_

#include <SDL2/SDL.h>

class AudioDriver final
{
public:
	static constexpr int SAMPLING_RATE = 44100;
protected:
	SDL_AudioDeviceID _dev {};
	SDL_AudioSpec _spec {};
public:
	AudioDriver();
	~AudioDriver();

	void enqueue(const short * data, size_t length);
};

#endif /* AUDIODRIVER_H_ */
