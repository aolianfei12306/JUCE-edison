#pragma once
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <vector>
#include <algorithm>

class RegionManager
{
public:
    struct Region
    {
        juce::String name;
        double startTime; // seconds
        double endTime;   // seconds
        juce::Colour colour;
        int id;
    };

    int addRegion(const juce::String& name, double startSec, double endSec);
    void removeRegion(int id);
    void removeRegionAt(double timeSec, double tolerance = 0.05);
    void renameRegion(int id, const juce::String& newName);
    void clear();

    const std::vector<Region>& getRegions() const noexcept { return m_regions; }
    int getNumRegions() const noexcept { return (int)m_regions.size(); }

    int getRegionAt(double timeSec) const;
    const Region* getRegionById(int id) const;

    int getNextRegion(int currentId) const;
    int getPrevRegion(int currentId) const;

    int getCurrentRegionId() const noexcept { return m_currentRegionId; }
    void setCurrentRegionId(int id) noexcept { m_currentRegionId = id; }

    std::function<void()> onRegionsChanged;

private:
    std::vector<Region> m_regions;
    int m_nextId = 1;
    int m_currentRegionId = -1;

    void sortRegions();

    static constexpr int kNumPaletteColours = 8;
    static juce::Colour paletteColour(int index);
};
