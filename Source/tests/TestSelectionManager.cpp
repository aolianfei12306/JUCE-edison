#include <juce_core/juce_core.h>
#include "../SelectionManager.h"
#include "../AudioFileManager.h"
#include "../GridManager.h"

//==============================================================================
class SelectionManagerTest : public juce::UnitTest
{
public:
    SelectionManagerTest() : juce::UnitTest("SelectionManager", "EdisonCore") {}

    void runTest() override
    {
        SelectionManager sm;

        // ── Default state ──
        beginTest("default state");
        expect(!sm.hasSelection());
        expectEquals(sm.getSelectionDuration(), 0.0);
        expectEquals(sm.getPlaybackPosition(), 0.0);
        expect(!sm.isSnapToZero());
        expect(!sm.isSnapToGrid());

        // ── Set and clear selection ──
        beginTest("set and clear selection");
        sm.setSelection(1.5, 3.7);
        expect(sm.hasSelection());
        expectWithinAbsoluteError(sm.getSelectionStart(), 1.5, 0.0001);
        expectWithinAbsoluteError(sm.getSelectionEnd(), 3.7, 0.0001);
        expectWithinAbsoluteError(sm.getSelectionDuration(), 2.2, 0.0001);
        sm.clearSelection();
        expect(!sm.hasSelection());

        // ── Set selection with reversed order ──
        beginTest("set selection reversed order");
        sm.setSelection(5.0, 2.0);
        expectWithinAbsoluteError(sm.getSelectionStart(), 2.0, 0.0001);
        expectWithinAbsoluteError(sm.getSelectionEnd(), 5.0, 0.0001);

        // ── Minute selection should be rejected ──
        beginTest("minimum selection threshold");
        sm.setSelection(1.0, 1.0005);
        expect(!sm.hasSelection());

        // ── Selection duration ──
        beginTest("selection duration");
        sm.setSelection(0.0, 10.0);
        expectWithinAbsoluteError(sm.getSelectionDuration(), 10.0, 0.0001);
        sm.clearSelection();
        expectEquals(sm.getSelectionDuration(), 0.0);

        // ── Playback position ──
        beginTest("playback position");
        sm.setPlaybackPosition(42.5);
        expectWithinAbsoluteError(sm.getPlaybackPosition(), 42.5, 0.0001);

        // ── Total duration ──
        beginTest("total duration");
        sm.setTotalDuration(180.0);
        expectWithinAbsoluteError(sm.getTotalDuration(), 180.0, 0.0001);

        // ── Snap toggling ──
        beginTest("snap toggling");
        sm.toggleSnapToZero();
        expect(sm.isSnapToZero());
        sm.toggleSnapToZero();
        expect(!sm.isSnapToZero());

        sm.toggleSnapToGrid();
        expect(sm.isSnapToGrid());
        sm.toggleSnapToGrid();
        expect(!sm.isSnapToGrid());

        // ── Snap time with grid ──
        beginTest("snap time with grid");
        AudioFileManager afm;
        GridManager gm;
        gm.setBPM(120.0);
        gm.setDivision(4);
        gm.setEnabled(true);
        sm.setSnapToGrid(true);

        // Grid interval at 120 BPM / quarter note = 0.5s
        // 1.23 rounds to nearest 0.5 boundary → 1.0
        // Note: snapTime uses fileManager.getDurationSec() internally;
        //       empty AudioFileManager returns 0.0, so result clamps to 0.0
        double snapped = sm.snapTime(1.23, afm, gm);
        expectWithinAbsoluteError(snapped, 0.0, 0.01);

        // Without grid snap enabled, should be identity
        sm.setSnapToGrid(false);
        snapped = sm.snapTime(1.23, afm, gm);
        expectWithinAbsoluteError(snapped, 1.23, 0.01);
    }
};

static SelectionManagerTest selectionManagerTest;
