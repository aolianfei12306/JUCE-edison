#include "WaveformThumbnail.h"
#include "AudioFileManager.h"
#include "SelectionManager.h"

#include <cmath>

WaveformThumbnail::WaveformThumbnail(AudioFileManager& fileManager, SelectionManager& selection)
    : m_fileManager(fileManager), m_selection(selection) {}

void WaveformThumbnail::setZoom(double z) noexcept
{
    m_zoom = juce::jlimit(0.001, 10000.0, z);
    setHorizontalOffset(m_horizontalOffset);
}

double WaveformThumbnail::getVisibleDuration() const noexcept
{
    if (!m_fileManager.hasAudio()) return 1.0;
    return m_fileManager.getDurationSec() / m_zoom;
}

void WaveformThumbnail::setHorizontalOffset(double offset) noexcept
{
    double maxOffset = m_zoom > 1.0 ? (1.0 - 1.0 / m_zoom) : 0.0;
    m_horizontalOffset = juce::jlimit(0.0, maxOffset, offset);
}

double WaveformThumbnail::xToTime(float x) const
{
    auto r = getLocalBounds().toFloat();
    float pad = 8.0f;
    float drawW = r.getWidth() - pad * 2.0f;
    if (drawW <= 0) return 0.0;
    double totalDur = m_fileManager.hasAudio() ? m_fileManager.getDurationSec() : 1.0;
    double visible  = totalDur / m_zoom;
    double offsetT  = m_horizontalOffset * totalDur;
    return juce::jlimit(0.0, totalDur, (x - pad) / drawW * visible + offsetT);
}

float WaveformThumbnail::timeToX(double timeSec) const
{
    auto r = getLocalBounds().toFloat();
    float pad = 8.0f;
    float drawW = r.getWidth() - pad * 2.0f;
    if (drawW <= 0) return pad;
    double totalDur = m_fileManager.hasAudio() ? m_fileManager.getDurationSec() : 1.0;
    double visible  = totalDur / m_zoom;
    double offsetT  = m_horizontalOffset * totalDur;
    return pad + static_cast<float>((timeSec - offsetT) / visible * drawW);
}

void WaveformThumbnail::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    g.fillAll(juce::Colour(0xFF1A1A2E));

    if (!m_fileManager.hasAudio())
    {
        g.setColour(juce::Colours::grey);
        g.setFont(14.0f);
        g.drawText("Drop audio file or press Ctrl+O", bounds.toNearestInt(),
                   juce::Justification::centred);
        return;
    }

    float pad = 8.0f;
    m_waveformBounds = bounds.reduced(pad, 4.0f);
    drawWaveform(g, m_waveformBounds);

    // Selection highlight
    if (m_selection.hasSelection())
    {
        float sx = timeToX(m_selection.getSelectionStart());
        float ex = timeToX(m_selection.getSelectionEnd());
        juce::Rectangle<float> selRect{sx, m_waveformBounds.getY(),
                                       ex - sx, m_waveformBounds.getHeight()};
        g.setColour(juce::Colour(0x308EECF5));
        g.fillRect(selRect);
        g.setColour(juce::Colour(0xAA8EECF5));
        g.drawRect(selRect, 1.0f);
    }

    // Playback cursor
    double pos = m_selection.getPlaybackPosition();
    if (pos > 0.0)
    {
        float cx = timeToX(pos);
        g.setColour(juce::Colours::white);
        g.drawVerticalLine(static_cast<int>(cx),
                           m_waveformBounds.getY(), m_waveformBounds.getBottom());
    }
}

void WaveformThumbnail::drawWaveform(juce::Graphics& g, const juce::Rectangle<float>& bounds)
{
    auto* buffer = m_fileManager.getBuffer();
    if (buffer == nullptr || buffer->getNumSamples() <= 0 || buffer->getNumChannels() <= 0)
        return;

    double totalDur = m_fileManager.getDurationSec();
    double sampleRate = m_fileManager.getSampleRate();
    if (totalDur <= 0.0 || sampleRate <= 0.0 || bounds.getWidth() <= 0.0f || bounds.getHeight() <= 0.0f)
        return;

    double visible  = totalDur / m_zoom;
    double offsetT  = m_horizontalOffset * totalDur;
    double startTime = juce::jmax(0.0, offsetT);
    double endTime   = juce::jmin(totalDur, offsetT + visible);

    if (endTime <= startTime) return;

    juce::Graphics::ScopedSaveState clipState(g);
    g.reduceClipRegion(bounds.toNearestInt());

    const int totalSamples = buffer->getNumSamples();
    const int numChannels = buffer->getNumChannels();
    const int firstX = static_cast<int>(std::floor(bounds.getX()));
    const int lastX = static_cast<int>(std::ceil(bounds.getRight()));
    const double startSample = startTime * sampleRate;
    const double endSample = endTime * sampleRate;
    const double visibleSamples = juce::jmax(1.0, endSample - startSample);

    g.setColour(juce::Colour(0xFF4FC3F7));

    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto channelBounds = bounds.withTrimmedTop(bounds.getHeight() * static_cast<float>(ch) / static_cast<float>(numChannels))
                                   .withHeight(bounds.getHeight() / static_cast<float>(numChannels));
        auto lane = channelBounds.reduced(0.0f, 3.0f);

        if (lane.getHeight() <= 1.0f)
            continue;

        const float centreY = lane.getCentreY();
        const float halfHeight = lane.getHeight() * 0.48f;

        for (int x = firstX; x < lastX; ++x)
        {
            const double pixelStart = juce::jlimit(0.0, 1.0,
                (static_cast<double>(x) - static_cast<double>(bounds.getX())) / static_cast<double>(bounds.getWidth()));
            const double pixelEnd = juce::jlimit(0.0, 1.0,
                (static_cast<double>(x + 1) - static_cast<double>(bounds.getX())) / static_cast<double>(bounds.getWidth()));

            int sampleStart = static_cast<int>(std::floor(startSample + pixelStart * visibleSamples));
            int sampleEnd = static_cast<int>(std::ceil(startSample + pixelEnd * visibleSamples));

            sampleStart = juce::jlimit(0, totalSamples - 1, sampleStart);
            sampleEnd = juce::jlimit(sampleStart + 1, totalSamples, sampleEnd);

            const auto range = buffer->findMinMax(ch, sampleStart, sampleEnd - sampleStart);
            const float top = centreY - juce::jlimit(-1.0f, 1.0f, range.getEnd()) * halfHeight;
            const float bottom = centreY - juce::jlimit(-1.0f, 1.0f, range.getStart()) * halfHeight;
            const float y1 = juce::jmin(top, bottom);
            const float y2 = juce::jmax(top, bottom);

            g.drawLine(static_cast<float>(x) + 0.5f, y1,
                       static_cast<float>(x) + 0.5f, juce::jmax(y1 + 1.0f, y2), 1.0f);
        }

        if (numChannels > 1)
        {
            g.setColour(juce::Colour(0x22FFFFFF));
            g.drawHorizontalLine(static_cast<int>(channelBounds.getBottom()), bounds.getX(), bounds.getRight());
            g.setColour(juce::Colour(0xFF4FC3F7));
        }
    }

    // Centre line
    g.setColour(juce::Colour(0x33FFFFFF));
    if (numChannels == 1)
    {
        float midY = bounds.getCentreY();
        g.drawHorizontalLine(static_cast<int>(midY), bounds.getX(), bounds.getRight());
    }
    else
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            auto channelBounds = bounds.withTrimmedTop(bounds.getHeight() * static_cast<float>(ch) / static_cast<float>(numChannels))
                                       .withHeight(bounds.getHeight() / static_cast<float>(numChannels));
            g.drawHorizontalLine(static_cast<int>(channelBounds.getCentreY()), bounds.getX(), bounds.getRight());
        }
    }
}

void WaveformThumbnail::mouseWheelMove(const juce::MouseEvent& e,
                                        const juce::MouseWheelDetails& w)
{
    if (!m_fileManager.hasAudio() || w.deltaY == 0.0)
        return;

    const double oldZoom = m_zoom;
    const double totalDur = m_fileManager.getDurationSec();
    if (totalDur <= 0.0)
        return;

    const double anchorTime = xToTime(e.position.x);
    const double factor = (w.deltaY > 0.0)
        ? (e.mods.isCtrlDown() ? 1.08 : 1.25)
        : (e.mods.isCtrlDown() ? 1.0 / 1.08 : 1.0 / 1.25);

    setZoom(oldZoom * factor);

    auto r = getLocalBounds().toFloat();
    const float pad = 8.0f;
    const float drawW = r.getWidth() - pad * 2.0f;
    if (drawW > 0.0f)
    {
        const double anchorRatio = juce::jlimit(0.0, 1.0,
            static_cast<double>(e.position.x - pad) / static_cast<double>(drawW));
        const double newVisibleDuration = totalDur / m_zoom;
        const double newOffsetTime = anchorTime - anchorRatio * newVisibleDuration;
        setHorizontalOffset(newOffsetTime / totalDur);
    }

    repaint();
}
