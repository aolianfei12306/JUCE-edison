#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class SelectionManager;
class WaveformThumbnail;

class SelectionOverlay : public juce::Component
{
public:
    SelectionOverlay(SelectionManager& selection, WaveformThumbnail& thumbnail);
    ~SelectionOverlay() override = default;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseMove(const juce::MouseEvent& e) override;

    void drawSelectionInfo(juce::Graphics& g, const juce::Rectangle<int>& area);

private:
    enum class DragMode { None, Create, MoveLeft, MoveRight };

    DragMode m_dragMode = DragMode::None;
    double   m_dragStartTime = 0.0;
    SelectionManager& m_selection;
    WaveformThumbnail& m_thumbnail;

    static constexpr float kEdgeTolerance = 8.0f;
    DragMode hitTestEdge(const juce::MouseEvent& e) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectionOverlay)
};
