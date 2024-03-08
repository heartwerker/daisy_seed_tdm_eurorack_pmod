#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

#define AUDIO_BLOCK_SIZE 32 // keep this to 32

#include "daisy_core.h"
#include "heartware_pod_prototype.h"

// ================================================================
extern HeartwarePodPrototype heartware;


enum AudioMode
{
    MODE_FEEDTHROUGH,
    MODE_SIN_GENERATOR
};

AudioMode audioMode = MODE_FEEDTHROUGH;
// AudioMode audioMode = MODE_SIN_GENERATOR;
bool auto_toggle_audioMode = true;

/** Generation of a simple Audio signal */
#include <cmath>
using namespace daisy;
#define M_TWOPI 6.28318530717958647692f
const float kSignalIncrement = (M_TWOPI * 100) * (1.0 / 48000);
float phs;

float amplitude_increment = 0.0f;
// float amplitude_increment = 1.0 / 48000.0 /5.0;
float amplitude = 1.0f;

volatile float in_buffer[6][AUDIO_BLOCK_SIZE];
// ================================================================
// ===== Audio "Thread"
// ================================================================
static void AudioCallback(AudioHandle::InputBuffer in,
                          AudioHandle::OutputBuffer out,
                          size_t size)
{
    static int counter = 0; 
    if (counter++ > ((48000 / AUDIO_BLOCK_SIZE) * 5))
    {
        counter = 0;
        if (auto_toggle_audioMode)
            audioMode = (audioMode == MODE_FEEDTHROUGH) ? MODE_SIN_GENERATOR : MODE_FEEDTHROUGH;
    }

    for (size_t i = 0; i < size; i++)
    {
        for (int ch = 0; ch < 6; ch++)
            in_buffer[ch][i] = in[ch][i];

        if (audioMode == MODE_FEEDTHROUGH)
        {

#if 0   // map all input to all output!
        float mixed = 0;
        mixed += in[0][i] + in[1][i];
        mixed += in[2][i] + in[3][i] + in[4][i] + in[5][i];
        for (int ch = 0; ch < 6; ch++)
            out[ch][i] = mixed * 100;
#elif 1 // map all input to all output (L/R)
            float mix_LR[2];
            // mix_LR[0] = in[0][i] + in[2][i] + in[4][i];
            // mix_LR[1] = in[1][i] + in[3][i] + in[5][i];
            mix_LR[0] = in[4][i];
            mix_LR[1] = in[5][i];
            for (int ch = 0; ch < 6; ch++)
                out[ch][i] = mix_LR[ch % 2] * 1.0f;
#endif
        }
        else if (audioMode == MODE_SIN_GENERATOR)
        {
            float signal = sin(phs) * amplitude;
            amplitude += amplitude_increment;
            if (amplitude > 1.5f || amplitude < 0.2f)
                amplitude_increment *= -1.0f;

            phs += kSignalIncrement;
            if (phs > M_TWOPI)
                phs -= M_TWOPI;

            for (int ch = 0; ch < 6; ch++)
                out[ch][i] = signal;
        }

        for (int ch = 0; ch < 6; ch++)
            out[ch][i] = out[ch][i] * 0.3f;

#if 0 
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