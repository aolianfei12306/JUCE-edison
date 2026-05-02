#pragma once
#include <juce_core/juce_core.h>

class LoopManager
{
public:
    LoopManager() = default;

    bool isLoopEnabled() const noexcept { return m_enabled; }
    void setLoopEnabled(bool en) noexcept
    {
        m_enabled = en;
        if (!en) m_active = false;
    }

    void setLoopRange(double startSec, double endSec) noexcept
    {
        m_loopStart = juce::jmin(startSec, endSec);
        m_loopEnd   = juce::jmax(startSec, endSec);
    }

    void clearLoop() noexcept
    {
        m_enabled = false;
        m_active  = false;
        m_loopStart = m_loopEnd = 0.0;
    }

    double getLoopStart() const noexcept { return m_loopStart; }
    double getLoopEnd()   const noexcept { return m_loopEnd; }

    bool hasValidLoop() const noexcept
    {
        return m_enabled && (m_loopEnd - m_loopStart) > 0.001;
    }

    void setActive(bool a) noexcept { m_active = a; }
    bool isActive() const noexcept { return m_active; }

private:
    bool   m_enabled   = false;
    bool   m_active    = false;
    double m_loopStart = 0.0;
    double m_loopEnd   = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoopManager)
};
