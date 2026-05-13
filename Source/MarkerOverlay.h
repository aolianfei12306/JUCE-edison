#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "MarkerManager.h"
#include "WaveformThumbnail.h"

class AudioFileManager;

class MarkerOverlay : public juce::Component
{
public:
    MarkerOverlay(MarkerManager& mgr, AudioFileManager& afm, WaveformThumbnail& thumb)
        : m_markerManager(mgr)
        , m_audioFileManager(afm)
        , m_waveformThumbnail(thumb)
    {
    }

    void paint(juce::Graphics& g) override;
    bool hitTest(int x, int y) override;
    void mouseDown(const juce::MouseEvent& e) override;

    void setPlaybackPosition(double pos) { m_playheadPos = pos; }
    void setTotalDuration(double dur) { m_totalDuration = dur; }

    std::function<void(double)> onMarkerClicked;

private:
    MarkerManager& m_markerManager;
    AudioFileManager& m_audioFileManager;
    WaveformThumbnail& m_waveformThumbnail;
    double m_playheadPos = -1.0;
    double m_totalDuration = 0.0;

    int getMarkerAtPosition(int x, int y) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MarkerOverlay)
};
