#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (BassPreampProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BassPreampProcessor& processorRef;
    
    // Custom Look and Feel
    punk_dsp::ExamplesLnF myCustomLnF;
    
    // Background
    juce::Image backgroundImage;
    
    // Layout utilities
    juce::TextButton header, sidebarLeft, sidebarRight, preampContainer;
    
    // Sliders - Rotary knobs
    juce::Slider inputSlider, gateSlider, mixSlider, outputSlider;
    juce::Slider characterSlider, driveSlider, compSlider, pumpSlider, bassSlider, trebleSlider, locutSlider;
    
    // Attachments for linking sliders-parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputAttachment, gateAttachment, mixAttachment, outputAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> characterAttachment, driveAttachment, compAttachment, pumpAttachment, bassAttachment, trebleAttachment, locutAttachment;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
