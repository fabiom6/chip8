#pragma once

#include <cstdint>
#include <SDL.h>

class Sound
{
public:
    Sound();
    ~Sound();
    void play();
    void stop();

    const double m_sineFreq;
    const double m_sampleFreq;
    const double m_samplesPerSine;
    uint32_t m_samplePos;

private:
    static void SDLAudioCallback(void* data, Uint8* buffer, int length);

    SDL_AudioDeviceID m_device;
};
