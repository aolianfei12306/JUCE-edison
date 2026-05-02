#include "SpectrogramComponent.h"
#include "AudioFileManager.h"
#include <cmath>
#include <algorithm>

// ── Colour helpers ──

static juce::Colour viridisColour(float t)
{
    // A simple blue->purple->green->yellow gradient inspired by viridis
    t = std::clamp(t, 0.0f, 1.0f);
    if (t < 0.25f)
    {
        float u = t / 0.25f;
        return juce::Colour::fromFloatRGBA(
            0.267f * u, 0.004f * u, 0.329f + 0.167f * u, 1.0f);
    }
    else if (t < 0.50f)
    {
        float u = (t - 0.25f) / 0.25f;
        return juce::Colour::fromFloatRGBA(
            0.267f + 0.132f * u, 0.004f + 0.315f * u, 0.496f - 0.127f * u, 1.0f);
    }
    else if (t < 0.75f)
    {
        float u = (t - 0.50f) / 0.25f;
        return juce::Colour::fromFloatRGBA(
            0.399f + 0.208f * u, 0.319f + 0.352f * u, 0.369f - 0.188f * u, 1.0f);
    }
    else
    {
        float u = (t - 0.75f) / 0.25f;
        return juce::Colour::fromFloatRGBA(
            0.607f + 0.283f * u, 0.671f + 0.170f * u, 0.181f - 0.086f * u, 1.0f);
    }
}

juce::Colour SpectrogramComponent::magnitudeToColour(float dB, float maxDB)
{
    // Map dB range [maxDB - 80, maxDB] -> [0, 1]
    float t = (dB - (maxDB - 80.0f)) / 80.0f;
    t = std::clamp(t, 0.0f, 1.0f);
    return viridisColour(t);
}

// ── Hann window ──

float SpectrogramComponent::hannWindow(int n, int N)
{
    return 0.5f * (1.0f - std::cos(2.0f * juce::MathConstants<float>::pi * n / (N - 1)));
}

// ── Construction ──

SpectrogramComponent::SpectrogramComponent(AudioFileManager& fileManager)
    : m_fileManager(fileManager)
{
    m_fft = std::make_unique<juce::dsp::FFT>(fftOrder);

    // Precompute Hann window
    m_window.resize(fftSize);
    for (int i = 0; i < fftSize; ++i)
        m_window[i] = hannWindow(i, fftSize);

    setInterceptsMouseClicks(false, false);
}

// ── STFT computation ──

void SpectrogramComponent::computeSTFT()
{
    auto* buffer = m_fileManager.getBuffer();
    if (!buffer || buffer->getNumSamples() == 0)
    {
        m_spectrogramData.clear();
        m_numTimeFrames = 0;
        return;
    }

    // Use only the first channel
    const float* data = buffer->getReadPointer(0);
    int totalSamples = buffer->getNumSamples();

    // Cap frames to prevent memory blowout (max ~10 min at 44.1kHz)
    int maxFrames = 10000;
    m_numTimeFrames = std::min(
        ((totalSamples - fftSize) / hopSize) + 1,
        maxFrames);

    m_numFreqBins = fftSize / 2;
    m_spectrogramData.resize(m_numTimeFrames);
    for (auto& row : m_spectrogramData)
        row.resize(m_numFreqBins, 0.0f);

    // FFT scratch buffer (real + imaginary interleaved)
    std::vector<float> fftBuf(fftSize * 2, 0.0f);

    float maxMagnitude = 0.0f;

    for (int t = 0; t < m_numTimeFrames; ++t)
    {
        int offset = t * hopSize;

        // Window and fill real part
        std::fill(fftBuf.begin(), fftBuf.end(), 0.0f);
        for (int i = 0; i < fftSize; ++i)
        {
            float sample = (offset + i < totalSamples) ? data[offset + i] : 0.0f;
            fftBuf[i * 2] = sample * m_window[i];
        }

        m_fft->performRealOnlyForwardTransform(fftBuf.data(), true);

        // Extract magnitudes (only positive frequencies)
        for (int f = 0; f < m_numFreqBins; ++f)
        {
            float re = fftBuf[f * 2];
            float im = fftBuf[f * 2 + 1];
            float mag = std::sqrt(re * re + im * im);
            m_spectrogramData[t][f] = mag;
            if (mag > maxMagnitude)
                maxMagnitude = mag;
        }
    }

    // Convert to dB scale (normalized to 0..1 for max magnitude)
    float dBFloor = -80.0f;
    float maxDB = (maxMagnitude > 1e-10f) ? juce::Decibels::gainToDecibels(maxMagnitude) : 0.0f;

    for (int t = 0; t < m_numTimeFrames; ++t)
    {
        for (int f = 0; f < m_numFreqBins; ++f)
        {
            float dB = (m_spectrogramData[t][f] > 1e-10f)
                           ? juce::Decibels::gainToDecibels(m_spectrogramData[t][f])
                           : dBFloor;
            // Store as normalized dB value for color mapping
            m_spectrogramData[t][f] = (dB - maxDB + 80.0f) / 80.0f;
        }
    }
}

// ── Image rendering ──

void SpectrogramComponent::renderImage()
{
    if (m_numTimeFrames == 0 || m_numFreqBins == 0)
    {
        m_spectrogramImage = juce::Image{};
        return;
    }

    const int imageWidth  = std::max(m_numTimeFrames, 1);
    const int imageHeight = std::max(m_numFreqBins, 1);

    m_spectrogramImage = juce::Image(juce::Image::RGB,
                                      imageWidth, imageHeight, true);

    for (int t = 0; t < m_numTimeFrames; ++t)
    {
        for (int f = 0; f < m_numFreqBins; ++f)
        {
            float val = m_spectrogramData[t][f];
            // Flip Y so low frequencies are at bottom
            m_spectrogramImage.setPixelAt(t, m_numFreqBins - 1 - f,
                                           viridisColour(val));
        }
    }
}

// ── Public API ──

void SpectrogramComponent::rebuildFromAudio()
{
    m_spectrogramData.clear();
    m_spectrogramImage = juce::Image{};
    m_numTimeFrames = 0;

    if (m_fileManager.hasAudio())
    {
        computeSTFT();
        renderImage();
    }

    m_dirty = false;
    repaint();
}

void SpectrogramComponent::clear()
{
    m_spectrogramData.clear();
    m_spectrogramImage = juce::Image{};
    m_numTimeFrames = 0;
    m_dirty = true;
    repaint();
}

void SpectrogramComponent::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    g.fillAll(juce::Colour(0xFF0D0D1A));

    if (!m_spectrogramImage.isValid())
    {
        // No spectrogram: draw placeholder text
        g.setColour(juce::Colours::grey);
        g.setFont(14.0f);
        g.drawText("No audio loaded", area, juce::Justification::centred);
        return;
    }

    // Calculate visible region
    double totalDurationSec = m_fileManager.getDurationSec();
    if (totalDurationSec <= 0.0) return;

    int totalFrames  = m_numTimeFrames;
    double timePerFrame = static_cast<double>(hopSize) / m_fileManager.getSampleRate();

    double viewDuration = totalDurationSec / m_hZoom;
    double startSec     = m_viewOffset;
    double endSec       = startSec + viewDuration;

    int startFrame = static_cast<int>(startSec / timePerFrame);
    int endFrame   = static_cast<int>(endSec   / timePerFrame);
    startFrame = std::clamp(startFrame, 0, totalFrames - 1);
    endFrame   = std::clamp(endFrame,   startFrame + 1, totalFrames);

    int visibleFrames = endFrame - startFrame;
    if (visibleFrames <= 0) visibleFrames = 1;

    // Paint the visible portion of the spectrogram
    float srcW = static_cast<float>(visibleFrames);
    float srcH = static_cast<float>(m_numFreqBins);

    g.drawImage(m_spectrogramImage,
                0, 0, area.getWidth(), area.getHeight(),   // dest
                static_cast<float>(startFrame), 0.0f, srcW, srcH); // src

    // Draw frequency axis labels on the right
    g.setFont(10.0f);
    g.setColour(juce::Colours::white.withAlpha(0.4f));

    double sampleRate = m_fileManager.getSampleRate();
    // Draw tick marks at major frequency points
    float labels[] = { 100, 500, 1000, 5000, 10000, (float)(sampleRate / 2) };
    for (float freq : labels)
    {
        if (freq > sampleRate / 2) continue;
        float normY = 1.0f - freq / (float)(sampleRate / 2);
        float y = normY * area.getHeight();
        g.drawHorizontalLine(static_cast<int>(y), area.getWidth() - 50, area.getWidth() - 5);
        juce::String label;
        if (freq >= 1000)
            label = juce::String(freq / 1000.0, 1) + "k";
        else
            label = juce::String(static_cast<int>(freq));
        g.drawText(label,
                   area.getWidth() - 50, y - 6, 45, 12,
                   juce::Justification::centredRight);
    }
}

void SpectrogramComponent::resized()
{
    // Image is pre-rendered at FFT grid resolution; no per-resize re-render.
}

void SpectrogramComponent::setZoom(double hZoom) noexcept
{
    m_hZoom = std::max(0.1, hZoom);
    repaint();
}
