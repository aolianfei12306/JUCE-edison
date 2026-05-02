#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "LoopManager.h"

class AudioFileManager;
class WaveformThumbnail;

class LoopOverlay : public juce::Component
{
public:
    LoopOverlay(LoopManager& mgr, AudioFileManager& afm, WaveformThumbnail& thumb);
    ~LoopOverlay() override;

    void paint(juce::Graphics& g) override;

private:
    LoopManager& m_loopManager;
    AudioFileManager& m_audioFileManager;
    WaveformThumbnail& m_waveformThumbnail;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoopOverlay)
};
