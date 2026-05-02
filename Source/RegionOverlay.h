#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "RegionManager.h"
class AudioFileManager;
class SelectionManager;
class WaveformThumbnail;

class RegionOverlay : public juce::Component,
                      public juce::ChangeListener
{
public:
    RegionOverlay(RegionManager& mgr, AudioFileManager& afm,
                  SelectionManager& sel, WaveformThumbnail& waveform);
    ~RegionOverlay() override = default;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void resized() override;

    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    // Callback when user clicks a region tab
    std::function<void(const RegionManager::Region&)> onRegionSelected;

private:
    RegionManager& m_regionManager;
    AudioFileManager& m_audioFileManager;
    SelectionManager& m_selection;
    WaveformThumbnail& m_waveform;

    juce::Rectangle<int> getRegionTabBounds(int index) const;
    int getRegionIndexAt(int x) const;

    static constexpr int kTabHeight = 22;
    static constexpr int kMinTabWidth = 80;
    static constexpr int kPadding = 4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RegionOverlay)
};
