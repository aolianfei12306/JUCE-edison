#include <juce_core/juce_core.h>
#include "../LoopManager.h"

//==============================================================================
class LoopManagerTest : public juce::UnitTest
{
public:
    LoopManagerTest() : juce::UnitTest("LoopManager", "EdisonCore") {}

    void runTest() override
    {
        LoopManager lm;

        // ── Default state ──
        beginTest("default state");
        expect(!lm.isLoopEnabled());
        expect(!lm.isActive());
        expect(!lm.hasValidLoop());

        // ── Enable loop ──
        beginTest("enable/disable loop");
        lm.setLoopEnabled(true);
        expect(lm.isLoopEnabled());
        expect(!lm.hasValidLoop()); // no range set yet

        // ── Set loop range ──
        beginTest("set loop range");
        lm.setLoopRange(2.0, 5.0);
        expect(lm.hasValidLoop());
        expectWithinAbsoluteError(lm.getLoopStart(), 2.0, 0.0001);
        expectWithinAbsoluteError(lm.getLoopEnd(), 5.0, 0.0001);

        // ── Reversed range ──
        beginTest("reversed range");
        lm.setLoopRange(8.0, 3.0);
        expectWithinAbsoluteError(lm.getLoopStart(), 3.0, 0.0001);
        expectWithinAbsoluteError(lm.getLoopEnd(), 8.0, 0.0001);

        // ── Active state ──
        beginTest("active state");
        expect(!lm.isActive());
        lm.setActive(true);
        expect(lm.isActive());

        // ── Disable disables active ──
        beginTest("disable clears active");
        lm.setActive(true);
        lm.setLoopEnabled(false);
        expect(!lm.isActive());
        expect(!lm.isLoopEnabled());

        // ── Clear loop ──
        beginTest("clear loop");
        lm.setLoopEnabled(true);
        lm.setLoopRange(1.0, 4.0);
        lm.clearLoop();
        expect(!lm.isLoopEnabled());
        expect(!lm.isActive());
        expect(!lm.hasValidLoop());

        // ── Edge cases ──
        beginTest("edge cases");
        lm.setLoopEnabled(true);
        lm.setLoopRange(5.0, 5.0);
        expect(!lm.hasValidLoop()); // zero-length loop

        lm.setLoopRange(5.0, 5.0005);
        expect(!lm.hasValidLoop()); // below minimum threshold
    }
};

static LoopManagerTest loopManagerTest;
