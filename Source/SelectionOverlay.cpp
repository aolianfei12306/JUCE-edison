#include "SelectionOverlay.h"
#include "GridManager.h"
#include "SelectionManager.h"
#include "WaveformThumbnail.h"
#include "AudioFileManager.h"

SelectionOverlay::SelectionOverlay(SelectionManager& selection, WaveformThumbnail& thumbnail,
                                   AudioFileManager& fileManager, GridManager& gridManager)
    : m_selection(selection), m_thumbnail(thumbnail), m_fileManager(fileManager), m_gridManager(gridManager)
{
    setInterceptsMouseClicks(true, false);
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

double SelectionOverlay::snapTime(double t) const noexcept
{
    return m_selection.snapTime(t, m_fileManager, m_gridManager);
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
        if (m_selection.hasSelection())
        {
            float sx = m_thumbnail.timeToX(m_selection.getSelectionStart());
            float ex = m_thumbnail.timeToX(m_selection.getSelectionEnd());

            if (e.mods.isAltDown() && e.position.x >= sx && e.position.x <= ex)
            {
                m_dragMode = DragMode::Export;
                repaint();
                return;
            }

            if (e.position.x < sx || e.position.x > ex)
                m_selection.clearSelection();
        }

        m_dragMode = DragMode::Create;
        m_dragStartTime = snapTime(t);
    }

    repaint();
    m_thumbnail.repaint();
    if (auto* parent = getParentComponent())
        parent->repaint();
}

void SelectionOverlay::mouseDrag(const juce::MouseEvent& e)
{
    double t = m_thumbnail.xToTime(e.position.x);
    t = juce::jmax(0.0, t);
    // Snap the dragged edge to zero crossing when enabled
    t = snapTime(t);

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
    case DragMode::Export:
        if (onExportDragStarted)
            onExportDragStarted(this);
        break;
    default:
        break;
    }
    repaint();
    // Ensure waveform selection highlight and parent info text update in real-time
    m_thumbnail.repaint();
    if (auto* parent = getParentComponent())
        parent->repaint();
}

void SelectionOverlay::mouseUp(const juce::MouseEvent& /*e*/)
{
    switch (m_dragMode)
    {
    case DragMode::Export:
        m_dragMode = DragMode::None;
        break;
    default:
        m_dragMode = DragMode::None;
        break;
    }
    repaint();
}

void SelectionOverlay::paint(juce::Graphics& /*g*/) {}


