#include "MainComponent.h"

MainComponent::MainComponent() :
    transportState(Stopped),
    audioThumbnailCache(5),
    thumbnailComponent(512, audioFormatManager, audioThumbnailCache),
    positionOverlay(audioTransportSource) {
    
    addAndMakeVisible(fileNameLabel);
    fileNameLabel.setText("No file", juce::dontSendNotification);
    
    addAndMakeVisible(currentPositionLabel);
    currentPositionLabel.setJustificationType(juce::Justification::centredRight);
        
    addAndMakeVisible(&thumbnailComponent);
    addAndMakeVisible(&positionOverlay);
    
    addAndMakeVisible(&playButton);
    playButton.setButtonText("Play");
    playButton.onClick = [this] {
        playButtonClicked();
    };
    
    addAndMakeVisible(&stopButton);
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] {
        stopButtonClicked();
    };
    
    addAndMakeVisible(&loopingToggle);
    loopingToggle.setButtonText("Loop");
    loopingToggle.onClick = [this] {
        loopButtonChanged();
    };
    
    addAndMakeVisible(&openButton);
    openButton.setButtonText("Open");
    openButton.onClick = [this] {
        openButtonClicked();
    };
    
    addAndMakeVisible(&volumeSlider);
    volumeSlider.setRange(DB_MINIMUM, 0.0);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 80, 20);
    volumeSlider.onValueChange = [this] {
        if (auto volumeSliderValue = static_cast<double>(volumeSlider.getValue()); volumeSliderValue <= DB_MINIMUM) {
            targetLevel = 0.0;
        } else {
            targetLevel = juce::Decibels::decibelsToGain(volumeSliderValue);
        }
    };
    volumeSlider.setValue(-6.0);
    
    setSize(500, 200);
    
    audioFormatManager.registerBasicFormats();
    audioTransportSource.addChangeListener(this);
    
    // *******************************************************************************
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio) &&
        !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio)) {
        juce::RuntimePermissions::request(
            juce::RuntimePermissions::recordAudio,
            [&](bool granted) {
                setAudioChannels(granted ? 2 : 0, 2);
            });
    } else {
        setAudioChannels(0, 2);
    }
    // *******************************************************************************
    
    startTimer(100);
}



MainComponent::~MainComponent() {
    shutdownAudio();
}



// ====================================================================================================
void MainComponent::changeListenerCallback(juce::ChangeBroadcaster *source) {
    if (source == &audioTransportSource) {
        if (audioTransportSource.isPlaying())
            changeTransportState(Playing);
        else if ((transportState == Stopping) || (transportState == Playing))
            changeTransportState(Stopped);
        else if (Pausing == transportState)
            changeTransportState(Paused);
    }
}



void MainComponent::timerCallback() {
    juce::RelativeTime position(audioTransportSource.getCurrentPosition());

    auto minutes = (static_cast<int>(position.inMinutes())) % 60;
    auto seconds = (static_cast<int>(position.inSeconds())) % 60;
    auto millis  = (static_cast<int>(position.inMilliseconds())) % 1000;

    auto positionString = juce::String::formatted("%02d:%02d:%03d", minutes, seconds, millis);

    currentPositionLabel.setText(positionString, juce::dontSendNotification);
}



void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    audioTransportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}



void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
    if (audioFormatReaderSource.get() == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    
    audioTransportSource.getNextAudioBlock(bufferToFill);
            
    for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
        auto *buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample) {
            buffer[sample] *= currentLevel;
        }
    }
    
    auto localTargetLevel = targetLevel;
    bufferToFill.buffer->applyGainRamp(
        bufferToFill.startSample,
        bufferToFill.numSamples,
        currentLevel,
        localTargetLevel);
    currentLevel = localTargetLevel;
}



void MainComponent::releaseResources() {
    audioTransportSource.releaseResources();
}


// ====================================================================================================
void MainComponent::paint(juce::Graphics &g) {
    
}



void MainComponent::resized() {
    auto indent = 5;
    auto menuHight = 20;
    
    juce::Rectangle<int> thumbnailBounds(
        /* X */ indent,
        /* Y */ indent * 2 + menuHight,
        /* W */ getWidth() - (indent * 2),
        /* H */ getHeight() - ((menuHight * 2) + (indent * 4)));
    
    thumbnailComponent.setBounds(thumbnailBounds);
    positionOverlay.setBounds(thumbnailBounds);
        
    auto horizontalGrid = thumbnailBounds.getWidth() / 8;
    auto сontrolPanelPosition = thumbnailBounds.getHeight() + (indent * 3) + menuHight;
    
    fileNameLabel.setBounds(
        /* X */ indent,
        /* Y */ indent,
        /* W */ (horizontalGrid * 4) - indent,
        /* H */ menuHight);
    
    currentPositionLabel.setBounds(
        /* X */ indent + (horizontalGrid * 5),
        /* Y */ indent,
        /* W */ (horizontalGrid * 2) - indent,
        /* H */ menuHight);
    
    loopingToggle.setBounds(
        /* X */ indent + (horizontalGrid * 7),
        /* Y */ indent,
        /* W */ horizontalGrid,
        /* H */ menuHight);
    
    playButton.setBounds(
        /* X */ indent,
        /* Y */ сontrolPanelPosition,
        /* W */ horizontalGrid - indent,
        /* H */ menuHight);
    
    stopButton.setBounds(
        /* X */ indent + horizontalGrid,
        /* Y */ сontrolPanelPosition,
        /* W */ horizontalGrid - indent,
        /* H */ menuHight);
    
    volumeSlider.setBounds(
        /* X */ indent + (horizontalGrid * 2),
        /* Y */ сontrolPanelPosition,
        /* W */ horizontalGrid * 5,
        /* H */ menuHight);
    
    openButton.setBounds(
        /* X */ (indent * 2) + (horizontalGrid * 7),
        /* Y */ сontrolPanelPosition,
        /* W */ horizontalGrid - indent,
        /* H */ menuHight);
}


// ====================================================================================================
void MainComponent::changeTransportState(TransportState newTransportState) {
    if (newTransportState != transportState) {
        transportState = newTransportState;
        
        switch (transportState) {
            case Starting:
                audioTransportSource.start();
                break;
                
            case Playing:
                playButton.setButtonText("Pause");
                stopButton.setButtonText("Stop");
                stopButton.setEnabled(true);
                break;
        
            case Pausing:
                audioTransportSource.stop();
                break;
        
            case Paused:
                playButton.setButtonText("Play");
                stopButton.setButtonText("Reset");
                break;
                
            case Stopping:
                audioTransportSource.stop();
                break;
                
            case Stopped:
                playButton.setButtonText("Play");
                stopButton.setButtonText("Stop");
                stopButton.setEnabled(false);
                audioTransportSource.setPosition(0.0);
                break;
                
            default:
                jassertfalse;
                break;
        }
    }
}



void MainComponent::audioTransportSourceChanged() {
    if (audioTransportSource.isPlaying()) {
        changeTransportState(Playing);
    } else {
        changeTransportState(Stopped);
    }
}



void MainComponent::updateLoopState(bool shouldLoop) {
    if (audioFormatReaderSource.get() != nullptr) {
        audioFormatReaderSource->setLooping(shouldLoop);
    }
}



void MainComponent::playButtonClicked() {
    if ((transportState == Stopped) || (transportState == Paused))
        changeTransportState(Starting);
    else if (transportState == Playing)
        changeTransportState(Pausing);
}



void MainComponent::stopButtonClicked() {
    if (transportState == Paused)
        changeTransportState(Stopped);
    else
        changeTransportState(Stopping);
}



void MainComponent::loopButtonChanged() {
    updateLoopState(loopingToggle.getToggleState());
}



void MainComponent::openButtonClicked() {
    if (audioTransportSource.isPlaying()) {
        audioTransportSource.stop();
    }
    
    juce::FileChooser fileChooser("Select a Wave file to play...", {}, "*.wav;*.mp3");
    
    if (fileChooser.browseForFileToOpen()) {
        juce::File file = fileChooser.getResult();
        
        if (auto *reader = audioFormatManager.createReaderFor(file)) {
            fileNameLabel.setText(file.getFileName(), juce::dontSendNotification);
            
            std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
            audioTransportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            playButton.setEnabled(true);
            thumbnailComponent.setFile(file);
            audioFormatReaderSource.reset(newSource.release());
        }
    }
}
