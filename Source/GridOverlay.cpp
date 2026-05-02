#include "GridOverlay.h"

#include "AudioFileManager.h"
#include "GridManager.h"
#include "WaveformThumbnail.h"

GridOverlay::GridOverlay(const GridManager& grid, const AudioFileManager& fileManager,
                         const WaveformThumbnail& thumbnail)
    : m_grid(grid), m_fileManager(fileManager), m_thumbnail(thumbnail)
{
    setInterceptsMouseClicks(false, false);
}

void GridOverlay::paint(juce::Graphics& g)
{
    if (!m_grid.isEnabled() || !m_fileManager.hasAudio() || getWidth() <= 0 || getHeight() <= 0)
        return;

    const auto bounds = getLocalBounds().toFloat();
    const double startTime = m_thumbnail.xToTime(0.0f);
    const double endTime = m_thumbnail.xToTime(static_cast<float>(getWidth()));

    if (endTime <= startTime)
        return;

    g.setColour(juce::Colours::white.withAlpha(0.07f));
    for (const double time : m_grid.getGridLines(startTime, endTime))
    {
        const float x = m_thumbnail.timeToX(time);
        if (x >= -1.0f && x <= static_cast<float>(getWidth()) + 1.0f)
            g.fillRect(juce::Rectangle<float>(x - 0.5f, bounds.getY(), 1.0f, bounds.getHeight()));
    }

    g.setColour(juce::Colours::white.withAlpha(0.15f));
    for (const double time : m_grid.getBarLines(startTime, endTime))
    {
        const float x = m_thumbnail.timeToX(time);
        if (x >= -1.0f && x <= static_cast<float>(getWidth()) + 1.0f)
            g.fillRect(juce::Rectangle<float>(x - 0.75f, bounds.getY(), 1.5f, bounds.getHeight()));
    }
}
