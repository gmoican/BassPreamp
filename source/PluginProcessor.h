#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "punk_dsp/punk_dsp.h"
#include "ff_meters/ff_meters.h"

#if (MSVC)
#include "ipps.h"
#endif

namespace Parameters
{
    // ======= UTILITY PARAMETERS ========
    constexpr auto inId = "in_gain";
    constexpr auto inName = "Input Gain (dB)";
    constexpr auto inDefault = 0.0f;
    constexpr auto inMin = -24.0f;
    constexpr auto inMax = 24.0f;

    constexpr auto gateId = "gate_thresh";
    constexpr auto gateName = "Gate (dB)";
    constexpr auto gateDefault = -80.f;
    constexpr auto gateMin = -90.0f;
    constexpr auto gateMax = 0.0f;

    constexpr auto mixId = "mix";
    constexpr auto mixName = "Mix (%)";
    constexpr auto mixDefault = 100.f;
    constexpr auto mixMin = 0.0f;
    constexpr auto mixMax = 100.0f;

    constexpr auto outId = "out_gain";
    constexpr auto outName = "Output Gain (dB)";
    constexpr auto outDefault = 0.f;
    constexpr auto outMin = -24.0f;
    constexpr auto outMax = 24.0f;

    // ====== PROCESSOR PARAMETERS ===========
    // Character - One-knob for adjusting the input eq-curve
    constexpr auto characterId = "character";
    constexpr auto characterName = "Character";
    constexpr auto characterDefault = 0.5f;
    constexpr auto characterMin = 0.0f;
    constexpr auto characterMax = 1.0f;

    // Drive: Adjust the input-gain and mix of a tube saturator
    constexpr auto driveId = "drive";
    constexpr auto driveName = "Drive";
    constexpr auto driveDefault = 0.5f;
    constexpr auto driveMin = 0.0f;
    constexpr auto driveMax = 1.0f;

    // Deep: Adjusts the input gain + ratio of a compressor running below 500Hz
    constexpr auto lowCompId = "lowcomp";
    constexpr auto lowCompName = "lowComp";
    constexpr auto lowCompDefault = 0.5f;
    constexpr auto lowCompMin = 0.0f;
    constexpr auto lowCompMax = 1.0f;

    // Bite: Adjusts the input gain + ratio of a compressor running above 500Hz
    constexpr auto hiCompId = "hicomp";
    constexpr auto hiCompName = "hiComp";
    constexpr auto hiCompDefault = 0.5f;
    constexpr auto hiCompMin = 0.0f;
    constexpr auto hiCompMax = 1.0f;

    // Bass: Low-shelf filter at the end of the chain
    constexpr auto bassId = "bass";
    constexpr auto bassName = "Bass";
    constexpr auto bassDefault = 0.5f;
    constexpr auto bassMin = 0.0f;
    constexpr auto bassMax = 1.0f;

    // Treble: High-shelf filter at the end of the chain
    constexpr auto trebleId = "treble";
    constexpr auto trebleName = "Treble";
    constexpr auto trebleDefault = 0.5f;
    constexpr auto trebleMin = 0.0f;
    constexpr auto trebleMax = 1.0f;

    // Lo-Cut: Hi-pass filter at the end of the chain
    constexpr auto locutId = "locut";
    constexpr auto locutName = "Lo-Cut";
    constexpr auto locutDefault = 0.5f;
    constexpr auto locutMin = 0.0f;
    constexpr auto locutMax = 1.0f;
}

class BassPreampProcessor : public juce::AudioProcessor
{
public:
    BassPreampProcessor();
    ~BassPreampProcessor() override;

    // DEFAULT STUFF ===============================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // ===== MY STUFF ===============================================================
    juce::AudioProcessorValueTreeState apvts;
    
    void updateParameters();
    
    foleys::LevelMeterSource& getInputMeter() { return inputMeter; }
    foleys::LevelMeterSource& getOutputMeter() { return outputMeter; }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParams();
    
    juce::AudioBuffer<float> lowCompBuffer, highCompBuffer;
    
    using IIRFilter = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using LRFilter = juce::dsp::LinkwitzRileyFilter<float>;
    using Mix = juce::dsp::DryWetMixer<float>;
    
    // --- INTERNAL PARAMETER HANDLING ---
    // Utilities
    float inGain = 1.0f;
    float outGain = 1.0f;
    punk_dsp::Gate gate;
    Mix dryWetMixer;
    punk_dsp::Waveshaper outputClipper;
    
    // --- PREAMP PROCESSORS ---
    punk_dsp::TubeModel saturator;
    punk_dsp::Compressor lowCompressor, highCompressor;
    
    juce::dsp::ProcessorChain<IIRFilter, IIRFilter, IIRFilter, IIRFilter, IIRFilter> characterEq;
    juce::dsp::ProcessorChain<IIRFilter, IIRFilter, IIRFilter> postEq;
    
    // Multiband compressors
    float lowBandGain = 1.0f;
    float highBandGain = 1.0f;
    LRFilter lowPassFilter, highPassFilter;
    
    // --- FOLEYS METERS ---
    foleys::LevelMeterSource inputMeter, outputMeter;

    // =============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BassPreampProcessor)
};
