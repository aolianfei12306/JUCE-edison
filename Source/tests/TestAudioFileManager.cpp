#include <juce_core/juce_core.h>
#include "../AudioFileManager.h"
#include "../GridManager.h"
#include <cmath>

//==============================================================================
class AudioFileManagerTest : public juce::UnitTest
{
public:
    AudioFileManagerTest() : juce::UnitTest("AudioFileManager", "EdisonCore") {}

    void runTest() override
    {
        AudioFileManager afm;

        beginTest("default state");
        expect(!afm.hasAudio());
        expectEquals(afm.getNumChannels(), 0);
        expectWithinAbsoluteError(afm.getDurationSec(), 0.0, 0.0001);
        expect(afm.getBuffer() == nullptr);
        // Thumbnail is created in constructor even without audio
        expect(afm.getThumbnail() != nullptr);
        expect(afm.getFileName().isEmpty());
        expect(!afm.getFile().exists());

        beginTest("unload when empty");
        afm.unload();
        expect(!afm.hasAudio());
    }
};

static AudioFileManagerTest audioFileManagerTest;

//==============================================================================
class SnapToZeroTest : public juce::UnitTest
{
public:
    SnapToZeroTest() : juce::UnitTest("SnapToZero", "EdisonAudio") {}

    void runTest() override
    {
        AudioFileManager afm;

        beginTest("snap with no audio loaded returns identity");
        double result = afm.snapToZeroCrossing(1.0);
        expectWithinAbsoluteError(result, 1.0, 0.0001);
    }
};

static SnapToZeroTest snapToZeroTest;
