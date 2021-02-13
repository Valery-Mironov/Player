#pragma once

#include <JuceHeader.h>

#include "ThumbnailComponent.h"
#include "PositionOverlay.h"
#include "DecibelSlider.h"

class MainComponent :
    public juce::AudioAppComponent,
    public juce::ChangeListener,
    public juce::Timer {
    
public:
    MainComponent();
    ~MainComponent() override;
    
    void changeListenerCallback(juce::ChangeBroadcaster *source) override;
    void timerCallback() override;
    
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override;
    void releaseResources() override;
    
    void paint(juce::Graphics &g) override;
    void resized() override;
    
private:
    enum TransportState {
        Starting,
        Playing,
        Pausing,
        Paused,
        Stopping,
        Stopped
    };
    
    void changeTransportState(TransportState newTransportState);
    void audioTransportSourceChanged();
    void updateLoopState(bool shouldLoop);
    
    void playButtonClicked();
    void stopButtonClicked();
    void loopButtonChanged();
    void openButtonClicked();
    
    // ====================================================================================================
    juce::Label fileNameLabel;
    juce::Label currentPositionLabel;
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::ToggleButton loopingToggle;
    DecibelSlider volumeSlider;
    juce::TextButton openButton;
    
    juce::AudioFormatManager audioFormatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> audioFormatReaderSource;
    juce::AudioTransportSource audioTransportSource;
    
    TransportState transportState;
    
    // Wave display
    juce::AudioThumbnailCache audioThumbnailCache;
    ThumbnailComponent thumbnailComponent;
    PositionOverlay positionOverlay;
    
    double currentLevel = 0.0;
    double targetLevel = 0.0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
