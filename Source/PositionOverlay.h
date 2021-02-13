#pragma once

#include <JuceHeader.h>

class PositionOverlay :
    public juce::Component,
    private juce::Timer {
    
public:
    PositionOverlay(juce::AudioTransportSource &audioTransportSourceToUse);
    
    void mouseDown(const juce::MouseEvent &event) override;
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void timerCallback() override;
    
    // ====================================================================================================
    juce::Colour lineColor = juce::Colours::red;
    juce::AudioTransportSource &audioTransportSource;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionOverlay)
};
