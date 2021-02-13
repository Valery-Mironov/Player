#include "PositionOverlay.h"

PositionOverlay::PositionOverlay(
    juce::AudioTransportSource &audioTransportSourceToUse) :
    audioTransportSource(
        audioTransportSourceToUse) {
    startTimer(50);
}


// ====================================================================================================
void PositionOverlay::mouseDown(const juce::MouseEvent &event) {
    auto duration = static_cast<double>(audioTransportSource.getLengthInSeconds());
    
    if (0.0 < duration) {
        auto clickPosition = event.position.x;
        auto newPosition = clickPosition / static_cast<double>(getWidth()) * duration;
        
        audioTransportSource.setPosition(newPosition);
    }
}



void PositionOverlay::paint(juce::Graphics &g) {
    auto duration = static_cast<double>(audioTransportSource.getLengthInSeconds());
    
    if (0.0 < duration) {
        auto currentPosition = static_cast<double>(audioTransportSource.getCurrentPosition());
        auto drawPosition = (currentPosition / duration) * static_cast<double>(getWidth());
        
        g.setColour(lineColor);
        g.drawLine(drawPosition, 0.0, drawPosition, static_cast<double>(getHeight()), 2.0);
    }
}



void PositionOverlay::resized() {
    
}


// ====================================================================================================
void PositionOverlay::timerCallback() {
    repaint();
}
