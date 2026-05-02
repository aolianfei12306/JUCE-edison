#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class AudioFileManager;
class SelectionManager;

class WaveformThumbnail : public juce::Component
{
public:
    WaveformThumbnail(AudioFileManager& fileManager, SelectionManager& selection);
    ~WaveformThumbnail() override = default;

    void paint(juce::Graphics& g) override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& w) override;

    double getZoom() const noexcept { return m_zoom; }
    void   setZoom(double z) noexcept;
    double getVisibleDuration() const noexcept;
    double getHorizontalOffset() const noexcept { return m_horizontalOffset; }
    void   setHorizontalOffset(double offset) noexcept;

    double xToTime(float x) const;
    float  timeToX(double timeSec) const;

    juce::Rectangle<float> getWaveformBounds() const { return m_waveformBounds; }

private:
    AudioFileManager& m_fileManager;
    SelectionManager& m_selection;
    double  m_zoom             = 1.0;
    double  m_horizontalOffset = 0.0;
    juce::Rectangle<float> m_waveformBounds;

    void drawWaveform(juce::Graphics& g, const juce::Rectangle<float>& bounds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveformThumbnail)
};
