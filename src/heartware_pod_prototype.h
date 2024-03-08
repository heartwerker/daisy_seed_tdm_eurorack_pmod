#ifndef HEARTDWAREPODPROTOTYPE_H
#define HEARTDWAREPODPROTOTYPE_H

#include "daisy_pod.h"
#include "daisy.h"

using namespace daisy;

// ================================================================
#define ENABLE_CUSTOM_AUDIO 1

#if ENABLE_CUSTOM_AUDIO
#define ENABLE_SAI2 1
// ================================================================
// https://github.com/apfelaudio/eurorack-pmod
#include "heartware/eurorack-pmod.h"
#endif

// ================================================================
class HeartwarePodPrototype
{
public:
    daisy::DaisyPod pod;

    daisy::GPIO ak4619_pdn;

public:
    HeartwarePodPrototype() {}

    void Init(bool boost = false) 
    {
        pod.Init(boost);

        pod.seed.StartLog(false); // waits until Serial Monitor is opened
        pod.seed.PrintLine("init!");
        

#if ENABLE_CUSTOM_AUDIO

        GPIO::Config cfg;
        cfg.mode = GPIO::Mode::OUTPUT;
        cfg.pin = seed::D32;
        ak4619_pdn.Init(cfg);
        ak4619_pdn.Write(true);

        InitAudio();
#endif
    }

    void StartAudio(AudioHandle::AudioCallback cb)  { pod.StartAudio(cb); }

    float AudioSampleRate()  { return pod.AudioSampleRate(); }

    void SetAudioBlockSize(size_t size)  { pod.SetAudioBlockSize(size); }

    void StartAdc()  { pod.StartAdc(); }

    void DelayMs(uint32_t ms)  { pod.DelayMs(ms); }

    void loop() 
    {
        pod.seed.PrintLine("loop!");
        pod.ProcessDigitalControls();
        pmod.LED_updateVU();
    }

    void UpdateLeds()  { pod.UpdateLeds(); }

    void ProcessAnalogControls()  { pod.ProcessAnalogControls(); }

    // ================================================================
#if ENABLE_CUSTOM_AUDIO
    EurorackPMOD pmod;

private:
    // Taken from Patchset SAI2 stuff -- run this between seed configure and init
    void InitAudio()
    {

        
        pod.seed.PrintLine("InitAudio");
        // Handle Seed Audio as-is and then
        SaiHandle::Config sai_config[2];
        SaiHandle sai_handle[2];

        // ================================================================
        // Internal Codec
        if (pod.seed.CheckBoardVersion() == DaisySeed::BoardVersion::DAISY_SEED_1_1)
        {
            sai_config[0].pin_config.sa = {DSY_GPIOE, 6};
            sai_config[0].pin_config.sb = {DSY_GPIOE, 3};
            sai_config[0].a_dir = SaiHandle::Config::Direction::RECEIVE;
            sai_config[0].b_dir = SaiHandle::Config::Direction::TRANSMIT;
        }
        else
        {
            sai_config[0].pin_config.sa = {DSY_GPIOE, 6};
            sai_config[0].pin_config.sb = {DSY_GPIOE, 3};
            sai_config[0].a_dir = SaiHandle::Config::Direction::TRANSMIT;
            sai_config[0].b_dir = SaiHandle::Config::Direction::RECEIVE;
        }

        sai_config[0].periph = SaiHandle::Config::Peripheral::SAI_1;
        // sai_config[0].proto           = SAI_I2S_MSBJUSTIFIED;
        sai_config[0].sr = SaiHandle::Config::SampleRate::SAI_48KHZ;
        sai_config[0].bit_depth = SaiHandle::Config::BitDepth::SAI_24BIT;
        sai_config[0].a_sync = SaiHandle::Config::Sync::MASTER;
        sai_config[0].b_sync = SaiHandle::Config::Sync::SLAVE;
        sai_config[0].pin_config.fs = {DSY_GPIOE, 4};
        sai_config[0].pin_config.mclk = {DSY_GPIOE, 2};
        sai_config[0].pin_config.sck = {DSY_GPIOE, 5};

        sai_handle[0].Init(sai_config[0]);

        // ================================================================
#if ENABLE_SAI2
        // External Codec
        sai_config[1].periph = SaiHandle::Config::Peripheral::SAI_2;
        sai_config[1].sr = SaiHandle::Config::SampleRate::SAI_48KHZ;
        sai_config[1].bit_depth = SaiHandle::Config::BitDepth::SAI_24BIT;
        sai_config[1].tdm_slots = 4;
        sai_config[1].b_sync = SaiHandle::Config::Sync::MASTER;
        sai_config[1].a_sync = SaiHandle::Config::Sync::SLAVE;
        sai_config[1].b_dir = SaiHandle::Config::Direction::RECEIVE;
        sai_config[1].a_dir = SaiHandle::Config::Direction::TRANSMIT;
        // sai_config[1].b_dir = SaiHandle::Config::Direction::TRANSMIT;
        // sai_config[1].a_dir = SaiHandle::Config::Direction::RECEIVE;

        sai_config[1].pin_config.mclk = seed::D24;
        sai_config[1].pin_config.sb = seed::D25; // Transmit
        sai_config[1].pin_config.sa = seed::D26; // Receive
        sai_config[1].pin_config.fs = seed::D27;
        sai_config[1].pin_config.sck = seed::D28;

        sai_handle[1].Init(sai_config[1]);
#endif
        pod.seed.PrintLine("SAI structs initialized!");

        I2CHandle::Config i2c_cfg;
        i2c_cfg.periph         = I2CHandle::Config::Peripheral::I2C_1;
        i2c_cfg.mode           = I2CHandle::Config::Mode::I2C_MASTER;
        i2c_cfg.speed          = I2CHandle::Config::Speed::I2C_100KHZ;
        // i2c_cfg.speed          = I2CHandle::Config::Speed::I2C_400KHZ;
        i2c_cfg.pin_config.scl = seed::D11;
        i2c_cfg.pin_config.sda = seed::D12;
        I2CHandle i2c1; 
        i2c1.Init(i2c_cfg);
        pod.seed.PrintLine("I2C initialized!");

        pmod.Init(i2c1);
        pod.seed.PrintLine("PMOD initialized!");

        // Reinit Audio for _both_ codecs...
        AudioHandle::Config cfg;
        cfg.blocksize = 48;
        cfg.samplerate = SaiHandle::Config::SampleRate::SAI_48KHZ;
        cfg.postgain = 1.0f;
        // SetAudioSampleRate(cfg.samplerate);

#if ENABLE_SAI2
        pod.seed.audio_handle.Init(cfg, sai_handle[0], sai_handle[1]);
#else
        pod.seed.audio_handle.Init(cfg, sai_handle[0]);
#endif
    }
#endif
};

#endif // HEARTDWAREPODPROTOTYPE_H
