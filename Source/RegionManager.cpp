#include "RegionManager.h"

juce::Colour RegionManager::paletteColour(int index)
{
    static const juce::Colour palette[kNumPaletteColours] = {
        juce::Colour(0xFFE74C3C), // Red
        juce::Colour(0xFF3498DB), // Blue
        juce::Colour(0xFF2ECC71), // Green
        juce::Colour(0xFFF39C12), // Orange
        juce::Colour(0xFF9B59B6), // Purple
        juce::Colour(0xFF1ABC9C), // Teal
        juce::Colour(0xFFE67E22), // Dark Orange
        juce::Colour(0xFF2980B9), // Dark Blue
    };
    return palette[index % kNumPaletteColours];
}

int RegionManager::addRegion(const juce::String& name, double startSec, double endSec)
{
    if (endSec - startSec < 0.001)
        return -1;

    Region r;
    r.id = m_nextId++;
    r.name = name.isEmpty() ? ("Region " + juce::String(r.id)) : name;
    r.startTime = juce::jmin(startSec, endSec);
    r.endTime = juce::jmax(startSec, endSec);

    // Assign palette colour based on insertion order
    r.colour = paletteColour((int)m_regions.size());

    m_regions.push_back(r);
    sortRegions();

    if (onRegionsChanged)
        onRegionsChanged();

    return r.id;
}

void RegionManager::removeRegion(int id)
{
    m_regions.erase(
        std::remove_if(m_regions.begin(), m_regions.end(),
            [id](const Region& r) { return r.id == id; }),
        m_regions.end());

    if (m_currentRegionId == id)
        m_currentRegionId = m_regions.empty() ? -1 : m_regions.front().id;

    if (onRegionsChanged)
        onRegionsChanged();
}

void RegionManager::removeRegionAt(double timeSec, double tolerance)
{
    for (auto it = m_regions.begin(); it != m_regions.end(); ++it)
    {
        if (timeSec >= it->startTime - tolerance && timeSec <= it->endTime + tolerance)
        {
            int removedId = it->id;
            m_regions.erase(it);

            if (m_currentRegionId == removedId)
                m_currentRegionId = m_regions.empty() ? -1 : m_regions.front().id;

            if (onRegionsChanged)
                onRegionsChanged();

            return;
        }
    }
}

void RegionManager::renameRegion(int id, const juce::String& newName)
{
    for (auto& r : m_regions)
    {
        if (r.id == id)
        {
            r.name = newName;
            return;
        }
    }
}

void RegionManager::clear()
{
    m_regions.clear();
    m_nextId = 1;
    m_currentRegionId = -1;

    if (onRegionsChanged)
        onRegionsChanged();
}

int RegionManager::getRegionAt(double timeSec) const
{
    for (const auto& r : m_regions)
        if (timeSec >= r.startTime && timeSec <= r.endTime)
            return r.id;
    return -1;
}

const RegionManager::Region* RegionManager::getRegionById(int id) const
{
    for (const auto& r : m_regions)
        if (r.id == id)
            return &r;
    return nullptr;
}

int RegionManager::getNextRegion(int currentId) const
{
    if (m_regions.empty())
        return -1;

    for (size_t i = 0; i < m_regions.size() - 1; ++i)
        if (m_regions[i].id == currentId)
            return m_regions[i + 1].id;

    // Wrap around to first
    return m_regions[0].id;
}

int RegionManager::getPrevRegion(int currentId) const
{
    if (m_regions.empty())
        return -1;

    for (size_t i = 1; i < m_regions.size(); ++i)
        if (m_regions[i].id == currentId)
            return m_regions[i - 1].id;

    // Wrap around to last
    return m_regions.back().id;
}

void RegionManager::sortRegions()
{
    std::sort(m_regions.begin(), m_regions.end(),
        [](const Region& a, const Region& b) { return a.startTime < b.startTime; });
}
