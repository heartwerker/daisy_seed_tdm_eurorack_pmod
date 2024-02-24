#pragma once
#ifndef EURORACK_PMOD_H
#define EURORACK_PMOD_H

// ================================================================
// https://github.com/apfelaudio/eurorack-pmod
// ================================================================

#include "daisy_core.h"
#include "daisy_seed.h"
#include "dev/codec_ak4619.h" // TODO integrate codec here directly
#include "per/i2c.h"

using namespace daisy;

// ================================================================
#define NUM_LEDS 16

#define LED_DRIVER_PC9635_ADDR \
    0x0A // I2C slave address (0x5<<1) and RW bit (W=0)

const uint8_t led_reg_vals[] = {
    0x81, // MODE1
    0x01, // MODE2
    0x00, // PWM0
    0x00, // PWM1
    0x08, // PWM2
    0x08, // PWM3
    0x04, // PWM4
    0x04, // PWM5
    0x00, // PWM6
    0x10, // PWM7
    0x10, // PWM8
    0x10, // PWM9
    0x10, // PWM10
    0x10, // PWM11
    0x01, // PWM12
    0x01, // PWM13
    0x10, // PWM14
    0x10, // PWM15
    0xFF, // GRPPWM
    0x00, // GRPFREQ
    0xAA, // LEDOUT0
    0xAA, // LEDOUT1
    0xAA, // LEDOUT2
    0xAA  // LEDOUT3
};

// ================================================================

class EurorackPMOD
{
public:
    enum class Result
    {
        OK,
        ERR,
    };

    EurorackPMOD() {}

    Result Init(I2CHandle i2c)
    {
        i2c_ = i2c;

        daisy::AK4619::Result ret = daisy::AK4619::Result::ERR;
        while (ret != daisy::AK4619::Result::OK)
        {
            ret = codec.Init(i2c_);
            if (ret == daisy::AK4619::Result::ERR)
            {
                // blocks trying to init codec
                System::Delay(1000);
            }
        }

        for (int i = 0; i < 24; i++)
        {
            uint8_t val = led_reg_vals[i];
            if (i2c_.WriteDataAtAddress(LED_DRIVER_PC9635_ADDR, i, 1, &val, 1, 250) != I2CHandle::Result::OK)
            {
                return Result::ERR;
            }
        }
        return Result::OK;
    }

// ================================================================
    float max_value_in = 0;
    float max_value_out = 0;
    void AudioVUMeter(AudioHandle::InputBuffer in,
                      AudioHandle::OutputBuffer out,
                      size_t size)
    {
        // simple vu meter for pmod leds:
        for (int ch = 0; ch < 4; ch++)
        {
            float value_in = 0;
            float value_out = 0;

            for (size_t i = 0; i < size; i++)
            {
                if (value_in < fabs(in[ch + 2][i]))
                    value_in += (fabs(in[ch + 2][i]) - value_in) * 0.1;
                if (value_out < fabs(out[ch + 2][i]))
                    value_out += (fabs(out[ch + 2][i]) - value_out) * 0.1;
            }

            if (max_value_in < value_in)
                max_value_in = value_in;

            if (max_value_out < value_out)
                max_value_out = value_out;

            LED_setVU(ch, value_in / max_value_in);
            LED_setVU(ch + 4, value_out / max_value_out);
        }
        max_value_in *= 0.99999;
        max_value_out *= 0.99999;
    }

// ================================================================
    void LED_clearAll()
    {
        for (int i = 0; i < NUM_LEDS; i++)
        {
            led_values[i] = 0;
        }
    }

    void LED_set(uint8_t index, uint8_t r, uint8_t g)
    {
        if (r > 15)
            r = 15;
        if (g > 15)
            g = 15;

        led_values[index * 2] = r;
        led_values[index * 2 + 1] = g;
    }

    void LED_setVU(uint8_t index, float value)
    {
        if (vu_values[index] < value)
            vu_values[index] = value;
    }

    void LED_updateVU()
    {
        LED_clearAll();
        for (int i = 0; i < 8; i++)
        {
            vu_values[i] *= 0.999;
            float value = vu_values[i];
            value = value < 0 ? 0 : value;
            value = value > 1 ? 1 : value;
            // scale value
            value = value * value * value;
            value = value * value * value;
            if (i >= 4)
                LED_set(i, value * 16, 0);
            else
                LED_set(i, 0, value * 16);
        }
        LED_update();
    }

    Result LED_update()
    {
#if 0
        // this somehow does not work yet..
        for (int i = 0; i < 16; i++)
        {
            uint8_t val = led_values[i];
            if (i2c_.WriteDataAtAddress(LED_DRIVER_PC9635_ADDR, i+2, 1, &val, 1, 250) != I2CHandle::Result::OK)
            {
                return Result::ERR;
            }
        }
#else // TODO make it work without sending the whole init struct, only led_values

        for (int i = 0; i < 24; i++)
        {
            uint8_t val = led_reg_vals[i];

            if (i >= 2 && i < 18)
                val = led_values[i - 2];

            if (i2c_.WriteDataAtAddress(LED_DRIVER_PC9635_ADDR, i, 1, &val, 1, 250) != I2CHandle::Result::OK)
            {
                return Result::ERR;
            }
        }
#endif
        return Result::OK;
    }

// ================================================================
public:
    AK4619 codec;

private:
    I2CHandle i2c_;

    float vu_values[8] = {0, 0, 0, 0, 0, 0, 0, 0};

    uint8_t led_values[16] = {
        0, 0, 0, 0, 0, 0, 0, 0, // 0-7
        0, 0, 0, 0, 0, 0, 0, 0  // 8-15
    };
};

#endif