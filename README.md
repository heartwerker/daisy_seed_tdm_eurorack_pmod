# Seed TDM Example for Eurorack PMOD

Welcome to the `seed_tdm_example_eurorack_pmod` repository! 

This is an example implementation for using the Daisy Seed with TDM codecs (Time Division Multiplexing/ multi-channel). 
It makes use of my libDaisy fork: [libDaisy/audio-tdm-codec-ak4619](https://github.com/heartwerker/libDaisy/tree/feature/audio-tdm-codec-ak4619) (
which extends libDaisy by adding support for TDM in general with the AK4619 codec as an first example. --> see [Feature - TDM SAI/Codec Support #499](https://github.com/electro-smith/libDaisy/issues/499)

## Features

- Shows, how to use Daisy Seed with TDM codecs.
- Using the **amazing and highly recommended: [EURORACK-PMOD by apfelaudio](https://github.com/apfelaudio/eurorack-pmod)** module!


- **Driver Implementation**: 
    - Eurorack PMOD driver includes initialisation of Codec + a simple LED driver (+VU meter)
    - The libDaisy fork includes a driver for the [codec_ak4619](https://www.akm.com/eu/en/products/audio/audio-codec/ak4619vn/)

## Getting Started

1. **Ensure that you have the following installed:**

    - [Daisy toolchain and environment](https://github.com/electro-smith/DaisyWiki/wiki/1.-Setting-Up-Your-Development-Environment)

2. **Clone the Repository with submodules:** 

   ```bash
   git clone https://github.com/heartwerker/seed_tdm_example_eurorack_pmod.git --recursive
   ```

3. **Connect Eurorack-PMOD to Daisy:**

The PMOD pinout is on the silkscreen on the back side of the board. Details are below. Note that Pin 1 is the SQUARE pad.

**PIN Mapping**

SEED - PMOD

1) D25 - SDI (AK4619VN SDIN1)
2) D11 - SCL (I2C SCL for AK4619VN CODEC, EEPROM, LED + JACK IO expanders)
3) D26 - SDO (AK4619VN SDOUT1)
4) D12 - SDA (I2C SDA)
5) D27 - LRCK (CODEC clock line)
6) 3V3 - PDN (CODEC power down, -- HIGH means everything is on)
7) D28 - BICK (CODEC clock line)
8) D24 - MCLK (CODEC clock line)
9) GND - GND
10) GND - GND
11) 3V3 - 3V3 IN
12) 3V3 - 3V3 IN

4. **Run + Have Fun with all them channelz!**