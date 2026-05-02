#pragma once

#include <juce_core/juce_core.h>

#include <cmath>
#include <vector>

class GridManager
{
public:
    GridManager() = default;

    void setEnabled(bool enabled) noexcept { m_enabled = enabled; }
    bool isEnabled() const noexcept { return m_enabled; }
    void toggleEnabled() noexcept { m_enabled = !m_enabled; }

    void setBPM(double bpm) noexcept { m_bpm = juce::jlimit(20.0, 500.0, bpm); }
    double getBPM() const noexcept { return m_bpm; }

    // Division denominator: 1=whole, 2=half, 4=quarter, 8=eighth, 16=sixteenth.
    void setDivision(int division) noexcept { m_division = juce::jlimit(1, 32, division); }
    int getDivision() const noexcept { return m_division; }

    // In standard BPM notation, one beat is a quarter note.
    double getBeatDuration() const noexcept { return 60.0 / m_bpm; }
    double getGridInterval() const noexcept { return getBeatDuration() * (4.0 / static_cast<double>(m_division)); }

    double snapToGrid(double timeSec, double totalDuration) const noexcept
    {
        if (!m_enabled || m_bpm <= 0.0)
            return timeSec;

        const double interval = getGridInterval();
        if (interval <= 0.0)
            return timeSec;

        const double gridPos = std::round(timeSec / interval) * interval;
        return juce::jlimit(0.0, totalDuration, gridPos);
    }

    std::vector<double> getGridLines(double startTime, double endTime, int maxLines = 500) const noexcept
    {
        std::vector<double> lines;

        if (!m_enabled || m_bpm <= 0.0 || endTime < startTime)
            return lines;

        const double interval = getGridInterval();
        if (interval <= 0.0)
            return lines;

        const double first = std::ceil(startTime / interval) * interval;
        for (double t = first; t <= endTime && static_cast<int>(lines.size()) < maxLines; t += interval)
            lines.push_back(t);

        return lines;
    }

    std::vector<double> getBarLines(double startTime, double endTime, int beatsPerBar = 4, int maxLines = 200) const noexcept
    {
        std::vector<double> lines;

        if (!m_enabled || m_bpm <= 0.0 || endTime < startTime || beatsPerBar <= 0)
            return lines;

        const double barDuration = getBeatDuration() * static_cast<double>(beatsPerBar);
        if (barDuration <= 0.0)
            return lines;

        const double first = std::ceil(startTime / barDuration) * barDuration;
        for (double t = first; t <= endTime && static_cast<int>(lines.size()) < maxLines; t += barDuration)
            lines.push_back(t);

        return lines;
    }

private:
    bool m_enabled = false;
    double m_bpm = 120.0;
    int m_division = 4;
};
