# BassPreamp
 [![BassPreamp](https://github.com/gmoican/BassPreamp/actions/workflows/build.yml/badge.svg)](https://github.com/gmoican/BassPreamp/actions/workflows/build.yml)

![DemoImage](docs/images/demo.png)

## Introduction
This is a VST3/AU bass preamp plugin made with [JUCE](https://juce.com/). My goal is blending together ideas and concepts from multiple bass preamps effects into one single processor.

**--- THERE WILL BE A VIDEO LINK HERE WHEN THE DEMO IS READY ---**

## Inner Processing
The signal flow is as follows:
1. Input Gain:
    - From -24 to +24dB.
2. Noise Gate:
    - Threshold: from -90dB to 0dB.
    - Ratio: 6:1.
    - Attack: 100ms.
    - Release: 30ms.
    - Mix: 90% _(if it is set to 100% the gate is very hard)_.
3. Character (inspired from VPF control in MarkBass amps):
    - Boost @ 80Hz.
    - Cuts greatly @ 500Hz.
    - Hi-shelf boosting @ 680Hz.
    - Hi-shelf boosting @ 3100Hz.
4. Drive (multiband):
    - Adjust input-drive level for a [tube emulation](https://github.com/gmoican/TubeModel).
    - Drive levels are different for the low-band and high-band circuit.
5. Deep & Bite (multiband):
    - Adjust input-gain and ratio for multi-band compressors.
    - Fixed values:
        - Threshold: -38dB.
        - Attack: 10ms.
        - Release: 80ms.
        - Feedback topology.
6. Bass, Treble & LoCut:
    - Bass: Low-shelf filter of ±12dB @ 150Hz.
    - Treble: High-shelf filter of ±15dB @ 3500Hz.
    - LoCut: Hi-Pass-Filter with low-cut frequency ranging from 20Hz to 90Hz.
7. Clipper (_TanH_ waveshaping function).
8. Mix control.
9. Output Gain:
    - From -24 to +24dB.

## Actual state of this repository
For the time being, I consider this project to be finished. I recognize that some aspects could be improved or customized for a more niche effect, but I prefer to learn more about other topics and revisit this plugin in a distant future.

If you want to mess with the code and work on top of my plugin, here are some details that could be looked after:
* **DSP**:
    * `Drive` is currently running my [tube emulation](https://github.com/gmoican/TubeModel) with different drive values for the low-band and the high-band. I don't quite like the result, I would try to emulate the *SansAmp 21 Bass Driver*.
* **GUI**:
    * My knobs are so damn ugly... 

