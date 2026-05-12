/**
 * Test runner for Open Edison unit tests.
 * JUCE's UnitTestRunner executes all registered UnitTest subclasses.
 */
#include <juce_core/juce_core.h>

class TestRunner : public juce::UnitTestRunner
{
public:
    TestRunner()
    {
        setAssertOnFailure(false);
    }
};

int main()
{
    TestRunner runner;

    // Only run Open Edison tests (categorized as "EdisonCore" and "EdisonAudio")
    runner.runTestsInCategory("EdisonCore");
    runner.runTestsInCategory("EdisonAudio");

    auto numTests = runner.getNumResults();
    int passed = 0, failed = 0;

    for (int i = 0; i < static_cast<int>(numTests); ++i)
    {
        auto* r = runner.getResult(i);
        if (r != nullptr)
        {
            if (r->failures > 0) failed += r->failures;
            else                 passed += r->passes;
        }
    }

    juce::Logger::outputDebugString(
        "========================================\n"
        "  Open Edison Test Summary\n"
        "========================================\n"
        "  Passed: " + juce::String(passed) + "\n"
        "  Failed: " + juce::String(failed) + "\n"
        "========================================\n");

    return failed > 0 ? 1 : 0;
}
