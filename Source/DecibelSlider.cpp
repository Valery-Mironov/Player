#include "DecibelSlider.h"

DecibelSlider::DecibelSlider() {
    
}


// ====================================================================================================
double DecibelSlider::getValueFromText(const juce::String &text) {
    auto minusInfinitydB = DB_MINIMUM;
    auto decibelText = text.upToFirstOccurrenceOf("dB", false, false).trim();
    return decibelText.equalsIgnoreCase("-INF") ? minusInfinitydB : decibelText.getDoubleValue();
}



juce::String DecibelSlider::getTextFromValue(double value) {
    if (value <= DB_MINIMUM) {
        return "-INF dB";
    } else {
        return juce::Decibels::toString(value);
    }
}
