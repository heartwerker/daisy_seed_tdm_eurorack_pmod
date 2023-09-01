#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

#include "daisy_core.h"
#include "heartware_pod_prototype.h"

// ================================================================
extern HeartwarePodPrototype heartware;

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
#else   
    // out[2] = max output via audio.cpp
    // out[3] = in[2];
    // etc...
    for (int ch = 3; ch < 6; ch++)
        out[ch][i] = in[ch-1][i] * -1;
#endif
    }

    heartware.pmod.AudioVUMeter(in, out, size);
}

#endif // AUDIO_PROCESSING_H