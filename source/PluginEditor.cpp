#include "PluginEditor.h"

PluginEditor::PluginEditor (BassPreampProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p)
{
    juce::ignoreUnused (processorRef);
    juce::LookAndFeel::setDefaultLookAndFeel(&myCustomLnF);
    
    // Images
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::BassPreampBackground_png, BinaryData::BassPreampBackground_pngSize);
    
    // --- LAYOUT ---
    header.setEnabled(false);
    header.setAlpha(0.f);
    // header.setButtonText ("Header");
    addAndMakeVisible (header);
    
    sidebarLeft.setEnabled(false);
    sidebarLeft.setAlpha(0.f);
    // sidebarLeft.setButtonText ("Sidebar L");
    addAndMakeVisible (sidebarLeft);
    
    sidebarRight.setEnabled(false);
    sidebarRight.setAlpha(0.f);
    // sidebarRight.setButtonText ("Sidebar R");
    addAndMakeVisible (sidebarRight);
    
    preampContainer.setEnabled(false);
    preampContainer.setAlpha(0.f);
    // lifterContainer.setButtonText("Preamp");
    addAndMakeVisible (preampContainer);
    
    // --- LEVEL METERS ---
    // adjust the colours to how you like them, e.g.
    lnf.setColour (foleys::LevelMeter::lmMeterGradientLowColour, juce::Colours::whitesmoke);
    
    inputMeter.setLookAndFeel (&lnf);
    outputMeter.setLookAndFeel (&lnf);
    
    inputMeter.setMeterSource (&processorRef.getInputMeter());
    outputMeter.setMeterSource (&processorRef.getOutputMeter());
    
    addAndMakeVisible (inputMeter);
    addAndMakeVisible (outputMeter);
    
    // --- UTILITIES PARAMETERS ---
    // Input knob
    inputSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    inputSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    inputSlider.setRange(Parameters::inMin, Parameters::inMax, 0.1);
    inputSlider.setValue(Parameters::inDefault);
    inputSlider.setName("In");
    inputSlider.setColour(juce::Slider::thumbColourId,          juce::Colours::whitesmoke);
    inputSlider.setColour(juce::Slider::trackColourId,          juce::Colours::darkgrey);
    inputSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::whitesmoke);
    inputSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
    inputSlider.setColour(juce::Slider::textBoxTextColourId,    juce::Colours::whitesmoke);
    addAndMakeVisible(inputSlider);
    
    inputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::inId, inputSlider);
    
    // Gate knob
    gateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    gateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    gateSlider.setRange(Parameters::gateMin, Parameters::gateMax, 0.1);
    gateSlider.setValue(Parameters::gateDefault);
    gateSlider.setName("Gate");
    gateSlider.setColour(juce::Slider::thumbColourId,          juce::Colours::whitesmoke);
    gateSlider.setColour(juce::Slider::trackColourId,          juce::Colours::darkgrey);
    gateSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::whitesmoke);
    gateSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
    gateSlider.setColour(juce::Slider::textBoxTextColourId,    juce::Colours::whitesmoke);
    addAndMakeVisible(gateSlider);
    
    gateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::gateId, gateSlider);
    
    // Mix knob
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mixSlider.setRange(Parameters::mixMin, Parameters::mixMax, 1.0);
    mixSlider.setValue(Parameters::mixDefault);
    mixSlider.setName("Mix");
    mixSlider.setColour(juce::Slider::thumbColourId,          juce::Colours::whitesmoke);
    mixSlider.setColour(juce::Slider::trackColourId,          juce::Colours::darkgrey);
    mixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::whitesmoke);
    mixSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
    mixSlider.setColour(juce::Slider::textBoxTextColourId,    juce::Colours::whitesmoke);
    addAndMakeVisible(mixSlider);
    
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::mixId, mixSlider);
    
    // Output knob
    outputSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    outputSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    outputSlider.setRange(Parameters::outMin, Parameters::outMax, 0.1);
    outputSlider.setValue(Parameters::outDefault);
    outputSlider.setName("Out");
    outputSlider.setColour(juce::Slider::thumbColourId,          juce::Colours::whitesmoke);
    outputSlider.setColour(juce::Slider::trackColourId,          juce::Colours::darkgrey);
    outputSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::whitesmoke);
    outputSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
    outputSlider.setColour(juce::Slider::textBoxTextColourId,    juce::Colours::whitesmoke);
    addAndMakeVisible(outputSlider);
    
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::outId, outputSlider);

    // --- PREAMP PARAMETERS ---
    // Character knob
    characterSlider.setLookAndFeel(&imageLnF);
    characterSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    characterSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    characterSlider.setRange(Parameters::characterMin, Parameters::characterMax, 0.1);
    characterSlider.setValue(Parameters::characterDefault);
    addAndMakeVisible(characterSlider);
    
    characterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::characterId, characterSlider);

    // Drive knob
    driveSlider.setLookAndFeel(&imageLnF);
    driveSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    driveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    driveSlider.setRange(Parameters::driveMin, Parameters::driveMax, 0.1);
    driveSlider.setValue(Parameters::driveDefault);
    addAndMakeVisible(driveSlider);
    
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::driveId, driveSlider);

    // Low Comp / Deep knob
    lowCompSlider.setLookAndFeel(&imageLnF);
    lowCompSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lowCompSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lowCompSlider.setRange(Parameters::lowCompMin, Parameters::lowCompMax, 0.1);
    lowCompSlider.setValue(Parameters::lowCompDefault);
    addAndMakeVisible(lowCompSlider);
    
    lowCompAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::lowCompId, lowCompSlider);
    
    // Hi Comp / Bite knob
    hiCompSlider.setLookAndFeel(&imageLnF);
    hiCompSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    hiCompSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    hiCompSlider.setRange(Parameters::hiCompMin, Parameters::hiCompMax, 0.01);
    hiCompSlider.setValue(Parameters::hiCompDefault);
    addAndMakeVisible(hiCompSlider);
    
    hiCompAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::hiCompId, hiCompSlider);

    // Bass knob
    bassSlider.setLookAndFeel(&imageLnF);
    bassSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    bassSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    bassSlider.setRange(Parameters::bassMin, Parameters::bassMax, 0.1);
    bassSlider.setValue(Parameters::bassDefault);
    addAndMakeVisible(bassSlider);
    
    bassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::bassId, bassSlider);
    
    // Treble knob
    trebleSlider.setLookAndFeel(&imageLnF);
    trebleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    trebleSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    trebleSlider.setRange(Parameters::trebleMin, Parameters::trebleMax, 0.1);
    trebleSlider.setValue(Parameters::trebleDefault);
    addAndMakeVisible(trebleSlider);
    
    trebleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::trebleId, trebleSlider);
    
    // Lo-cut knob
    locutSlider.setLookAndFeel(&imageLnF);
    locutSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    locutSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    locutSlider.setRange(Parameters::locutMin, Parameters::locutMax, 0.1);
    locutSlider.setValue(Parameters::locutDefault);
    addAndMakeVisible(locutSlider);
    
    locutAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(processorRef.apvts, Parameters::locutId, locutSlider);
    
    // Sizing
    setResizable(true, true);
    getConstrainer()->setFixedAspectRatio(1.51);
    setResizeLimits(800, 530, 1600, 1060);
    setSize (800, 530);
}

PluginEditor::~PluginEditor()
{
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
    inputMeter.setLookAndFeel (nullptr);
    outputMeter.setLookAndFeel (nullptr);
}

void PluginEditor::paint (juce::Graphics& g)
{
    g.drawImageWithin(backgroundImage, 0, 0, getWidth(), getHeight(), juce::RectanglePlacement::stretchToFit);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    auto scaleFactor = area.getHeight() / 530.0;
    int scaledMargin = static_cast<int>(50.0 * scaleFactor);
    
    // --- LAYOUT SETUP ---
    auto headerArea = area.removeFromTop( scaledMargin );
    auto sideLArea = area.removeFromLeft( scaledMargin );
    auto sideRArea = area.removeFromRight( scaledMargin );
    
    header.setBounds(headerArea);
    sidebarLeft.setBounds(sideLArea);
    sidebarRight.setBounds(sideRArea);
    preampContainer.setBounds (area);
    
    // LEVEL METERS
    inputMeter.setBounds(sideLArea.reduced(6, 2));
    outputMeter.setBounds(sideRArea.reduced(6, 2));
    
    // --- HEADER CONTROLS ---
    inputSlider.setBounds( headerArea.removeFromLeft(headerArea.getHeight() ) );
    
    gateSlider.setBounds( headerArea.removeFromLeft(headerArea.getHeight() ) );

    outputSlider.setBounds( headerArea.removeFromRight(headerArea.getHeight() ) );
    
    mixSlider.setBounds( headerArea.removeFromRight(headerArea.getHeight() ) );
        
    // --- PREAMP CONTROLS ---
    // Define knob positions at minimum size (center coordinates)
    struct KnobLayout {
        juce::Component* knob;
        int centerX;
        int centerY;
        int size;
    };
    
    std::vector<KnobLayout> knobs = {
        { &characterSlider, 129 + 38, 275 + 38, 84 },
        { &driveSlider, 223 + 38, 401 + 36, 84 },
        { &lowCompSlider, 316 + 38, 275 + 38, 84 },
        { &hiCompSlider, 410 + 38, 401 + 36, 84 },
        { &bassSlider, 504 + 38, 275 + 38, 84 },
        { &trebleSlider, 597 + 38, 401 + 36, 84 },
        { &locutSlider, 665 + 19, 285 + 19, 49 }
    };
    
    // Apply scaling to each knob
    for (auto& knobLayout : knobs)
    {
        int scaledCenterX = static_cast<int>(knobLayout.centerX * scaleFactor);
        int scaledCenterY = static_cast<int>(knobLayout.centerY * scaleFactor);
        int scaledSize = static_cast<int>(knobLayout.size * scaleFactor);
        
        // Calculate bounds from center position
        int x = scaledCenterX - scaledSize / 2;
        int y = scaledCenterY - scaledSize / 2;
        
        knobLayout.knob->setBounds(x, y, scaledSize, scaledSize);
    }
}
