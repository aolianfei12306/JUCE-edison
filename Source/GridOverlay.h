#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class AudioFileManager;
class WaveformThumbnail;
class GridManager;

class GridOverlay : public juce::Component
{
public:
    GridOverlay(const GridManager& grid, const AudioFileManager& fileManager,
                const WaveformThumbnail& thumbnail);
    ~GridOverlay() override = default;

    void paint(juce::Graphics& g) override;

private:
    const GridManager& m_grid;
    const AudioFileManager& m_fileManager;
    const WaveformThumbnail& m_thumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GridOverlay)
};
