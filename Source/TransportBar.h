#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>

class AudioFileManager;
class SelectionManager;

class TransportBar : public juce::Component,
                     public juce::AudioSource,
                     public juce::Timer
{
public:
    TransportBar(AudioFileManager& fileManager, SelectionManager& selection);
    ~TransportBar() override;

    enum class State { Stopped, Playing, Paused, Recording };

    // AudioSource
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Timer
    void timerCallback() override;

    void play();
    void playSelection();
    void pause();
    void stop();
    void startRecording();
    void stopRecording();

    bool isRecording() const noexcept { return m_state == State::Recording; }
    bool isPlaying()   const noexcept { return m_state == State::Playing; }
    State getState()   const noexcept { return m_state; }

    void setAudioDeviceManager(juce::AudioDeviceManager* dm) { m_deviceManager = dm; }
    void setPosition(double posSec);
    double getPosition() const noexcept { return m_position; }

    std::function<void(double)> onPositionChanged;

    juce::AudioBuffer<float>* getRecordedBuffer() const noexcept { return m_recordedBuffer.get(); }
    double getRecordedSampleRate() const noexcept { return m_recordedSampleRate; }

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    AudioFileManager& m_fileManager;
    SelectionManager& m_selection;
    juce::AudioDeviceManager* m_deviceManager = nullptr;
    std::unique_ptr<juce::AudioSourcePlayer> m_player;

    State  m_state      = State::Stopped;
    double m_position   = 0.0;
    double m_sampleRate = 44100.0;
    int    m_readIndex  = 0;

    std::unique_ptr<juce::AudioBuffer<float>> m_recordedBuffer;
    double m_recordedSampleRate   = 44100.0;
    int    m_recordedChannels     = 0;
    juce::CriticalSection m_recordingLock;

    juce::TextButton m_playBtn   { "▶" };
    juce::TextButton m_pauseBtn  { "⏸" };
    juce::TextButton m_stopBtn   { "■" };
    juce::TextButton m_recordBtn { "⏺" };
    juce::Slider     m_progress  { juce::Slider::LinearBar, juce::Slider::NoTextBox };

    void updateButtonStates();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TransportBar)
};
