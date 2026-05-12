#include <juce_core/juce_core.h>
#include "../MarkerManager.h"

//==============================================================================
class MarkerManagerTest : public juce::UnitTest
{
public:
    MarkerManagerTest() : juce::UnitTest("MarkerManager", "EdisonCore") {}

    void runTest() override
    {
        MarkerManager mm;

        // ── Default state ──
        beginTest("default state");
        expectEquals(mm.getNumMarkers(), 0);
        expectEquals(mm.getMarkers().size(), (size_t)0);

        // ── Add markers ──
        beginTest("add markers");
        mm.addMarker(1.5);
        mm.addMarker(3.0);
        mm.addMarker(0.5);
        expectEquals(mm.getNumMarkers(), 3);

        // Verify sorted order (sorted by time, not by ID)
        auto& markers = mm.getMarkers();
        expectWithinAbsoluteError(markers[0].timeSeconds, 0.5, 0.0001);
        expectWithinAbsoluteError(markers[1].timeSeconds, 1.5, 0.0001);
        expectWithinAbsoluteError(markers[2].timeSeconds, 3.0, 0.0001);

        // Verify labels (IDs: M1=1.5, M2=3.0, M3=0.5, sorted by time: M3, M1, M2)
        expect(markers[0].label == juce::String("M3"));  // 0.5s
        expect(markers[1].label == juce::String("M1"));  // 1.5s
        expect(markers[2].label == juce::String("M2"));  // 3.0s

        // ── Get marker at position ──
        beginTest("get marker at position");
        expect(mm.getMarkerAt(1.5) > 0);   // exact match
        expect(mm.getMarkerAt(1.51) > 0);   // within tolerance
        expectEquals(mm.getMarkerAt(1.6), -1); // outside tolerance

        // ── Navigation ──
        beginTest("navigation");
        // getNextMarker
        expectWithinAbsoluteError(mm.getNextMarker(0.0), 0.5, 0.0001);
        expectWithinAbsoluteError(mm.getNextMarker(0.6), 1.5, 0.0001);
        expectWithinAbsoluteError(mm.getNextMarker(2.0), 3.0, 0.0001);
        expectEquals(mm.getNextMarker(3.5), -1.0); // no next marker

        // getPrevMarker
        expectEquals(mm.getPrevMarker(0.0), -1.0); // no prev marker
        expectWithinAbsoluteError(mm.getPrevMarker(1.0), 0.5, 0.0001);
        expectWithinAbsoluteError(mm.getPrevMarker(2.0), 1.5, 0.0001);
        expectWithinAbsoluteError(mm.getPrevMarker(4.0), 3.0, 0.0001);

        // ── Remove marker by position ──
        beginTest("remove marker by position");
        mm.removeMarkerAt(1.5);
        expectEquals(mm.getNumMarkers(), 2);

        // Verify remaining markers
        auto& remaining = mm.getMarkers();
        expectWithinAbsoluteError(remaining[0].timeSeconds, 0.5, 0.0001);
        expectWithinAbsoluteError(remaining[1].timeSeconds, 3.0, 0.0001);

        // ── Remove marker by ID ──
        beginTest("remove marker by ID");
        int id = remaining[0].id;
        mm.removeMarker(id);
        expectEquals(mm.getNumMarkers(), 1);

        // ── Clear ──
        beginTest("clear");
        mm.clear();
        expectEquals(mm.getNumMarkers(), 0);
        expectEquals(mm.getMarkers().size(), (size_t)0);

        // Verify ID reset: first marker after clear should be M1 again
        mm.addMarker(10.0);
        expect(mm.getMarkers()[0].label == juce::String("M1"));
        expectEquals(mm.getMarkers()[0].id, 1);

        // ── Marker labels ──
        beginTest("marker labels");
        mm.addMarker(1.0);  // m_nextId=2, label="M2"
        mm.addMarker(2.5);  // m_nextId=3, label="M3"
        auto labels = mm.getMarkerLabels();
        expectEquals(labels.size(), 3);
        // After adding 1.0 and 2.5, sorted: 1.0(M2), 2.5(M3), 10.0(M1)
        expect(labels[0].contains("M2"));
        expect(labels[0].contains("1.00s"));
    }
};

static MarkerManagerTest markerManagerTest;
