#include "SelectionOverlay.h"
#include "SelectionManager.h"
#include "WaveformThumbnail.h"

SelectionOverlay::SelectionOverlay(SelectionManager& selection, WaveformThumbnail& thumbnail)
    : m_selection(selection), m_thumbnail(thumbnail)
{
    setInterceptsMouseClicks(true, false);
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

SelectionOverlay::DragMode SelectionOverlay::hitTestEdge(const juce::MouseEvent& e) const
{
    if (!m_selection.hasSelection()) return DragMode::None;

    float sx = m_thumbnail.timeToX(m_selection.getSelectionStart());
    float ex = m_thumbnail.timeToX(m_selection.getSelectionEnd());

    if (std::abs(e.position.x - sx) < kEdgeTolerance) return DragMode::MoveLeft;
    if (std::abs(e.position.x - ex) < kEdgeTolerance) return DragMode::MoveRight;

    return DragMode::None;
}

void SelectionOverlay::mouseMove(const juce::MouseEvent& e)
{
    if (hitTestEdge(e) != DragMode::None)
        setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
    else
        setMouseCursor(juce::MouseCursor::NormalCursor);
}

void SelectionOverlay::mouseDown(const juce::MouseEvent& e)
{
    m_dragMode = hitTestEdge(e);

    if (m_dragMode == DragMode::None)
    {
        double t = m_thumbnail.xToTime(e.position.x);
        // If outside existing selection, start new one
        if (m_selection.hasSelection())
        {
            float sx = m_thumbnail.timeToX(m_selection.getSelectionStart());
            float ex = m_thumbnail.timeToX(m_selection.getSelectionEnd());
            if (e.position.x < sx || e.position.x > ex)
                m_selection.clearSelection();
        }

        m_dragMode = DragMode::Create;
        m_dragStartTime = t;
    }

    repaint();
}

void SelectionOverlay::mouseDrag(const juce::MouseEvent& e)
{
    double t = m_thumbnail.xToTime(e.position.x);
    t = juce::jmax(0.0, t);

    switch (m_dragMode)
    {
    case DragMode::Create:
        m_selection.setSelection(m_dragStartTime, t);
        break;
    case DragMode::MoveLeft:
        m_selection.setSelection(t, m_selection.getSelectionEnd());
        break;
    case DragMode::MoveRight:
        m_selection.setSelection(m_selection.getSelectionStart(), t);
        break;
    default:
        break;
    }
    repaint();
}

void SelectionOverlay::mouseUp(const juce::MouseEvent& /*e*/)
{
    m_dragMode = DragMode::None;
    repaint();
}

void SelectionOverlay::paint(juce::Graphics& /*g*/) {}

void SelectionOverlay::drawSelectionInfo(juce::Graphics& g, const juce::Rectangle<int>& area)
{
    if (!m_selection.hasSelection()) return;

    auto start = m_selection.getSelectionStart();
    auto end   = m_selection.getSelectionEnd();
    auto dur   = m_selection.getSelectionDuration();

    auto fmt = [](double sec) -> juce::String {
        int m = static_cast<int>(sec) / 60;
        double r = sec - m * 60;
        return juce::String::formatted("%02d:%06.3f", m, r);
    };

    g.setColour(juce::Colour(0xCCE0E0E0));
    g.setFont(12.0f);
    g.drawText("Sel: " + fmt(start) + " - " + fmt(end) + " (" + fmt(dur) + ")",
               area, juce::Justification::centredLeft);
}
