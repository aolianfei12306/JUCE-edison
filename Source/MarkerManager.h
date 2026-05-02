#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <vector>
#include <algorithm>

class MarkerManager
{
public:
    struct Marker
    {
        double timeSeconds;
        int    id;
        juce::String label;
    };

    void addMarker(double timeSeconds)
    {
        Marker m;
        m.timeSeconds = timeSeconds;
        m.id = m_nextId++;
        m.label = "M" + juce::String(m.id);

        m_markers.push_back(m);
        std::sort(m_markers.begin(), m_markers.end(),
            [](const Marker& a, const Marker& b) { return a.timeSeconds < b.timeSeconds; });
    }

    void removeMarkerAt(double timeSeconds, double tolerance = 0.05)
    {
        for (auto it = m_markers.begin(); it != m_markers.end(); ++it)
        {
            if (std::abs(it->timeSeconds - timeSeconds) <= tolerance)
            {
                m_markers.erase(it);
                return;
            }
        }
    }

    void removeMarker(int id)
    {
        m_markers.erase(
            std::remove_if(m_markers.begin(), m_markers.end(),
                [id](const Marker& m) { return m.id == id; }),
            m_markers.end());
    }

    void clear()
    {
        m_markers.clear();
        m_nextId = 1;
    }

    const std::vector<Marker>& getMarkers() const { return m_markers; }

    double getNextMarker(double currentTime) const
    {
        for (const auto& m : m_markers)
            if (m.timeSeconds > currentTime + 0.001)
                return m.timeSeconds;
        return -1.0;
    }

    double getPrevMarker(double currentTime) const
    {
        double best = -1.0;
        for (const auto& m : m_markers)
            if (m.timeSeconds < currentTime - 0.001)
                best = m.timeSeconds;
        return best;
    }

    int getMarkerAt(double timeSeconds, double tolerance = 0.05) const
    {
        for (const auto& m : m_markers)
            if (std::abs(m.timeSeconds - timeSeconds) <= tolerance)
                return m.id;
        return -1;
    }

    void setTotalDuration(double dur) { m_duration = dur; }
    int getNumMarkers() const { return (int)m_markers.size(); }

    juce::StringArray getMarkerLabels() const
    {
        juce::StringArray labels;
        for (const auto& m : m_markers)
            labels.add(m.label + " @ " + juce::String(m.timeSeconds, 2) + "s");
        return labels;
    }

private:
    std::vector<Marker> m_markers;
    double m_duration = 0.0;
    int m_nextId = 1;
};
