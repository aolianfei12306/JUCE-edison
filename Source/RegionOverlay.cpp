#include "RegionOverlay.h"
#include "RegionManager.h"
#include "SelectionManager.h"
#include "WaveformThumbnail.h"
#include "AudioFileManager.h"

RegionOverlay::RegionOverlay(RegionManager& mgr, AudioFileManager& afm,
                             SelectionManager& sel, WaveformThumbnail& waveform)
    : m_regionManager(mgr)
    , m_audioFileManager(afm)
    , m_selection(sel)
    , m_waveform(waveform)
{
    // Listen for region list changes to repaint
    m_regionManager.onRegionsChanged = [this]
    {
        juce::MessageManager::callAsync([this] { repaint(); });
    };
}

RegionOverlay::~RegionOverlay()
{
    // Clear the regions-changed callback to prevent dangling lambda
    // during destruction (m_regionManager outlives this overlay)
    m_regionManager.onRegionsChanged = nullptr;
}

void RegionOverlay::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.fillAll(juce::Colour(0xFF252540));

    if (!m_audioFileManager.hasAudio())
    {
        g.setColour(juce::Colours::grey);
        g.drawText("No audio loaded", bounds, juce::Justification::centred, true);
        return;
    }

    const auto& regions = m_regionManager.getRegions();
    if (regions.empty())
    {
        g.setColour(juce::Colours::darkgrey);
        g.setFont(11.0f);
        g.drawText("Select a range and press Ctrl+Shift+R to add a region",
                   bounds, juce::Justification::centred, true);
        return;
    }

    int currentId = m_regionManager.getCurrentRegionId();

    // Draw each region as a tab
    for (int i = 0; i < (int)regions.size(); ++i)
    {
        const auto& region = regions[i];
        auto tabBounds = getRegionTabBounds(i);
        bool isActive = (region.id == currentId);

        // Tab background – active tab gets brighter border
        auto bg = region.colour.withAlpha(isActive ? 0.9f : 0.6f);
        g.setColour(bg);
        g.fillRoundedRectangle(tabBounds.toFloat(), 4.0f);

        // Border
        g.setColour(isActive ? region.colour.brighter(0.5f)
                             : region.colour.withAlpha(0.3f));
        g.drawRoundedRectangle(tabBounds.toFloat(), 4.0f, 1.0f);

        // Duration text
        double dur = region.endTime - region.startTime;
        juce::String durStr = juce::String(dur, 2) + "s";

        // Region name
        g.setColour(isActive ? juce::Colours::white : juce::Colours::lightgrey);
        g.setFont(11.0f);
        g.drawText(region.name, tabBounds.reduced(4, 0),
                   juce::Justification::centredLeft, true);

        // Duration label on right side
        g.setColour(juce::Colours::white.withAlpha(0.5f));
        g.setFont(9.0f);
        g.drawText(durStr, tabBounds.reduced(4, 0),
                   juce::Justification::centredRight, true);
    }
}

void RegionOverlay::resized()
{
    repaint();
}

void RegionOverlay::mouseDown(const juce::MouseEvent& e)
{
    if (!m_audioFileManager.hasAudio())
        return;

    if (e.mods.isRightButtonDown())
    {
        // Right-click context menu on a region tab
        int idx = getRegionIndexAt(e.x);
        if (idx >= 0 && idx < m_regionManager.getNumRegions())
        {
            const auto& regions = m_regionManager.getRegions();
            const auto& region = regions[idx];
            int id = region.id;

            juce::PopupMenu menu;
            menu.addItem("Select \"" + region.name + "\"", true, false, [this, id]() {
                const auto* r = m_regionManager.getRegionById(id);
                if (r && onRegionSelected)
                    onRegionSelected(*r);
            });
            menu.addSeparator();
            // Rename not available on Linux (no modal loop support)
            // Region auto-names as Region 1, Region 2, etc.
            menu.addSeparator();
            menu.addItem("Remove \"" + region.name + "\"", [this, id]() {
                m_regionManager.removeRegion(id);
                repaint();
            });
            menu.addItem("Remove All Regions", [this]() {
                m_regionManager.clear();
                repaint();
            });
            menu.showMenuAsync(juce::PopupMenu::Options{});
        }
        return;
    }

    // Left-click: select region
    int idx = getRegionIndexAt(e.x);
    if (idx >= 0 && idx < m_regionManager.getNumRegions())
    {
        const auto& regions = m_regionManager.getRegions();
        const auto& region = regions[idx];

        m_regionManager.setCurrentRegionId(region.id);

        if (onRegionSelected)
            onRegionSelected(region);

        repaint();
    }
}

juce::Rectangle<int> RegionOverlay::getRegionTabBounds(int index) const
{
    int numRegions = m_regionManager.getNumRegions();
    if (numRegions <= 0)
        return {};

    int availWidth = getWidth() - kPadding * 2;
    int tabWidth = std::max(kMinTabWidth, availWidth / numRegions);

    int x = kPadding + index * tabWidth;
    return { x, 1, tabWidth - 2, kTabHeight - 2 };
}

int RegionOverlay::getRegionIndexAt(int x) const
{
    int numRegions = m_regionManager.getNumRegions();
    for (int i = 0; i < numRegions; ++i)
    {
        if (getRegionTabBounds(i).contains(x, getHeight() / 2))
            return i;
    }
    return -1;
}
