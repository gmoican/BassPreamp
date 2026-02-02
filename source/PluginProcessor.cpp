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
    
    // Low Comp / Deep: Compressor affecting frequencies below 500 Hz
    preampGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                      Parameters::lowCompId,
                                                                      Parameters::lowCompName,
                                                                      juce::NormalisableRange<float>(Parameters::lowCompMin, Parameters::lowCompMax, 0.001f),
                                                                      Parameters::lowCompDefault
                                                                      )
                          );
    
    // Hi Comp / Bite: Compressor affecting frequencies above 500 Hz
    preampGroup->addChild(std::make_unique<juce::AudioParameterFloat>(
                                                                      Parameters::hiCompId,
                                                                      Parameters::hiCompName,
                                                                      juce::NormalisableRange<float>(Parameters::hiCompMin, Parameters::hiCompMax, 0.001f),
                                                                      Parameters::hiCompDefault
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
    
    // --- 2. Pream
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
    
    // Saturator - TODO: Fine tuning
    juce::NormalisableRange<float> lowDriveRange(0.5f, 4.0f);
    lowDriveRange.setSkewForCentre(1.2f);
    juce::NormalisableRange<float> highDriveRange(0.5f, 20.0f);
    highDriveRange.setSkewForCentre(1.6f);
    
    const auto driveValue = apvts.getRawParameterValue(Parameters::driveId)->load();
    lowSaturator.setDrive( lowDriveRange.convertFrom0to1(driveValue) );
    highSaturator.setDrive( highDriveRange.convertFrom0to1(driveValue) );
    
    // Low Comp
    const auto lowCompValue = apvts.getRawParameterValue(Parameters::lowCompId)->load();
    lowBandGain = juce::jmap(lowCompValue, 1.0f, 5.f);
    lowCompressor.updateRatio( juce::jmap(lowCompValue, 8.f, 20.f) );
    lowCompressor.updateMakeUp( juce::jmap(lowCompValue, 1.f, -10.f) );
    
    // Hi Comp
    const auto hiCompValue = apvts.getRawParameterValue(Parameters::hiCompId)->load();
    highBandGain = juce::jmap(hiCompValue, 1.0f, 5.0f);
    highCompressor.updateRatio( juce::jmap(hiCompValue, 8.f, 20.f) );
    highCompressor.updateMakeUp( juce::jmap(hiCompValue, 1.0f, -10.f) );
    
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
    
    // Utility classes
    gate.prepare(spec);
    gate.updateAttack(100.f);
    gate.updateRelease(30.f);
    gate.updateMix(90.f);
    
    outputClipper.setOutGain(1.7f);
    
    dryWetMixer.prepare(spec);
    
    // Character EQ
    characterEq.prepare(spec);
    characterEq.reset();
    *characterEq.get<4>().state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 12000.f);
    
    // Drive
    lowSaturator.setOutGain(1.7f);
    lowSaturator.setHarmonicBalance(1.0f);
    lowSaturator.setSagTime(100.0f);
    highSaturator.setOutGain(1.7f);
    highSaturator.setHarmonicBalance(0.8f);
    highSaturator.setSagTime(50.0f);
    
    // Bass + Treble + LoCut
    postEq.prepare(spec);
    postEq.reset();
    
    // Deep & Bite Compressors + Buffers and filters
    lowCompressor.prepare(spec);
    lowCompressor.updateThres(-38.0f);
    lowCompressor.updateKnee(1.0f);
    lowCompressor.updateAttack(10.0f);
    lowCompressor.updateRelease(80.0f);
    lowCompressor.updateFeedForward(false);
    
    highCompressor.prepare(spec);
    highCompressor.updateThres(-38.f);
    highCompressor.updateKnee(1.f);
    highCompressor.updateAttack(10.f);
    highCompressor.updateRelease(80.f);
    highCompressor.updateFeedForward(false);
    
    // Prepare LR filters
    lowPassFilter.prepare(spec);
    lowPassFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    lowPassFilter.setCutoffFrequency(500.f);
    
    highPassFilter.prepare(spec);
    highPassFilter.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    highPassFilter.setCutoffFrequency(500.f);
    
    // Allocate band buffers
    lowCompBuffer.setSize(spec.numChannels, samplesPerBlock);
    highCompBuffer.setSize(spec.numChannels, samplesPerBlock);
    
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
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

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
    
    // 2.2.1. Copy input to band buffers and create audio blocks for DSP processing
    for (int ch = 0; ch < numChannels; ++ch)
    {
        lowCompBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
        highCompBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
    }
    
    juce::dsp::AudioBlock<float> lowBlock(lowCompBuffer);
    juce::dsp::AudioBlock<float> highBlock(highCompBuffer);
    
    juce::dsp::ProcessContextReplacing<float> lowContext(lowBlock);
    juce::dsp::ProcessContextReplacing<float> highContext(highBlock);
    
    // 2.2.2. Filtering the buffers
    lowPassFilter.process(lowContext);
    highPassFilter.process(highContext);
    
    // 2.3. Apply saturation and compression to each band
    lowSaturator.processBuffer(lowCompBuffer);
    lowCompBuffer.applyGain(lowBandGain);
    lowCompressor.process(lowCompBuffer);
    
    highSaturator.processBuffer(highCompBuffer);
    highCompBuffer.applyGain(highBandGain);
    highCompressor.process(highCompBuffer);
    
    //2.4. Sum buffers together
    buffer.clear();
    for (int ch = 0; ch < numChannels; ++ch)
    {
        buffer.addFrom(ch, 0, lowCompBuffer, ch, 0, numSamples);
        buffer.addFrom(ch, 0, highCompBuffer, ch, 0, numSamples);
    }
    
    // 2.5. Preamp - Output EQ
    postEq.process( juce::dsp::ProcessContextReplacing<float>(audioBlock) );
    
    // 3. UTILITIES - MIX & OUTPUT GAIN
    // outputClipper.applyTanhClipper(buffer);
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
