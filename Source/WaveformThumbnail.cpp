#include "WaveformThumbnail.h"
#include "AudioFileManager.h"
#include "SelectionManager.h"

WaveformThumbnail::WaveformThumbnail(AudioFileManager& fileManager, SelectionManager& selection)
    : m_fileManager(fileManager), m_selection(selection) {}

void WaveformThumbnail::setZoom(double z) noexcept
{
    m_zoom = juce::jlimit(0.001, 10000.0, z);
}

double WaveformThumbnail::getVisibleDuration() const noexcept
{
    if (!m_fileManager.hasAudio()) return 1.0;
    return m_fileManager.getDurationSec() / m_zoom;
}

void WaveformThumbnail::setHorizontalOffset(double offset) noexcept
{
    double maxOffset = m_zoom > 1.0 ? 0.0 : (1.0 - m_zoom) / m_zoom;
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
    auto* thumb = m_fileManager.getThumbnail();
    if (thumb == nullptr) return;

    double totalDur = m_fileManager.getDurationSec();
    double visible  = totalDur / m_zoom;
    double offsetT  = m_horizontalOffset * totalDur;
    double startTime = juce::jmax(0.0, offsetT);
    double endTime   = juce::jmin(totalDur, offsetT + visible);

    if (endTime <= startTime) return;

    g.setColour(juce::Colour(0xFF4FC3F7));
    thumb->drawChannels(g, bounds.toNearestInt(), startTime, endTime, 1.0f);

    // Centre line
    g.setColour(juce::Colour(0x33FFFFFF));
    float midY = bounds.getCentreY();
    g.drawHorizontalLine(static_cast<int>(midY), bounds.getX(), bounds.getRight());
}

void WaveformThumbnail::mouseWheelMove(const juce::MouseEvent& e,
                                        const juce::MouseWheelDetails& w)
{
    if (w.deltaY == 0.0) return;

    if (e.mods.isCtrlDown())
    {
        // Ctrl+wheel: fine zoom
        double factor = (w.deltaY > 0) ? 1.05 : 1.0 / 1.05;
        m_zoom = juce::jlimit(0.001, 10000.0, m_zoom * factor);
    }
    else
    {
        // Plain wheel: horizontal pan/scroll (deltaY>0 = scroll right = move view forward)
        double totalDur = m_fileManager.hasAudio() ? m_fileManager.getDurationSec() : 1.0;
        double visibleDur = totalDur / m_zoom;
        double scrollAmount = visibleDur * 0.1 * w.deltaY;
        double offsetFrac = scrollAmount / totalDur;
        setHorizontalOffset(m_horizontalOffset + offsetFrac);
    }

    repaint();
}
