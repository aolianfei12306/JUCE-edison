#include "SpectrogramComponent.h"
#include "AudioFileManager.h"
#include <cmath>
#include <algorithm>

// ── Colour helpers ──

juce::Colour SpectrogramComponent::viridisColour(float t)
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
}

// ── Viewport-driven STFT rendering ──
//
// Instead of precomputing the full spectrogram into a large memory buffer
// (which blows up for long audio files), this method computes FFT only
// for the visible time range and renders directly to a viewport-sized image.
//
// When zoomed out (many audio samples per pixel), multiple FFT frames are
// max-pooled per pixel column, capturing transients without memory explosion.
//
// This supports arbitrarily long audio files (tested up to 1+ hour).

void SpectrogramComponent::renderViewport(int viewWidth, int viewHeight)
{
    auto* buffer = m_fileManager.getBuffer();
    if (!buffer || buffer->getNumSamples() == 0 || viewWidth <= 0 || viewHeight <= 0)
    {
        m_viewportImage = juce::Image{};
        return;
    }

    const float* data = buffer->getReadPointer(0);
    int totalSamples   = buffer->getNumSamples();
    double sampleRate  = m_fileManager.getSampleRate();
    double totalDur    = m_fileManager.getDurationSec();
    double viewDur     = totalDur / m_hZoom;
    double startTime   = m_viewOffset;

    // Clamp start so we don't render past end, and sync back to m_viewOffset
    if (startTime + viewDur > totalDur)
        startTime = std::max(0.0, totalDur - viewDur);
    if (startTime != m_viewOffset)
        m_viewOffset = startTime;
    double endTime = startTime + viewDur;

    int startSample = std::clamp(static_cast<int>(startTime * sampleRate), 0, totalSamples - 1);
    int endSample   = std::clamp(static_cast<int>(endTime   * sampleRate), startSample + 1, totalSamples);

    int numFreqBins = fftSize / 2;

    // Allocate viewport image
    m_viewportImage = juce::Image(juce::Image::RGB, viewWidth, viewHeight, true);

    // FFT scratch buffer (reused per pixel column)
    std::vector<float> fftBuf(fftSize * 2, 0.0f);
    std::vector<float> colMaxMag(numFreqBins, 0.0f);

    // ── Per-column STFT with max-hold pooling ──
    //
    // For each pixel column covering [pxStartTime, pxEndTime]:
    //   - Compute all FFT frames within that time slice
    //   - Take the maximum magnitude per frequency bin (max-hold pool)
    //
    // When zoomed out, stride increases to bound total FFTs to roughly
    // (1-2x viewWidth), keeping performance smooth for any audio length.

    // Calculate adaptive stride: at most ~2x viewWidth FFT frames total
    int totalVisibleFrames = (endSample - startSample - fftSize) / hopSize + 1;
    int stride = hopSize;
    if (totalVisibleFrames > viewWidth * 2)
    {
        // Multiply hop size so we compute roughly 2*viewWidth frames total
        int factor = totalVisibleFrames / (viewWidth * 2) + 1;
        stride = hopSize * factor;
        // Cap stride at fftSize to avoid losing all transients
        stride = std::min(stride, fftSize);
    }

    // Pre-compute FFT for the visible range (all frames, then map to pixels)
    // We store per-frame max magnitudes across the visible range
    int actualFrames = 0;
    for (int s = startSample; s + fftSize < endSample; s += stride)
        ++actualFrames;

    if (actualFrames == 0)
    {
        m_viewportImage.clear(m_viewportImage.getBounds(), juce::Colour(0xFF0D0D1A));
        return;
    }

    // Buffer: frames × freqBins
    std::vector<std::vector<float>> frameMags(actualFrames, std::vector<float>(numFreqBins, 0.0f));
    float maxGlobalMag = 0.0f;

    int frameIdx = 0;
    for (int s = startSample; s + fftSize < endSample && frameIdx < actualFrames; s += stride)
    {
        std::fill(fftBuf.begin(), fftBuf.end(), 0.0f);

        // Apply window
        for (int i = 0; i < fftSize; ++i)
        {
            int idx = s + i;
            float sample = (idx < totalSamples) ? data[idx] : 0.0f;
            fftBuf[i * 2] = sample * m_window[i];
        }

        m_fft->performRealOnlyForwardTransform(fftBuf.data(), true);

        for (int f = 0; f < numFreqBins; ++f)
        {
            float re = fftBuf[f * 2];
            float im = fftBuf[f * 2 + 1];
            float mag = std::sqrt(re * re + im * im);
            frameMags[frameIdx][f] = mag;
            if (mag > maxGlobalMag)
                maxGlobalMag = mag;
        }
        ++frameIdx;
    }
    actualFrames = frameIdx; // in case of early exit

    if (maxGlobalMag < 1e-10f)
        maxGlobalMag = 1.0f;

    float maxDB = juce::Decibels::gainToDecibels(maxGlobalMag);

    // ── Map frames to pixel columns with max-hold pooling ──
    //
    // Each pixel column covers a time range. We find which FFT frames
    // fall in that range and take the maximum magnitude per bin.
    double pxDuration = viewDur / viewWidth;

    for (int px = 0; px < viewWidth; ++px)
    {
        double pxStartTime = startTime + px * pxDuration;
        double pxEndTime   = pxStartTime + pxDuration;

        // Find FFT frame range for this pixel
        double frameDuration = stride / sampleRate;
        int frameStart = static_cast<int>(pxStartTime / frameDuration);
        int frameEnd   = static_cast<int>(pxEndTime   / frameDuration);

        // Adjust for the fact our FFT frames start at startTime
        int baseFrameOffset = static_cast<int>(startTime / frameDuration);
        frameStart = std::clamp(frameStart - baseFrameOffset, 0, actualFrames - 1);
        frameEnd   = std::clamp(frameEnd   - baseFrameOffset, frameStart, actualFrames - 1);

        // Max-hold pool across frames for this pixel
        std::fill(colMaxMag.begin(), colMaxMag.end(), 0.0f);
        for (int f = frameStart; f < frameEnd; ++f)
        {
            for (int b = 0; b < numFreqBins; ++b)
            {
                if (frameMags[f][b] > colMaxMag[b])
                    colMaxMag[b] = frameMags[f][b];
            }
        }

        // Render column — fill ALL pixel rows without gaps
        // by iterating Ys and finding the nearest bin for each row
        for (int y = 0; y < viewHeight; ++y)
        {
            // Map Y to frequency bin (low freq = bottom of viewport)
            float binFrac = 1.0f - static_cast<float>(y) / static_cast<float>(viewHeight);
            int b = static_cast<int>(binFrac * numFreqBins);
            b = std::clamp(b, 0, numFreqBins - 1);

            float mag = colMaxMag[b];
            float dB = (mag > 1e-10f)
                           ? juce::Decibels::gainToDecibels(mag)
                           : (maxDB - 80.0f);
            float t = (dB - (maxDB - 80.0f)) / 80.0f;
            t = std::clamp(t, 0.0f, 1.0f);

            m_viewportImage.setPixelAt(px, y, viridisColour(t));
        }
    }
}

// ── Public API ──

void SpectrogramComponent::rebuildFromAudio()
{
    refreshViewport();
}

void SpectrogramComponent::refreshViewport()
{
    m_dirty = true;
    repaint();
}

void SpectrogramComponent::clear()
{
    m_viewportImage = juce::Image{};
    m_dirty = true;
    repaint();
}

void SpectrogramComponent::resized()
{
    // Viewport image will be re-rendered on next paint
    m_dirty = true;
}

void SpectrogramComponent::paint(juce::Graphics& g)
{
    auto area = getLocalBounds().toFloat();
    g.fillAll(juce::Colour(0xFF0D0D1A));

    if (!m_fileManager.hasAudio())
    {
        g.setColour(juce::Colours::grey);
        g.setFont(14.0f);
        g.drawText("No audio loaded", area, juce::Justification::centred);
        return;
    }

    int viewW = getWidth();
    int viewH = getHeight();

    if (viewW <= 0 || viewH <= 0)
        return;

    // Re-render viewport if dirty or size changed
    if (m_dirty || !m_viewportImage.isValid()
        || m_viewportImage.getWidth() != viewW
        || m_viewportImage.getHeight() != viewH)
    {
        renderViewport(viewW, viewH);
        m_dirty = false;
    }

    if (!m_viewportImage.isValid())
    {
        g.drawText("(no spectrogram)", area, juce::Justification::centred);
        return;
    }

    // Draw the viewport image
    g.drawImage(m_viewportImage,
                0, 0, viewW, viewH,
                0, 0, viewW, viewH, false);

    // ── Playhead cursor ──
    if (m_playbackPosition >= 0.0)
    {
        double totalDur = m_fileManager.getDurationSec();
        double viewDur  = totalDur / m_hZoom;
        double startSec = m_viewOffset;
        double endSec   = startSec + viewDur;

        if (m_playbackPosition >= startSec && m_playbackPosition <= endSec)
        {
            float playheadX = static_cast<float>(
                (m_playbackPosition - startSec) / viewDur * area.getWidth());

            g.setColour(juce::Colours::white.withAlpha(0.85f));
            g.drawVerticalLine(static_cast<int>(playheadX), 0.0f, area.getHeight());

            float triSize = 8.0f;
            juce::Path tri;
            tri.addTriangle(playheadX, 0.0f,
                            playheadX - triSize, triSize,
                            playheadX + triSize, triSize);
            g.setColour(juce::Colours::cyan.withAlpha(0.7f));
            g.fillPath(tri);
        }
    }

    // ── Frequency axis labels on the right ──
    g.setFont(10.0f);
    g.setColour(juce::Colours::white.withAlpha(0.4f));

    double sampleRate = m_fileManager.getSampleRate();
    float labels[] = { 100, 500, 1000, 5000, 10000, static_cast<float>(sampleRate / 2) };
    for (float freq : labels)
    {
        if (freq > sampleRate / 2) continue;
        float normY = 1.0f - freq / static_cast<float>(sampleRate / 2);
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

void SpectrogramComponent::mouseWheelMove(const juce::MouseEvent& e,
                                           const juce::MouseWheelDetails& w)
{
    if (!m_fileManager.hasAudio()) return;

    double totalDuration = m_fileManager.getDurationSec();

    // Handle horizontal scroll (deltaX from trackpad)
    if (w.deltaX != 0.0)
    {
        double viewDuration = totalDuration / m_hZoom;
        double scrollAmount = -viewDuration * 0.1 * w.deltaX;
        double newOffset = m_viewOffset + scrollAmount;
        double maxOffset = totalDuration - viewDuration;
        m_viewOffset = std::clamp(newOffset, 0.0, std::max(0.0, maxOffset));
        refreshViewport();
        if (onUserViewChanged)
            onUserViewChanged(m_hZoom, m_viewOffset);
        return;
    }

    if (w.deltaY == 0.0) return;

    if (e.mods.isCtrlDown())
    {
        // Ctrl+wheel: fine zoom
        double factor = (w.deltaY > 0) ? 1.05 : 1.0 / 1.05;
        m_hZoom = std::max(0.1, m_hZoom * factor);

        // Clamp viewOffset so view doesn't extend past audio end
        double viewDuration = totalDuration / m_hZoom;
        double maxOffset = totalDuration - viewDuration;
        if (maxOffset > 0.0)
            m_viewOffset = std::clamp(m_viewOffset, 0.0, maxOffset);
        else
            m_viewOffset = 0.0;
    }
    else
    {
        // Plain wheel: horizontal pan/scroll
        double viewDuration = totalDuration / m_hZoom;
        double scrollAmount = viewDuration * 0.1 * w.deltaY;
        double newOffset = m_viewOffset + scrollAmount;
        double maxOffset = totalDuration - viewDuration;
        m_viewOffset = std::clamp(newOffset, 0.0, std::max(0.0, maxOffset));
    }

    refreshViewport();
    if (onUserViewChanged)
        onUserViewChanged(m_hZoom, m_viewOffset);
}

void SpectrogramComponent::setPlaybackPosition(double posSec) noexcept
{
    m_playbackPosition = posSec;

    if (posSec >= 0.0 && m_followPlayback && m_fileManager.hasAudio())
    {
        double totalDuration = m_fileManager.getDurationSec();
        double viewDuration = totalDuration / m_hZoom;
        double rightEdge = m_viewOffset + viewDuration;
        double threshold = viewDuration * 0.15;

        if (posSec > rightEdge - threshold)
        {
            double totalDur = m_fileManager.getDurationSec();
            m_viewOffset = posSec - viewDuration * 0.7;
            m_viewOffset = std::clamp(m_viewOffset, 0.0, std::max(0.0, totalDur - viewDuration));
            refreshViewport();
            return;
        }
    }

    repaint();
}

void SpectrogramComponent::setZoom(double hZoom) noexcept
{
    m_hZoom = std::max(0.1, hZoom);
    refreshViewport();
}

void SpectrogramComponent::setViewOffset(double offsetSec) noexcept
{
    double totalDur = m_fileManager.hasAudio() ? m_fileManager.getDurationSec() : 0.0;
    double viewDur  = totalDur / m_hZoom;
    double maxOffset = totalDur - viewDur;
    if (maxOffset > 0.0)
        m_viewOffset = std::clamp(offsetSec, 0.0, maxOffset);
    else
        m_viewOffset = 0.0;
    refreshViewport();
}
