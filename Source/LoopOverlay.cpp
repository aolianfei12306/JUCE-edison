#include "LoopOverlay.h"
#include "AudioFileManager.h"
#include "WaveformThumbnail.h"

LoopOverlay::LoopOverlay(LoopManager& mgr, AudioFileManager& afm, WaveformThumbnail& thumb)
    : m_loopManager(mgr), m_audioFileManager(afm), m_waveformThumbnail(thumb)
{
    setInterceptsMouseClicks(false, false);
}

LoopOverlay::~LoopOverlay() = default;

void LoopOverlay::paint(juce::Graphics& g)
{
    if (!m_loopManager.hasValidLoop() && !m_loopManager.isLoopEnabled())
        return;

    auto area = getLocalBounds().toFloat();

    double totalDur = m_audioFileManager.hasAudio() ? m_audioFileManager.getDurationSec() : 1.0;
    double loopStart = m_loopManager.getLoopStart();
    double loopEnd   = m_loopManager.getLoopEnd();

    // Clamp to valid range
    loopStart = juce::jlimit(0.0, totalDur, loopStart);
    loopEnd   = juce::jlimit(0.0, totalDur, loopEnd);

    float x1 = m_waveformThumbnail.timeToX(static_cast<float>(loopStart));
    float x2 = m_waveformThumbnail.timeToX(static_cast<float>(loopEnd));

    auto loopRect = juce::Rectangle<float>(x1, 0.0f, x2 - x1, area.getHeight());

    // Draw semi-transparent highlighted region
    if (m_loopManager.hasValidLoop())
    {
        g.setColour(juce::Colour(0x3300FF88)); // semi-transparent teal/green
        g.fillRect(loopRect);

        // Boundary lines
        g.setColour(juce::Colour(0xCC00FF88));
        g.drawVerticalLine(static_cast<int>(x1), 0.0f, area.getHeight());
        g.drawVerticalLine(static_cast<int>(x2), 0.0f, area.getHeight());

        // Loop label
        g.setColour(juce::Colours::white);
        g.setFont(11.0f);
        g.drawText("LOOP", loopRect.removeFromTop(16.0f).reduced(4.0f, 0.0f),
                   juce::Justification::centredLeft);
    }
    else if (m_loopManager.isLoopEnabled())
    {
        // Draw a subtle indicator that loop is enabled but has no range yet
        g.setColour(juce::Colour(0x4400FF88));
        g.drawText("LOOP (no range)", area.withHeight(16.0f).reduced(4.0f, 0.0f),
                   juce::Justification::centredLeft);
    }
}
