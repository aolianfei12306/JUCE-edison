#pragma once
#include "AudioFileManager.h"
#include <juce_core/juce_core.h>
#include <cmath>

class AudioFileManager;

class SelectionManager
{
public:
    SelectionManager() = default;

    void setPlaybackPosition(double posSec) noexcept { m_playbackPos = posSec; }
    double getPlaybackPosition() const noexcept { return m_playbackPos; }

    bool hasSelection() const noexcept
    {
        return m_selectionStart >= 0.0 && m_selectionEnd >= 0.0
            && std::abs(m_selectionEnd - m_selectionStart) > 0.001;
    }

    void setSelection(double startSec, double endSec) noexcept
    {
        m_selectionStart = std::min(startSec, endSec);
        m_selectionEnd   = std::max(startSec, endSec);
    }

    void clearSelection() noexcept
    {
        m_selectionStart = -1.0;
        m_selectionEnd   = -1.0;
    }

    double getSelectionStart() const noexcept { return m_selectionStart; }
    double getSelectionEnd()   const noexcept { return m_selectionEnd; }
    double getSelectionDuration() const noexcept
    {
        return hasSelection() ? (m_selectionEnd - m_selectionStart) : 0.0;
    }

    void setTotalDuration(double sec) noexcept { m_totalDuration = sec; }
    double getTotalDuration() const noexcept { return m_totalDuration; }

    // ── Snap-to-zero crossing ──
    void setSnapToZero(bool enabled) noexcept { m_snapToZero = enabled; }
    bool isSnapToZero() const noexcept { return m_snapToZero; }
    void toggleSnapToZero() noexcept { m_snapToZero = !m_snapToZero; }

    /** Snap a time value to the nearest zero crossing.
     *  If snap-to-zero is disabled, returns timeSec unchanged.
     */
    double snapTime(double timeSec, const AudioFileManager& fileManager) const noexcept
    {
        if (!m_snapToZero)
            return timeSec;
        return fileManager.snapToZeroCrossing(timeSec);
    }

private:
    double m_playbackPos    = 0.0;
    double m_totalDuration  = 0.0;
    double m_selectionStart = -1.0;
    double m_selectionEnd   = -1.0;
    bool   m_snapToZero     = false;
};
