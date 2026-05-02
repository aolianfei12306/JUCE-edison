#include "AudioFileManager.h"

AudioFileManager::AudioFileManager()
{
    m_formatManager.registerBasicFormats();
    m_thumbnailCache  = std::make_unique<juce::AudioThumbnailCache>(256);
    m_thumbnail       = std::make_unique<juce::AudioThumbnail>(1024, m_formatManager, *m_thumbnailCache);
}

bool AudioFileManager::loadFile(const juce::File& file)
{
    unload();

    auto reader = std::unique_ptr<juce::AudioFormatReader>(
        m_formatManager.createReaderFor(file));

    if (reader == nullptr)
        return false;

    m_file     = file;
    m_fileName = file.getFileName();
    m_sampleRate = reader->sampleRate;

    auto totalSamples = static_cast<int>(reader->lengthInSamples);
    auto numChannels  = static_cast<int>(reader->numChannels);

    m_buffer = std::make_unique<juce::AudioBuffer<float>>(numChannels, totalSamples);
    reader->read(m_buffer.get(), 0, totalSamples, 0, true, true);

    m_thumbnail->reset(numChannels, m_sampleRate, totalSamples);
    m_thumbnail->addBlock(0, *m_buffer, 0, totalSamples);

    return true;
}

void AudioFileManager::unload()
{
    m_buffer   = nullptr;
    m_thumbnail->clear();
    m_fileName.clear();
    m_file     = juce::File{};
    m_sampleRate = 44100.0;
}

double AudioFileManager::getDurationSec() const noexcept
{
    if (m_buffer == nullptr || m_sampleRate <= 0.0)
        return 0.0;
    return static_cast<double>(m_buffer->getNumSamples()) / m_sampleRate;
}
