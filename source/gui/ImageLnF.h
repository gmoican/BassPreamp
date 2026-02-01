#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class ImageLnF : public juce::LookAndFeel_V4
{
public:
    ImageLnF()
    {
        // Load your knob image
        knobImage = juce::ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize);
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override
    {
        juce::ignoreUnused(slider);
        
        // Calculate rotation based on slider position
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // Calculate the center point
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        
        // Scale the image to fit the slider bounds
        const float scaleFactor = juce::jmin(width / (float)knobImage.getWidth(),
                                             height / (float)knobImage.getHeight());
        
        // Draw the rotated image
        juce::AffineTransform transform = juce::AffineTransform::translation(-knobImage.getWidth() * 0.5f, -knobImage.getHeight() * 0.5f)
                                          .scaled(scaleFactor)
                                          .rotated(angle)
                                          .translated(centerX, centerY);
        
        g.drawImageTransformed(knobImage, transform);
    }
    
private:
    juce::Image knobImage;
};
