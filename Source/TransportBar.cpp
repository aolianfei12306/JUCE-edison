#include "TransportBar.h"
#include "AudioFileManager.h"
#include "SelectionManager.h"
#include "LoopManager.h"
#include "GridManager.h"

TransportIconButton::TransportIconButton(const juce::String& name, Icon icon)
    : juce::Button(name), m_icon(icon)
{
    setWantsKeyboardFocus(false);
    setTooltip(name);
}

void TransportIconButton::setIcon(Icon icon) noexcept
{
    if (m_icon == icon)
        return;

    m_icon = icon;
    repaint();
}

void TransportIconButton::paintButton(juce::Graphics& g, bool isMouseOverButton, bool isButtonDown)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    auto base = juce::Colour(0xFF303052);

    if (isButtonDown)
        base = base.brighter(0.20f);
    else if (isMouseOverButton)
        base = base.brighter(0.12f);

    if (!isEnabled())
        base = base.withMultipliedSaturation(0.35f).withMultipliedBrightness(0.7f);

    g.setColour(base);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(juce::Colour(0x663F8CFF));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    auto iconArea = bounds.reduced(8.0f);
    auto iconColour = (m_icon == Icon::Record)
        ? juce::Colour(0xFFFF5656)
        : juce::Colour(0xFFE8F0FF);

    if (!isEnabled())
        iconColour = iconColour.withAlpha(0.35f);

    g.setColour(iconColour);

    switch (m_icon)
    {
    case Icon::Play:
    {
        juce::Path play;
        play.addTriangle(iconArea.getX() + 1.0f, iconArea.getY(),
                         iconArea.getX() + 1.0f, iconArea.getBottom(),
                         iconArea.getRight(), iconArea.getCentreY());
        g.fillPath(play);
        break;
    }
    case Icon::Pause:
    {
        const auto barWidth = juce::jmax(3.0f, iconArea.getWidth() * 0.28f);
        g.fillRoundedRectangle(iconArea.withWidth(barWidth), 1.0f);
        g.fillRoundedRectangle(iconArea.withX(iconArea.getRight() - barWidth).withWidth(barWidth), 1.0f);
        break;
    }
    case Icon::Stop:
    case Icon::RecordStop:
    {
        g.fillRoundedRectangle(iconArea.reduced(1.5f), 1.5f);
        break;
    }
    case Icon::Record:
    {
        g.fillEllipse(iconArea.reduced(1.0f));
        break;
    }
    }
}

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

    m_progress.setRange(0.0, 1.0, 0.0);
    m_progress.setValue(0.0, juce::dontSendNotification);

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
    constexpr int infoWidth = 380;

    auto r = getLocalBounds().reduced(4, 2);
    int bw = 30;
    m_playBtn.setBounds  (r.removeFromLeft(bw).reduced(2));
    m_pauseBtn.setBounds (r.removeFromLeft(bw).reduced(2));
    m_stopBtn.setBounds  (r.removeFromLeft(bw).reduced(2));
    m_recordBtn.setBounds(r.removeFromLeft(bw).reduced(2));
    r.removeFromLeft(4);

    // Reserve 200px for selection info text (left of progress bar, after buttons)
    r.removeFromRight(juce::jmin(infoWidth, r.getWidth()));
    r.removeFromLeft(200);
    m_progress.setBounds(r);
}

void TransportBar::updateButtonStates()
{
    bool hasAudio = m_fileManager.hasAudio();
    m_playBtn.setEnabled(hasAudio);
    m_pauseBtn.setEnabled(m_state == State::Playing);
    m_stopBtn.setEnabled(m_state != State::Stopped);
    m_progress.setEnabled(hasAudio);
    m_recordBtn.setIcon(isRecording() ? TransportIconButton::Icon::RecordStop
                                      : TransportIconButton::Icon::Record);
}

void TransportBar::drawSelectionInfo(juce::Graphics& g, const juce::Rectangle<int>& area)
{
    auto r = area;

    if (m_selection.hasSelection())
    {
        auto start = m_selection.getSelectionStart();
        auto end   = m_selection.getSelectionEnd();
        auto dur   = m_selection.getSelectionDuration();

        auto fmt = [](double sec) -> juce::String {
            int m = static_cast<int>(sec) / 60;
            double s = sec - m * 60;
            return juce::String::formatted("%02d:%06.3f", m, s);
        };

        // Draw selection time on left half
        g.setColour(juce::Colour(0xCCE0E0E0));
        g.setFont(12.0f);
        auto selTextRect = r.removeFromLeft(r.getWidth() / 2);
        g.drawText("Sel: " + fmt(start) + " - " + fmt(end) + " (" + fmt(dur) + ")",
                   selTextRect, juce::Justification::centredLeft);
    }

    juce::StringArray snapModes;

    if (m_selection.isSnapToGrid())
    {
        if (m_gridManager != nullptr)
        {
            juce::String bpmText = juce::String(m_gridManager->getBPM(), 1);
            if (bpmText.endsWith(".0"))
                bpmText = bpmText.dropLastCharacters(2);
            snapModes.add("Grid " + bpmText + "/" + juce::String(m_gridManager->getDivision()));
        }
        else
        {
            snapModes.add("Grid");
        }
    }

    if (m_selection.isSnapToZero())
        snapModes.add("ZC");

    if (!snapModes.isEmpty())
    {
        // Draw snap info on right half
        g.setColour(juce::Colour(0xCCFFCC44));
        g.setFont(12.0f);
        auto snapArea = area;
        auto snapRect = snapArea.removeFromRight(snapArea.getWidth() / 2);
        g.drawText("  [Snap: " + snapModes.joinIntoString(", ") + "]",
                   snapRect, juce::Justification::centredRight);
    }
}

void TransportBar::paint(juce::Graphics& g)
{
    constexpr int infoWidth = 380;

    g.fillAll(juce::Colour(0xFF222244));

    auto fmt = [](double sec) -> juce::String {
        int m = static_cast<int>(sec) / 60;
        double s = sec - m * 60;
        return juce::String::formatted("%02d:%06.3f", m, s);
    };

    // Layout: [buttons x=0..124] [selection info x=132..372] [progress bar] [time/file info x=right-380..right]
    // Compute the middle area: between buttons and right info panel
    auto bounds = getLocalBounds();

    // Right panel: time display + file info
    auto rightPanel = bounds.removeFromRight(juce::jmin(infoWidth, bounds.getWidth()));

    // Draw time display (Row 1 of right panel)
    double total = m_fileManager.hasAudio() ? m_fileManager.getDurationSec() : 0.0;
    g.setColour(juce::Colour(0xCCE0E0E0));
    g.setFont(12.0f);
    g.drawText(fmt(m_position) + " / " + fmt(total), rightPanel.removeFromTop(16), juce::Justification::centredLeft);

    // Draw file info (Row 2 of right panel)
    if (m_fileManager.hasAudio()) {
        juce::String info;
        info += juce::String(m_fileManager.getSampleRate(), 0) + " Hz";
        info += " / " + juce::String(m_fileManager.getBitsPerSample()) + "-bit";
        info += " / " + juce::String(m_fileManager.getNumChannels()) + "ch";
        info += " / " + m_fileManager.getFileName();

        g.setColour(juce::Colour(0x99A0A0B0));
        g.setFont(10.0f);
        g.drawText(info, rightPanel, juce::Justification::centredLeft);
    }

    // Selection info: draw directly in the dedicated 200px area between buttons and progress bar
    // Buttons occupy ~124px (4 × 30 + 4 gap + 2×2 inset padding)
    // Selection info: matches resized() layout
    // Buttons occupy 4×30 + 4 gap + 2×2 inset from reduced(4,2) = ~128px from left edge
    constexpr int buttonAreaEnd = 128;
    constexpr int selInfoWidth = 200;
    auto selArea = getLocalBounds().withTrimmedLeft(buttonAreaEnd)
                                    .withWidth(selInfoWidth);
    if (selArea.getWidth() > 0)
        drawSelectionInfo(g, selArea);
}

void TransportBar::play()
{
    if (!m_fileManager.hasAudio()) return;
    m_state = State::Playing;

    // If there's a selection and we're at the start or end,
    // start from the selection's beginning like playSelection does.
    // If user paused mid-playback, resume from current position.
    if (m_selection.hasSelection() && m_position <= 0.001)
        setPosition(m_selection.getSelectionStart());
    else if (m_position >= m_fileManager.getDurationSec())
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
    if (total <= 0.0) return;

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
    if (src == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        m_state.store(State::Stopped);
        juce::MessageManager::callAsync([this] { stop(); });
        return;
    }

    int numSamples   = bufferToFill.numSamples;
    int totalSamples = src->getNumSamples();
    int numCh        = juce::jmin(bufferToFill.buffer->getNumChannels(), src->getNumChannels());

    bool shouldLoop = (m_loopManager != nullptr && m_loopManager->hasValidLoop());
    bool hasSelection = (!shouldLoop && m_selection.hasSelection());

    // Pre-compute loop bounds to avoid manager calls in the inner loop
    int loopStartSample = 0;
    int loopEndSample   = totalSamples;
    int selectionEndSample = totalSamples;
    if (shouldLoop)
    {
        loopStartSample = static_cast<int>(m_loopManager->getLoopStart() * m_sampleRate);
        loopStartSample = juce::jlimit(0, totalSamples - 1, loopStartSample);
        loopEndSample = static_cast<int>(m_loopManager->getLoopEnd() * m_sampleRate);
        loopEndSample = juce::jmin(loopEndSample, totalSamples);
    }
    else if (hasSelection)
    {
        selectionEndSample = static_cast<int>(m_selection.getSelectionEnd() * m_sampleRate);
        selectionEndSample = juce::jmin(selectionEndSample, totalSamples);
    }

    for (int s = 0; s < numSamples; ++s)
    {
        if (m_readIndex >= totalSamples)
        {
            if (shouldLoop)
            {
                m_readIndex = loopStartSample;
            }
            else
            {
                m_state.store(State::Stopped);
                juce::MessageManager::callAsync([this] { stop(); });
                return;
            }
        }

        if (shouldLoop)
        {
            if (m_readIndex >= loopEndSample)
                m_readIndex = loopStartSample;
        }
        else if (hasSelection && m_readIndex >= selectionEndSample)
        {
            m_state.store(State::Stopped);
            juce::MessageManager::callAsync([this] { stop(); });
            return;
        }

        for (int ch = 0; ch < numCh; ++ch)
            bufferToFill.buffer->setSample(ch, s, src->getSample(ch, m_readIndex));
        for (int ch = numCh; ch < bufferToFill.buffer->getNumChannels(); ++ch)
            bufferToFill.buffer->clear(ch, s, 1);
        ++m_readIndex;
    }

    double newPos = static_cast<double>(m_readIndex.load()) / m_sampleRate;

    juce::MessageManager::callAsync([this, newPos] {
        if (m_state != State::Playing) return;
        m_selection.setPlaybackPosition(newPos);
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
