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
    double  getSampleRate()  const noexcept { return m_sampleRate; }
    int     getNumChannels() const noexcept
    {
        return m_buffer ? m_buffer->getNumChannels() : 0;
    }
    double  getDurationSec() const noexcept;

    juce::AudioFormatManager& getFormatManager() noexcept { return m_formatManager; }
    juce::AudioThumbnail*     getThumbnail() noexcept { return m_thumbnail.get(); }

    const juce::String& getFileName() const noexcept { return m_fileName; }
    const juce::File&   getFile()     const noexcept { return m_file; }

private:
    juce::AudioFormatManager                m_formatManager;
    std::unique_ptr<juce::AudioBuffer<float>> m_buffer;
    std::unique_ptr<juce::AudioThumbnail>     m_thumbnail;
    std::unique_ptr<juce::AudioThumbnailCache> m_thumbnailCache;
    double   m_sampleRate = 44100.0;
    juce::String m_fileName;
    juce::File   m_file;
};
