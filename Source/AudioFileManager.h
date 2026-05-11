#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <memory>

class AudioFileManager
{
public:
    AudioFileManager();
    ~AudioFileManager() = default;

    bool loadFile(const juce::File& file);
    void unload();

    bool hasAudio() const noexcept
    {
        return m_buffer != nullptr && m_buffer->getNumSamples() > 0;
    }

    juce::AudioBuffer<float>* getBuffer() const noexcept { return m_buffer.get(); }
    double  getSampleRate()    const noexcept { return m_sampleRate; }
    int     getBitsPerSample() const noexcept { return m_bitsPerSample; }
    int     getNumChannels()   const noexcept
    {
        return m_buffer ? m_buffer->getNumChannels() : 0;
    }
    double  getDurationSec() const noexcept;

    juce::AudioFormatManager& getFormatManager() noexcept { return m_formatManager; }
    juce::AudioThumbnail*     getThumbnail() noexcept { return m_thumbnail.get(); }

    const juce::String& getFileName() const noexcept { return m_fileName; }
    const juce::File&   getFile()     const noexcept { return m_file; }

    /** Find the nearest zero crossing to a given time position.
     *  Searches forward and backward within maxSearchSamples from the given time.
     *  Returns the time of the nearest zero crossing (sub-sample interpolated),
     *  or the original time if no zero crossing is found within range.
     */
    double snapToZeroCrossing(double timeSec, int maxSearchSamples = 128) const noexcept;

private:
    juce::AudioFormatManager                m_formatManager;
    std::unique_ptr<juce::AudioBuffer<float>> m_buffer;
    std::unique_ptr<juce::AudioThumbnail>     m_thumbnail;
    std::unique_ptr<juce::AudioThumbnailCache> m_thumbnailCache;
    double   m_sampleRate     = 44100.0;
    int      m_bitsPerSample  = 16;
    juce::String m_fileName;
    juce::File   m_file;
};
