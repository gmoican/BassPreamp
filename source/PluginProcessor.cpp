#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BassPreampProcessor::BassPreampProcessor()
: AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
                  ), apvts(*this, nullptr, "Parameters", createParams())
{
}

BassPreampProcessor::~BassPreampProcessor()
{
}

//==============================================================================
const juce::String BassPreampProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BassPreampProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool BassPreampProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool BassPreampProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double BassPreampProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BassPreampProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int BassPreampProcessor::getCurrentProgram()
{
    return 0;
}

void BassPreampProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String BassPreampProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void BassPreampProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

// =========== PARAMETER LAYOUT ====================
juce::AudioProcessorValueTreeState::ParameterLayout BassPreampProcessor::createParams()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // --- Utility Parameters Group ---
    auto utilsGroup = std::make_unique<juce::AudioProcessorParameterGroup>(
                                                                           "utils",       // Group ID (must be unique string)
                                                                           "Utilities",   // Group Name (displayed in host)
                                                                           " | "          // Separator string for hosts that flatten the hierarchy
                                                                           );
    
    // Input Level
    utilsGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                     Parameters::inId,
                                                                     Parameters::inName,
                                                                     juce::NormalisableRange<float>(Parameters::inMin, Parameters::inMax, 0.1f),
                                                                     Parameters::inDefault
                                                                     )
                         );
    
    // Input Noise Gate
    utilsGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                     Parameters::gateId,
                                                                     Parameters::gateName,
                                                                     juce::NormalisableRange<float>(Parameters::gateMin, Parameters::gateMax, 0.1f),
                                                                     Parameters::gateDefault
                                                                     )
                         );
    
    // Mix
    utilsGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                     Parameters::mixId,
                                                                     Parameters::mixName,
                                                                     juce::NormalisableRange<float>(Parameters::mixMin, Parameters::mixMax, 1.0f),
                                                                     Parameters::mixDefault
                                                                     )
                         );
    
    // Output Level
    utilsGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                     Parameters::outId,
                                                                     Parameters::outName,
                                                                     juce::NormalisableRange<float>(Parameters::outMin, Parameters::outMax, 0.1f),
                                                                     Parameters::outDefault
                                                                     )
                         );
    
    layout.add(std::move(utilsGroup));
    
    // --- Preamp Parameters Group ---
    
    auto preampGroup = std::make_unique<juce::AudioProcessorParameterGroup>(
                                                                            "preamp",         // Group ID (must be unique string)
                                                                            "Preamp",    // Group Name (displayed in host)
                                                                            " | "          // Separator string for hosts that flatten the hierarchy
                                                                            );
    
    // Character - One-knob for adjusting the input eq-curve
    preampGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                      Parameters::characterId,
                                                                      Parameters::characterName,
                                                                      juce::NormalisableRange<float>(Parameters::characterMin, Parameters::characterMax, 0.01f),
                                                                      Parameters::characterDefault
                                                                      )
                          );
    
    // Drive: Adjust the input-gain and mix of a tube saturator
    preampGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                      Parameters::driveId,
                                                                      Parameters::driveName,
                                                                      juce::NormalisableRange<float>(Parameters::driveMin, Parameters::driveMax, 0.01f),
                                                                      Parameters::driveDefault
                                                                      )
                          );
    
    // Comp: Adjusts the input-gain and mix of a compressor running in series
    preampGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                      Parameters::compId,
                                                                      Parameters::compName,
                                                                      juce::NormalisableRange<float>(Parameters::compMin, Parameters::compMax, 0.01f),
                                                                      Parameters::compDefault
                                                                      )
                          );
    
    // Pump: Adjusts the input-gain and mix of a compressor running in parallel
    preampGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                      Parameters::pumpId,
                                                                      Parameters::pumpName,
                                                                      juce::NormalisableRange<float>(Parameters::pumpMin, Parameters::pumpMax, 0.01f),
                                                                      Parameters::pumpDefault
                                                                      )
                          );
    
    // Bass: Low-shelf filter at the end of the chain
    preampGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                      Parameters::bassId,
                                                                      Parameters::bassName,
                                                                      juce::NormalisableRange<float>(Parameters::bassMin, Parameters::bassMax, 0.01f),
                                                                      Parameters::bassDefault
                                                                      )
                          
                          );
    
    // Treble: High-shelf filter at the end of the chain
    preampGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                      Parameters::trebleId,
                                                                      Parameters::trebleName,
                                                                      juce::NormalisableRange<float>(Parameters::trebleMin, Parameters::trebleMax, 0.01f),
                                                                      Parameters::trebleDefault
                                                                      )
                          );
    
    // Lo-Cut: Hi-pass filter at the end of the chain
    preampGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                      Parameters::locutId,
                                                                      Parameters::locutName,
                                                                      juce::NormalisableRange<float>(Parameters::locutMin, Parameters::locutMax, 0.01f),
                                                                      Parameters::locutDefault
                                                                      )
                          );
    
    layout.add(std::move(preampGroup));
    
    return layout;
}

//==============================================================================
void BassPreampProcessor::updateParameters()
{
    // --- 1. Utilities
    inGain = juce::Decibels::decibelsToGain( apvts.getRawParameterValue(Parameters::inId)->load() );
    
    gate.updateThres( apvts.getRawParameterValue(Parameters::gateId)->load() );
    
    const auto mixValue = apvts.getRawParameterValue(Parameters::mixId)->load();
    dryWetMixer.setWetMixProportion( mixValue / 100.f );
    
    outGain = juce::Decibels::decibelsToGain( apvts.getRawParameterValue(Parameters::outId)->load() );
    
    // --- 2. Preamp
    // TODO: Adjusts all mappings and variables affected
    
    // Character - Needs an eq-curve
    const auto characterValue = apvts.getRawParameterValue(Parameters::characterId)->load();
    const auto charLowGain = juce::jmap(characterValue, 1.f, 2.f);
    const auto charMidGain = juce::jmap(characterValue, 1.f, 0.1f);
    const auto charMidHiGain = juce::jmap(characterValue, 1.f, 1.8f);
    const auto charHiGain = juce::jmap(characterValue, 1.f, 2.4f);
    
    double sampleRate = getSampleRate();
    
    *characterEq.get<0>().state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 80, 0.5f, charLowGain);
    *characterEq.get<1>().state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, 500.f, 1.f, charMidGain);
    *characterEq.get<2>().state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 680.f, 1.f, charMidHiGain);
    *characterEq.get<3>().state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 3100.f, 1.f, charHiGain);
    
    // Saturator
    const auto driveValue = apvts.getRawParameterValue(Parameters::driveId)->load();
    saturator.setDrive( juce::jmap(driveValue, 0.5f, 2.0f) );
    
    // Comp
    const auto compValue = apvts.getRawParameterValue(Parameters::compId)->load();
    seriesCompressor.updateThres( juce::jmap(compValue, -6.f, -20.f) );
    seriesCompressor.updateMakeUp( juce::jmap(compValue, 0.f, 15.f) );
    
    // Pump
    const auto pumpValue = apvts.getRawParameterValue(Parameters::pumpId)->load();
    parallelCompressor.updateMix( juce::jmap(pumpValue, 0.0f, 0.5f) );
    parallelCompressor.updateMakeUp( juce::jmap(pumpValue, 0.0f, 12.f) );
    
    // Bass - Treble - LoCut
    const auto bassValue = apvts.getRawParameterValue(Parameters::bassId)->load();
    const auto bassGain = juce::mapToLog10(bassValue, 0.25f, 4.0f);
        
    const auto trebleValue = apvts.getRawParameterValue(Parameters::trebleId)->load();
    const auto trebleGain = juce::mapToLog10(trebleValue, 0.2f, 5.0f);
    
    const auto locutValue = apvts.getRawParameterValue(Parameters::locutId)->load();
    const auto locutFreq = juce::jmap(locutValue, 20.f, 90.f);
    
    *postEq.get<0>().state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 150.f, 0.7f, bassGain);
    *postEq.get<1>().state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 3500.f, 0.7f, trebleGain);
    *postEq.get<2>().state = *juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, locutFreq);
    
}

void BassPreampProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;
    
    gate.prepare(spec);
    gate.updateAttack(100.f);
    gate.updateRelease(30.f);
    gate.updateMix(90.f);
    
    outputClipper.setOutGain(1.7f);
    
    dryWetMixer.prepare(spec);
    
    characterEq.prepare(spec);
    characterEq.reset();
    *characterEq.get<4>().state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 12000.f);
    
    postEq.prepare(spec);
    postEq.reset();
    
    seriesCompressor.prepare(spec);
    seriesCompressor.updateKnee(12.0f);
    seriesCompressor.updateRatio(6.0f);
    seriesCompressor.updateAttack(10.0f);
    seriesCompressor.updateRelease(30.0f);
    seriesCompressor.updateFeedForward(false);
    
    parallelCompressor.prepare(spec);
    parallelCompressor.updateRatio(12.f);
    parallelCompressor.updateThres(-20.f);
    parallelCompressor.updateKnee(15.f);
    parallelCompressor.updateAttack(30.f);
    parallelCompressor.updateRelease(60.f);
    
    saturator.setOutGain(1.7f);
    saturator.setHarmonicBalance(0.8f);
    saturator.setSagTime(50.0f);
    
    // this prepares the meterSource to measure all output blocks and average over 100ms to allow smooth movements
    inputMeter.resize (getTotalNumOutputChannels(), (int) (sampleRate * 0.1 / samplesPerBlock));
    outputMeter.resize (getTotalNumOutputChannels(), (int) (sampleRate * 0.1 / samplesPerBlock));
    
    updateParameters();
}

void BassPreampProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool BassPreampProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    
    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    
    return true;
#endif
}

void BassPreampProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // Clear any unused output channel
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Update params
    updateParameters();
    
    juce::dsp::AudioBlock<float> audioBlock = juce::dsp::AudioBlock<float>(buffer);
    
    // LEVEL METERS - INPUT
    inputMeter.measureBlock (buffer);
    
    // 1. UTILITIES - INPUT GAIN & GATE
    dryWetMixer.pushDrySamples(audioBlock);

    buffer.applyGain(inGain);
    gate.process(buffer);
    
    // 2.1. Preamp - Character
    characterEq.process( juce::dsp::ProcessContextReplacing<float>(audioBlock) );
    
    // 2.2. Preamp - Drive
    saturator.processBuffer(buffer);
    
    // 2.3. Preamp - Comp
    seriesCompressor.process(buffer);
    
    // 2.4. Preamp - Pump
    parallelCompressor.process(buffer);
    
    // 2.5. Preamp - Output EQ
    postEq.process( juce::dsp::ProcessContextReplacing<float>(audioBlock) );
    
    // 3. UTILITIES - MIX & OUTPUT GAIN
    outputClipper.applyTanhClipper(buffer);
    buffer.applyGain(outGain);
    dryWetMixer.mixWetSamples( audioBlock );
    
    // LEVEL METERS - OUTPUT
    outputMeter.measureBlock (buffer);
}

//==============================================================================
bool BassPreampProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BassPreampProcessor::createEditor()
{
    return new PluginEditor (*this);
    // return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void BassPreampProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void BassPreampProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BassPreampProcessor();
}
