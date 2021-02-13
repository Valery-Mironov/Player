#pragma once

#include <JuceHeader.h>

const float DB_MINIMUM = -32.0f;

class DecibelSlider  : public juce::Slider {
public:
    DecibelSlider();
    
    double getValueFromText(const juce::String &text) override;
    juce::String getTextFromValue(double value) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DecibelSlider)
};
