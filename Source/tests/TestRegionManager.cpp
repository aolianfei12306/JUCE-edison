#include <juce_core/juce_core.h>
#include "../RegionManager.h"

//==============================================================================
class RegionManagerTest : public juce::UnitTest
{
public:
    RegionManagerTest() : juce::UnitTest("RegionManager", "EdisonCore") {}

    void runTest() override
    {
        RegionManager rm;

        // ── Default state ──
        beginTest("default state");
        expectEquals(rm.getNumRegions(), 0);

        // ── Add regions ──
        beginTest("add regions");
        int id1 = rm.addRegion("Intro", 0.0, 10.0);
        int id2 = rm.addRegion("Verse", 10.0, 30.0);
        int id3 = rm.addRegion("Chorus", 30.0, 50.0);
        expectEquals(rm.getNumRegions(), 3);
        expect(id1 != id2);
        expect(id2 != id3);

        // Verify sort order
        auto& regions = rm.getRegions();
        expect(regions[0].name == juce::String("Intro"));
        expect(regions[1].name == juce::String("Verse"));
        expect(regions[2].name == juce::String("Chorus"));

        // ── Get region at time ──
        beginTest("get region at time");
        expectEquals(rm.getRegionAt(5.0), id1);
        expectEquals(rm.getRegionAt(15.0), id2);
        expectEquals(rm.getRegionAt(40.0), id3);
        expectEquals(rm.getRegionAt(55.0), -1);  // outside any region
        expectEquals(rm.getRegionAt(-1.0), -1);  // negative time

        // ── Get region by ID ──
        beginTest("get region by ID");
        auto* r = rm.getRegionById(id2);
        expect(r != nullptr);
        expect(r->name == juce::String("Verse"));
        expectEquals(r->id, id2);
        expect(rm.getRegionById(999) == nullptr); // non-existent ID

        // ── Rename region ──
        beginTest("rename region");
        rm.renameRegion(id1, "Introduction");
        auto* renamed = rm.getRegionById(id1);
        expect(renamed->name == juce::String("Introduction"));

        // ── Navigation ──
        beginTest("navigation");
        expectEquals(rm.getNextRegion(id1), id2);
        expectEquals(rm.getNextRegion(id2), id3);
        expectEquals(rm.getNextRegion(id3), id1); // wraps around to first

        expectEquals(rm.getPrevRegion(id3), id2);
        expectEquals(rm.getPrevRegion(id2), id1);
        expectEquals(rm.getPrevRegion(id1), id3); // wraps around to last

        // ── Remove region ──
        beginTest("remove region");
        rm.removeRegion(id2);
        expectEquals(rm.getNumRegions(), 2);
        expect(rm.getRegionById(id2) == nullptr);

        // ── Remove region at time ──
        beginTest("remove region at time");
        rm.removeRegionAt(5.0);
        expectEquals(rm.getNumRegions(), 1);
        expect(rm.getRegions()[0].name == juce::String("Chorus"));

        // ── Current region ID ──
        beginTest("current region ID");
        rm.setCurrentRegionId(id3);
        expectEquals(rm.getCurrentRegionId(), id3);

        // ── Clear ──
        beginTest("clear");
        rm.clear();
        expectEquals(rm.getNumRegions(), 0);
        expectEquals(rm.getRegions().size(), (size_t)0);
    }
};

static RegionManagerTest regionManagerTest;
