
#include "daisy_core.h"
#include "audio_processing.h"
#include "heartware_pod_prototype.h"

// ================================================================
HeartwarePodPrototype heartware;

#define MAIN_LOOP_DELAY 1   // milliseconds

// ================================================================
int main(void)
{
    heartware.Init(1);
    heartware.SetAudioBlockSize(AUDIO_BLOCK_SIZE);

    heartware.StartAdc();
    heartware.StartAudio(AudioCallback);


    while (1)
    {
        heartware.loop();
        System::Delay(MAIN_LOOP_DELAY);
    }
}
