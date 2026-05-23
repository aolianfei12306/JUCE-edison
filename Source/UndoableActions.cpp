#include "UndoableActions.h"
#include "AudioFileManager.h"

//==============================================================================
AudioModifyAction::AudioModifyAction(AudioFileManager& mgr,
                                     int startSample,
                                     int numSamples,
                                     Processor proc)
    : m_manager(mgr),
      m_startSample(startSample),
      m_numSamples(numSamples),
      m_processor(std::move(proc))
{
    jassert(startSample >= 0 && numSamples > 0);
    saveOriginalSamples();
}

//==============================================================================
void AudioModifyAction::saveOriginalSamples()
{
    auto* buf = m_manager.getBuffer();
    if (buf == nullptr)
        return;

    auto numChannels = buf->getNumChannels();

    // Clamp to buffer bounds
    if (m_startSample + m_numSamples > buf->getNumSamples())
        m_numSamples = buf->getNumSamples() - m_startSample;

    if (m_numSamples <= 0 || numChannels <= 0)
        return;

    m_originalSamples = juce::AudioBuffer<float>(numChannels, m_numSamples);
    m_originalSamples.clear();

    for (int ch = 0; ch < numChannels; ++ch)
        m_originalSamples.copyFrom(ch, 0, *buf, ch, m_startSample, m_numSamples);
}

//==============================================================================
bool AudioModifyAction::perform()
{
    auto* buf = m_manager.getBuffer();
    if (buf == nullptr)
        return false;

    // If original samples weren't saved, nothing to do
    if (m_originalSamples.getNumSamples() == 0)
        return false;

    // Ensure we don't exceed buffer bounds
    int safeNum = std::min(m_numSamples,
                           buf->getNumSamples() - m_startSample);
    if (safeNum <= 0)
        return false;

    m_processor(*buf, m_startSample, safeNum);

    if (onAudioChanged)
        onAudioChanged();

    return true;
}

//==============================================================================
bool AudioModifyAction::undo()
{
    auto* buf = m_manager.getBuffer();
    if (buf == nullptr)
        return false;

    if (m_originalSamples.getNumSamples() == 0)
        return false;

    int safeNum = std::min(m_originalSamples.getNumSamples(),
                           buf->getNumSamples() - m_startSample);
    if (safeNum <= 0)
        return false;

    int numChannels = std::min(buf->getNumChannels(),
                               m_originalSamples.getNumChannels());

    for (int ch = 0; ch < numChannels; ++ch)
        buf->copyFrom(ch, m_startSample, m_originalSamples, ch, 0, safeNum);

    if (onAudioChanged)
        onAudioChanged();

    return true;
}

//==============================================================================
BufferSizeChangeAction::BufferSizeChangeAction(AudioFileManager& mgr,
                                                int startSample,
                                                int numSamples,
                                                Mode mode)
    : m_manager(mgr),
      m_startSample(startSample),
      m_numSamples(numSamples),
      m_mode(mode)
{
    jassert(startSample >= 0 && numSamples > 0);
}

bool BufferSizeChangeAction::perform()
{
    auto* buf = m_manager.getBuffer();
    if (buf == nullptr || buf->getNumSamples() == 0)
        return false;

    // Save full buffer for undo before modifying
    const int numCh = buf->getNumChannels();
    const int numSamp = buf->getNumSamples();
    m_savedBuffer = std::make_unique<juce::AudioBuffer<float>>(numCh, numSamp);
    for (int ch = 0; ch < numCh; ++ch)
        m_savedBuffer->copyFrom(ch, 0, *buf, ch, 0, numSamp);

    if (m_mode == Mode::RIPPLE_DELETE)
    {
        m_manager.removeRange(m_startSample, m_numSamples);
    }
    else if (m_mode == Mode::CROP)
    {
        // Keep only start..start+num, remove everything else
        const int totalSamp = buf->getNumSamples();
        const int endSample = juce::jmin(m_startSample + m_numSamples, totalSamp);

        // First remove the tail after the selection (right side)
        const int samplesAfter = totalSamp - endSample;
        if (samplesAfter > 0)
            m_manager.removeRange(endSample, samplesAfter);

        // Then remove the prefix before the selection (left side)
        if (m_startSample > 0)
            m_manager.removeRange(0, m_startSample);
    }

    if (onAudioChanged)
        onAudioChanged();

    return true;
}

bool BufferSizeChangeAction::undo()
{
    if (m_savedBuffer == nullptr)
        return false;

    m_manager.replaceBuffer(std::move(m_savedBuffer));

    if (onAudioChanged)
        onAudioChanged();

    return true;
}
