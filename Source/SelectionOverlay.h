#pragma once
#include <functional>
#include <juce_gui_basics/juce_gui_basics.h>

class SelectionManager;
class WaveformThumbnail;
class AudioFileManager;

class SelectionOverlay : public juce::Component
{
public:
    SelectionOverlay(SelectionManager& selection, WaveformThumbnail& thumbnail,
                     AudioFileManager& fileManager);
    ~SelectionOverlay() override = default;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;

    void drawSelectionInfo(juce::Graphics& g, const juce::Rectangle<int>& area);

    std::function<void(juce::Component*)> onExportDragStarted;

private:
    enum class DragMode { None, Create, MoveLeft, MoveRight, Export };

    DragMode m_dragMode = DragMode::None;
    double   m_dragStartTime = 0.0;
    SelectionManager& m_selection;
    WaveformThumbnail& m_thumbnail;
    AudioFileManager&  m_fileManager;

    static constexpr float kEdgeTolerance = 8.0f;
    DragMode hitTestEdge(const juce::MouseEvent& e) const;

    /** Snap a time value to zero crossing if snap-to-zero is enabled */
    double snapTime(double t) const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectionOverlay)
};
