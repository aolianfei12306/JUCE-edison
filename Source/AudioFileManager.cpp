#include "AudioFileManager.h"
#include <cmath>

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

    m_bitsPerSample = reader->bitsPerSample;

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

int AudioFileManager::removeRange(int startSample, int numSamples)
{
    if (!hasAudio() || m_buffer == nullptr || startSample < 0 || numSamples <= 0)
        return 0;

    const int totalSamples = m_buffer->getNumSamples();
    if (startSample >= totalSamples)
        return 0;

    // Clamp to buffer bounds
    const int safeNum = juce::jmin(numSamples, totalSamples - startSample);
    if (safeNum <= 0)
        return 0;

    const int newTotalSamples = totalSamples - safeNum;
    const int numChannels = m_buffer->getNumChannels();
    const int remainingSamples = totalSamples - (startSample + safeNum);

    if (remainingSamples > 0)
    {
        // Shift remaining audio left
        for (int ch = 0; ch < numChannels; ++ch)
        {
            float* buf = m_buffer->getWritePointer(ch);
            std::copy(buf + startSample + safeNum,
                      buf + totalSamples,
                      buf + startSample);
        }
    }

    // Shrink the buffer
    m_buffer->setSize(numChannels, newTotalSamples, false, false, true);

    // Update thumbnail
    m_thumbnail->clear();
    if (m_buffer->getNumSamples() > 0)
    {
        m_thumbnail->reset(numChannels, m_sampleRate, m_buffer->getNumSamples());
        m_thumbnail->addBlock(0, *m_buffer, 0, m_buffer->getNumSamples());
    }

    return safeNum;
}

void AudioFileManager::replaceBuffer(std::unique_ptr<juce::AudioBuffer<float>> newBuffer)
{
    jassert(newBuffer != nullptr);
    if (newBuffer == nullptr)
        return;

    m_buffer = std::move(newBuffer);

    // Update thumbnail
    m_thumbnail->clear();
    if (m_buffer->getNumSamples() > 0)
    {
        m_thumbnail->reset(m_buffer->getNumChannels(), m_sampleRate, m_buffer->getNumSamples());
        m_thumbnail->addBlock(0, *m_buffer, 0, m_buffer->getNumSamples());
    }
}

double AudioFileManager::snapToZeroCrossing(double timeSec, int maxSearchSamples) const noexcept
{
    if (!hasAudio() || m_buffer == nullptr || m_sampleRate <= 0.0)
        return timeSec;

    const int totalSamples = m_buffer->getNumSamples();
    const int centerSample = static_cast<int>(std::round(timeSec * m_sampleRate));

    if (centerSample < 0 || centerSample >= totalSamples)
        return timeSec;

    // Use channel 0 for zero-crossing search
    const float* samples = m_buffer->getReadPointer(0);
    if (samples == nullptr)
        return timeSec;

    // Search backward from centerSample
    int bestBackward = -1;
    for (int i = centerSample; i > 0 && i > centerSample - maxSearchSamples; --i)
    {
        // Check if there's a sign change between samples[i-1] and samples[i]
        if ((samples[i - 1] <= 0.0f && samples[i] > 0.0f) ||
            (samples[i - 1] >= 0.0f && samples[i] < 0.0f))
        {
            bestBackward = i;
            break;
        }
    }

    // Search forward from centerSample
    int bestForward = -1;
    for (int i = centerSample; i < totalSamples - 1 && i < centerSample + maxSearchSamples; ++i)
    {
        if ((samples[i] <= 0.0f && samples[i + 1] > 0.0f) ||
            (samples[i] >= 0.0f && samples[i + 1] < 0.0f))
        {
            bestForward = i;
            break;
        }
    }

    // Pick nearest zero crossing
    int bestSample = -1;
    if (bestBackward >= 0 && bestForward >= 0)
    {
        int distBack = centerSample - bestBackward;
        int distFwd  = bestForward - centerSample;
        bestSample = (distBack <= distFwd) ? bestBackward : bestForward;
    }
    else if (bestBackward >= 0)
    {
        bestSample = bestBackward;
    }
    else if (bestForward >= 0)
    {
        bestSample = bestForward;
    }

    if (bestSample < 0)
        return timeSec; // no zero crossing found - return original time

    // Sub-sample interpolation: find exact crossing position between bestSample-1 and bestSample
    if (bestSample > 0 && bestSample < totalSamples)
    {
        float a = samples[bestSample - 1];
        float b = samples[bestSample];
        if (std::abs(a - b) > 1e-10f)
        {
            double t = static_cast<double>(-a) / static_cast<double>(b - a);
            if (t >= 0.0 && t <= 1.0)
                return (static_cast<double>(bestSample - 1) + t) / m_sampleRate;
        }
    }

    return static_cast<double>(bestSample) / m_sampleRate;
}
