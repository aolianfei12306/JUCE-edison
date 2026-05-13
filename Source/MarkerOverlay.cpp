#include "MarkerOverlay.h"
#include "AudioFileManager.h"

void MarkerOverlay::paint(juce::Graphics& g)
{
    const auto& markers = m_markerManager.getMarkers();
    if (markers.empty())
        return;

    auto bounds = getLocalBounds().toFloat();
    float compHeight = bounds.getHeight();
    float flagSize = 8.0f;
    float flagHalf = flagSize * 0.5f;

    for (const auto& marker : markers)
    {
        int x = m_waveformThumbnail.timeToX(marker.timeSeconds);
        if (x < -20 || x > getWidth() + 20)
            continue;

        bool isActive = (m_playheadPos >= 0.0
                         && std::abs(marker.timeSeconds - m_playheadPos) < 0.01);

        // Colors
        juce::Colour lineColour = isActive
            ? juce::Colour(0xFF00E5FF)  // bright cyan when active
            : juce::Colour(0xB300D4FF); // standard cyan, 70% alpha
        juce::Colour flagColour = isActive
            ? juce::Colour(0xFF00E5FF)
            : juce::Colour(0xCC00D4FF);

        float lineWidth = isActive ? 2.0f : 1.0f;

        // Vertical hairline (use a thin rect since drawVerticalLine doesn't support width)
        g.setColour(lineColour);
        g.fillRect(juce::Rectangle<float>(
            (float)x - lineWidth * 0.5f,
            flagSize + 2.0f,
            lineWidth,
            compHeight - flagSize - 2.0f));

        // Triangular flag at top
        juce::Path flagPath;
        float flagTop = 2.0f;
        flagPath.addTriangle(
            (float)x,           flagTop + flagSize,
            (float)x - flagHalf, flagTop,
            (float)x + flagHalf, flagTop
        );
        g.setColour(flagColour);
        g.fillPath(flagPath);
        g.strokePath(flagPath, juce::PathStrokeType(1.0f));

        // Label text
        g.setColour(juce::Colour(0xE0FFFFFF));
        g.setFont(10.0f);
        g.drawText(marker.label,
                   x + (int)flagHalf + 3, (int)flagTop,
                   40, 14,
                   juce::Justification::centredLeft, false);
    }
}

bool MarkerOverlay::hitTest(int x, int y)
{
    return getMarkerAtPosition(x, y) >= 0;
}

void MarkerOverlay::mouseDown(const juce::MouseEvent& e)
{
    int markerId = getMarkerAtPosition(e.x, e.y);
    if (markerId >= 0 && onMarkerClicked)
    {
        const auto& markers = m_markerManager.getMarkers();
        for (const auto& m : markers)
        {
            if (m.id == markerId)
            {
                onMarkerClicked(m.timeSeconds);
                repaint();
                return;
            }
        }
    }
}

int MarkerOverlay::getMarkerAtPosition(int x, int y) const
{
    const auto& markers = m_markerManager.getMarkers();
    float flagHalf = 4.0f;

    for (const auto& marker : markers)
    {
        int mx = m_waveformThumbnail.timeToX(marker.timeSeconds);
        if (mx < -10 || mx > getWidth() + 10)
            continue;

        // Check if click is near the flag area (top 20px)
        if (y >= 0 && y <= 20)
        {
            if (std::abs(x - mx) <= 8)
                return marker.id;
        }
        // Also check if click is near the vertical line (wider tolerance)
        if (std::abs(x - mx) <= 4)
        {
            return marker.id;
        }
    }
    return -1;
}
