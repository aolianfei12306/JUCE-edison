#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_data_structures/juce_data_structures.h>
#include <functional>

class AudioFileManager;

//==============================================================================
/**
 * An UndoableAction that modifies a region of the audio buffer.
 * Stores the original samples before modification for undo/redo.
 *
 * Usage:
 *   auto action = new AudioModifyAction(*fileManager, startSample, numSamples,
 *       [](AudioBuffer<float>& buf, int start, int num) {
 *           // silence the region
 *           for (int ch = 0; ch < buf.getNumChannels(); ++ch)
 *               buf.clear(ch, start, num);
 *       });
 *   undoManager->perform(action);
 */
class AudioModifyAction : public juce::UndoableAction
{
public:
    using Processor = std::function<void(juce::AudioBuffer<float>&,
                                          int startSample,
                                          int numSamples)>;

    AudioModifyAction(AudioFileManager& mgr,
                      int startSample,
                      int numSamples,
                      Processor proc);

    bool perform() override;
    bool undo() override;

    int getSizeInUnits() override { return static_cast<int>(sizeof(*this)); }

    /** Notify WaveformThumbnail and SpectrogramComponent that audio changed.
     *  Set before constructing so the action can broadcast after perform/undo.
     */
    std::function<void()> onAudioChanged;

private:
    void saveOriginalSamples();

    AudioFileManager& m_manager;
    int m_startSample;
    int m_numSamples;
    juce::AudioBuffer<float> m_originalSamples;
    Processor m_processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioModifyAction)
};

//==============================================================================
/**
 * An UndoableAction that removes (ripple-deletes) a range of samples from the
 * audio buffer and shifts subsequent audio left. Saves the full pre-edit buffer
 * so undo can completely restore the original state.
 *
 * Used by: Delete/Backspace ripple delete, Crop (keep selection, remove rest).
 */
class BufferSizeChangeAction : public juce::UndoableAction
{
public:
    /** Mode: RIPPLE_DELETE removes start..start+num from buffer, shifting rest left.
     *        CROP retains only start..start+num, removing everything else. */
    enum class Mode { RIPPLE_DELETE, CROP };

    BufferSizeChangeAction(AudioFileManager& mgr,
                           int startSample,
                           int numSamples,
                           Mode mode);

    bool perform() override;
    bool undo() override;
    int getSizeInUnits() override { return static_cast<int>(sizeof(*this)); }

    std::function<void()> onAudioChanged;

private:
    AudioFileManager& m_manager;
    int m_startSample;
    int m_numSamples;
    Mode m_mode;
    std::unique_ptr<juce::AudioBuffer<float>> m_savedBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BufferSizeChangeAction)
};
