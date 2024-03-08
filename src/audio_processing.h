#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

#include "daisy_core.h"
#include "heartware_pod_prototype.h"

// ================================================================
extern HeartwarePodPrototype heartware;

/** Generation of a simple Audio signal */
#include <cmath>
using namespace daisy;
#define M_TWOPI 6.28318530717958647692f
const float kSignalIncrement = (M_TWOPI * 220) * (1.0 / 48000);
float phs;

float amplitude_increment = 1.0 / 48000.0 /5.0;
float amplitude = 0.0f;
// ================================================================
// ===== Audio "Thread"
// ================================================================
static void AudioCallback(AudioHandle::InputBuffer in,
                          AudioHandle::OutputBuffer out,
                          size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
#if 1 // map all input to all output!
        float mixed = 0;
        mixed += in[0][i] + in[1][i];
        mixed += in[2][i] + in[3][i] + in[4][i] + in[5][i];
        for (int ch = 0; ch < 6; ch++)
            out[ch][i] = mixed;
#elif 1

        float signal = sin(phs) * amplitude;
        amplitude += amplitude_increment;
        if (amplitude > 2.0f || amplitude < 0.0f)
            amplitude_increment *= -1.0f;

        phs += kSignalIncrement;
        if(phs > M_TWOPI)
            phs -= M_TWOPI;

        for (int ch = 0; ch < 6; ch++)
            out[ch][i] = signal;
#else
    // out[2] = max output via audio.cpp
    // out[3] = in[2];
    // etc...
    for (int ch = 3; ch < 6; ch++)
        out[ch][i] = in[ch-1][i] * -1;
#endif
    }

#if ENABLE_CUSTOM_AUDIO
    heartware.pmod.AudioVUMeter(in, out, size);
#endif
}

#endif // AUDIO_PROCESSING_H