#include "TransportBar.h"
#include "AudioFileManager.h"
#include "SelectionManager.h"

TransportBar::TransportBar(AudioFileManager& fileManager, SelectionManager& selection)
    : m_fileManager(fileManager), m_selection(selection)
{
    m_player = std::make_unique<juce::AudioSourcePlayer>();

    addAndMakeVisible(m_playBtn);
    addAndMakeVisible(m_pauseBtn);
    addAndMakeVisible(m_stopBtn);
    addAndMakeVisible(m_recordBtn);
    addAndMakeVisible(m_progress);

    m_playBtn.onClick   = [this] { play(); };
    m_pauseBtn.onClick  = [this] { pause(); };
    m_stopBtn.onClick   = [this] { stop(); };
    m_recordBtn.onClick = [this] {
        isRecording() ? stopRecording() : startRecording();
    };

    m_progress.onValueChange = [this] {
        if (m_fileManager.hasAudio())
            setPosition(m_progress.getValue() * m_fileManager.getDurationSec());
    };

    startTimerHz(30);
    updateButtonStates();
}

TransportBar::~TransportBar()
{
    stopTimer();
    stop();
}

void TransportBar::resized()
{
    auto r = getLocalBounds().reduced(4, 2);
    int bw = 30;
    m_playBtn.setBounds  (r.removeFromLeft(bw).reduced(2));
    m_pauseBtn.setBounds (r.removeFromLeft(bw).reduced(2));
    m_stopBtn.setBounds  (r.removeFromLeft(bw).reduced(2));
    m_recordBtn.setBounds(r.removeFromLeft(bw).reduced(2));
    r.removeFromLeft(4);
    m_progress.setBounds(r);
}

void TransportBar::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF222244));

    auto r = getLocalBounds().removeFromRight(180);
    auto fmt = [](double sec) -> juce::String {
        int m = static_cast<int>(sec) / 60;
        double s = sec - m * 60;
        return juce::String::formatted("%02d:%06.3f", m, s);
    };

    double total = m_fileManager.hasAudio() ? m_fileManager.getDurationSec() : 0.0;
    g.setColour(juce::Colour(0xCCE0E0E0));
    g.setFont(12.0f);
    g.drawText(fmt(m_position) + " / " + fmt(total), r, juce::Justification::centredLeft);
}

void TransportBar::updateButtonStates()
{
    m_playBtn.setEnabled(m_fileManager.hasAudio());
    m_pauseBtn.setEnabled(m_state == State::Playing);
    m_stopBtn.setEnabled(m_state != State::Stopped);
    m_recordBtn.setButtonText(isRecording() ? "⏹" : "⏺");
}

void TransportBar::play()
{
    if (!m_fileManager.hasAudio()) return;
    m_state = State::Playing;

    if (m_position >= m_fileManager.getDurationSec())
        setPosition(0.0);
    m_readIndex = static_cast<int>(m_position * m_sampleRate);

    if (m_deviceManager)
    {
        m_player->setSource(this);
        m_deviceManager->addAudioCallback(m_player.get());
    }
    updateButtonStates();
}

void TransportBar::playSelection()
{
    if (!m_fileManager.hasAudio() || !m_selection.hasSelection()) return;
    setPosition(m_selection.getSelectionStart());
    m_state = State::Playing;
    m_readIndex = static_cast<int>(m_position * m_sampleRate);
    if (m_deviceManager)
    {
        m_player->setSource(this);
        m_deviceManager->addAudioCallback(m_player.get());
    }
    updateButtonStates();
}

void TransportBar::pause()
{
    if (m_deviceManager) m_deviceManager->removeAudioCallback(m_player.get());
    m_state = State::Paused;
    updateButtonStates();
}

void TransportBar::stop()
{
    if (m_deviceManager) m_deviceManager->removeAudioCallback(m_player.get());
    m_state = State::Stopped;
    setPosition(0.0);
    updateButtonStates();
}

void TransportBar::setPosition(double posSec)
{
    if (!m_fileManager.hasAudio()) return;
    double total = m_fileManager.getDurationSec();
    m_position = juce::jlimit(0.0, total, posSec);
    m_readIndex = static_cast<int>(m_position * m_sampleRate);
    m_selection.setPlaybackPosition(m_position);
    m_progress.setValue(m_position / total, juce::dontSendNotification);
    if (onPositionChanged) onPositionChanged(m_position);
}

void TransportBar::prepareToPlay(int, double sampleRate)
{
    m_sampleRate = sampleRate;
}

void TransportBar::releaseResources() {}

void TransportBar::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (isRecording())
    {
        const juce::CriticalSection::ScopedLockType lock(m_recordingLock);
        if (m_recordedBuffer)
        {
            int ns = bufferToFill.numSamples;
            int oldSize = m_recordedBuffer->getNumSamples();
            m_recordedBuffer->setSize(m_recordedBuffer->getNumChannels(), oldSize + ns, true, true);
            for (int ch = 0; ch < m_recordedBuffer->getNumChannels(); ++ch)
                m_recordedBuffer->copyFrom(ch, oldSize, *bufferToFill.buffer, ch,
                                           bufferToFill.startSample, ns);
        }
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    if (m_state != State::Playing || !m_fileManager.hasAudio())
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    auto* src = m_fileManager.getBuffer();
    if (src == nullptr) { bufferToFill.clearActiveBufferRegion(); stop(); return; }

    int numSamples   = bufferToFill.numSamples;
    int totalSamples = src->getNumSamples();
    int numCh        = juce::jmin(bufferToFill.buffer->getNumChannels(), src->getNumChannels());

    for (int s = 0; s < numSamples; ++s)
    {
        if (m_readIndex >= totalSamples)
        {
            // Check selection end
            if (m_selection.hasSelection())
            {
                double pos = static_cast<double>(m_readIndex) / m_sampleRate;
                if (pos >= m_selection.getSelectionEnd()) { stop(); return; }
            }
            stop();
            return;
        }

        for (int ch = 0; ch < numCh; ++ch)
            bufferToFill.buffer->setSample(ch, s, src->getSample(ch, m_readIndex));
        for (int ch = numCh; ch < bufferToFill.buffer->getNumChannels(); ++ch)
            bufferToFill.buffer->clear(ch, s, 1);
        ++m_readIndex;
    }

    double newPos = static_cast<double>(m_readIndex) / m_sampleRate;
    m_selection.setPlaybackPosition(newPos);

    juce::MessageManager::callAsync([this, newPos] {
        m_position = newPos;
        if (m_fileManager.hasAudio())
            m_progress.setValue(m_position / m_fileManager.getDurationSec(),
                                juce::dontSendNotification);
        if (onPositionChanged) onPositionChanged(newPos);
    });
}

void TransportBar::timerCallback() { repaint(); }

void TransportBar::startRecording()
{
    if (!m_deviceManager) return;
    m_deviceManager->removeAudioCallback(m_player.get());

    m_recordedSampleRate = m_deviceManager->getCurrentAudioDevice()
        ? m_deviceManager->getCurrentAudioDevice()->getCurrentSampleRate() : 44100.0;

    {
        const juce::CriticalSection::ScopedLockType lock(m_recordingLock);
        int numInput = m_deviceManager->getCurrentAudioDevice()
            ? m_deviceManager->getCurrentAudioDevice()->getActiveInputChannels().countNumberOfSetBits()
            : 2;
        numInput = juce::jmax(1, numInput);
        m_recordedBuffer = std::make_unique<juce::AudioBuffer<float>>(numInput, 0);
        m_recordedChannels = numInput;
    }

    m_state = State::Recording;
    m_player->setSource(this);
    m_deviceManager->addAudioCallback(m_player.get());
    updateButtonStates();
}

void TransportBar::stopRecording()
{
    if (m_deviceManager) m_deviceManager->removeAudioCallback(m_player.get());
    m_state = State::Stopped;
    updateButtonStates();
    // Signal recording finished
    if (onPositionChanged) onPositionChanged(-1.0);
}
