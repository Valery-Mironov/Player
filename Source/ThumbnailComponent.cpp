#include "ThumbnailComponent.h"

ThumbnailComponent::ThumbnailComponent(
    int sourceSamplesPerThumbnailSample,
    juce::AudioFormatManager &audioFormatManager,
    juce::AudioThumbnailCache &audioTumbnailChache) :
        audioThumbnail(sourceSamplesPerThumbnailSample, audioFormatManager, audioTumbnailChache) {
    audioThumbnail.addChangeListener(this);
}


// ====================================================================================================
void ThumbnailComponent::setFile(const juce::File &file) {
    audioThumbnail.setSource(new juce::FileInputSource(file));
}


// ====================================================================================================
void ThumbnailComponent::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &audioThumbnail) {
        audioThumbnailChanged();
    }
}


void ThumbnailComponent::paint (juce::Graphics &g) {
    if (audioThumbnail.getNumChannels() == 0) {
        paintIfNoFileLoaded(g);
    } else {
        paintIfFileLoaded(g);
    }
}



void ThumbnailComponent::resized() {
    
}


// ====================================================================================================
void ThumbnailComponent::audioThumbnailChanged() {
    repaint();
}



void ThumbnailComponent::paintIfNoFileLoaded(juce::Graphics &g) {
    g.fillAll(backgroundColor);
    g.setColour(textColor);
    g.drawFittedText("No File Loaded", getLocalBounds(), juce::Justification::centred, 1);
}



void ThumbnailComponent::paintIfFileLoaded(juce::Graphics &g) {
    g.fillAll(backgroundColor);
    g.setColour(waveColor);
    audioThumbnail.drawChannels(g, getLocalBounds(), 0.0, audioThumbnail.getTotalLength(), 1.0);
}
