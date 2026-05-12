#include <juce_core/juce_core.h>
#include "../GridManager.h"

//==============================================================================
class GridManagerTest : public juce::UnitTest
{
public:
    GridManagerTest() : juce::UnitTest("GridManager", "EdisonCore") {}

    void runTest() override
    {
        GridManager gm;

        // ── Default state ──
        beginTest("default state");
        expect(!gm.isEnabled());
        expectWithinAbsoluteError(gm.getBPM(), 120.0, 0.0001);
        expectEquals(gm.getDivision(), 4);

        // ── Enable/disable toggle ──
        beginTest("toggle enabled");
        gm.toggleEnabled();
        expect(gm.isEnabled());
        gm.toggleEnabled();
        expect(!gm.isEnabled());

        // ── BPM clamping ──
        beginTest("BPM clamping");
        gm.setBPM(10.0);   // below minimum 20
        expectWithinAbsoluteError(gm.getBPM(), 20.0, 0.0001);
        gm.setBPM(600.0);  // above maximum 500
        expectWithinAbsoluteError(gm.getBPM(), 500.0, 0.0001);
        gm.setBPM(140.0);
        expectWithinAbsoluteError(gm.getBPM(), 140.0, 0.0001);

        // ── Division clamping ──
        beginTest("division clamping");
        gm.setDivision(0);  // below minimum 1
        expectEquals(gm.getDivision(), 1);
        gm.setDivision(64); // above maximum 32
        expectEquals(gm.getDivision(), 32);
        gm.setDivision(8);
        expectEquals(gm.getDivision(), 8);

        // ── Beat duration ──
        beginTest("beat duration");
        gm.setBPM(120.0);
        expectWithinAbsoluteError(gm.getBeatDuration(), 0.5, 0.0001);
        gm.setBPM(60.0);
        expectWithinAbsoluteError(gm.getBeatDuration(), 1.0, 0.0001);
        gm.setBPM(240.0);
        expectWithinAbsoluteError(gm.getBeatDuration(), 0.25, 0.0001);

        // ── Grid interval ──
        beginTest("grid interval");
        gm.setBPM(120.0);

        gm.setDivision(4);   // quarter notes → 0.5s
        expectWithinAbsoluteError(gm.getGridInterval(), 0.5, 0.0001);

        gm.setDivision(8);   // eighth notes → 0.25s
        expectWithinAbsoluteError(gm.getGridInterval(), 0.25, 0.0001);

        gm.setDivision(16);  // sixteenth notes → 0.125s
        expectWithinAbsoluteError(gm.getGridInterval(), 0.125, 0.0001);

        gm.setDivision(1);   // whole notes → 2.0s
        expectWithinAbsoluteError(gm.getGridInterval(), 2.0, 0.0001);

        // ── Snap to grid ──
        beginTest("snap to grid");
        gm.setEnabled(true);
        gm.setBPM(120.0);
        gm.setDivision(4);  // interval = 0.5s

        expectWithinAbsoluteError(gm.snapToGrid(0.0, 10.0), 0.0, 0.0001);
        expectWithinAbsoluteError(gm.snapToGrid(0.2, 10.0), 0.0, 0.0001);
        expectWithinAbsoluteError(gm.snapToGrid(0.4, 10.0), 0.5, 0.0001);
        expectWithinAbsoluteError(gm.snapToGrid(1.2, 10.0), 1.0, 0.0001);
        expectWithinAbsoluteError(gm.snapToGrid(1.3, 10.0), 1.5, 0.0001);
        expectWithinAbsoluteError(gm.snapToGrid(9.9, 10.0), 10.0, 0.0001); // clamped

        // Snap disabled → identity
        gm.setEnabled(false);
        expectWithinAbsoluteError(gm.snapToGrid(1.23, 10.0), 1.23, 0.0001);

        // ── Get grid lines ──
        beginTest("get grid lines");
        gm.setEnabled(true);
        gm.setBPM(120.0);
        gm.setDivision(4);
        auto lines = gm.getGridLines(0.0, 4.0);
        expect(lines.size() >= 7); // 0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0

        if (lines.size() > 0)
        {
            expectWithinAbsoluteError(lines[0], 0.0, 0.0001);
            expectWithinAbsoluteError(lines[1], 0.5, 0.0001);
        }

        // ── Get bar lines ──
        beginTest("get bar lines");
        auto bars = gm.getBarLines(0.0, 4.0, 4);
        expect(bars.size() >= 2);
        if (bars.size() > 0)
        {
            expectWithinAbsoluteError(bars[0], 0.0, 0.0001);
            expectWithinAbsoluteError(bars[1], 2.0, 0.0001);
        }

        // ── Edge cases ──
        beginTest("edge cases");
        gm.setEnabled(true);
        expectWithinAbsoluteError(gm.snapToGrid(-1.0, 10.0), 0.0, 0.0001); // below 0 clamped
        expectWithinAbsoluteError(gm.snapToGrid(11.0, 10.0), 10.0, 0.0001); // above duration clamped

        // BPM = 0 should not snap
        gm.setBPM(20.0); // clamped to 20
        expect(gm.getBPM() >= 20.0);
    }
};

static GridManagerTest gridManagerTest;
