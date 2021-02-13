#pragma once

#include <JuceHeader.h>

class ThumbnailComponent :
    public juce::Component,
    public juce::ChangeListener {
    
public:
    ThumbnailComponent(
        int sourceSamplesPerThumbnailSample,
        juce::AudioFormatManager &audioFormatManager,
        juce::AudioThumbnailCache &audioTumbnailChache);
    
    void setFile(const juce::File &file);
    
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    void paint(juce::Graphics &) override;
    void resized() override;

private:
    void audioThumbnailChanged();
    void paintIfNoFileLoaded(juce::Graphics &g);
    void paintIfFileLoaded(juce::Graphics &g);
    
    // ====================================================================================================
    juce::Colour backgroundColor = juce::Colours::white;
    juce::Colour waveColor = juce::Colours::darkgrey;
    juce::Colour textColor = juce::Colours::red;
    
    juce::AudioThumbnail audioThumbnail;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ThumbnailComponent)
};
