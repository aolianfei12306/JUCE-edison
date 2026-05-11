#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_dsp/juce_dsp.h>
#include <memory>
#include <vector>

class AudioFileManager;

class SpectrogramComponent : public juce::Component
{
public:
    explicit SpectrogramComponent(AudioFileManager& fileManager);
    ~SpectrogramComponent() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& w) override;

    void rebuildFromAudio();
    void clear();
    void setPlaybackPosition(double posSec) noexcept;

    void setZoom(double hZoom) noexcept;
    double getZoom() const noexcept { return m_hZoom; }
    void setViewOffset(double offsetSec) noexcept { m_viewOffset = offsetSec; refreshViewport(); }
    double getViewOffset() const noexcept { return m_viewOffset; }

private:
    /** Render spectrogram for current viewport at given pixel dimensions.
     *  Uses lazy on-demand STFT: only computes FFT frames needed for visible
     *  time range, and max-pools multiple frames per pixel column when zoomed out.
     *  This avoids storing the full spectrogram in memory, enabling arbitrarily
     *  long audio files.
     */
    void renderViewport(int viewWidth, int viewHeight);

    /** Mark viewport image as dirty and schedule repaint. */
    void refreshViewport();

    static float hannWindow(int n, int N);
    static juce::Colour magnitudeToColour(float dB, float maxDB);
    static juce::Colour viridisColour(float t);

    AudioFileManager& m_fileManager;

    // STFT config
    static constexpr int fftOrder = 10;   // 1024-point FFT
    static constexpr int fftSize  = 1 << fftOrder;
    static constexpr int hopSize  = 256;
    std::unique_ptr<juce::dsp::FFT> m_fft;

    // Hann window (precomputed)
    std::vector<float> m_window;

    // Viewport-sized rendered image (rebuilt on zoom/scroll, no full-audio storage)
    juce::Image m_viewportImage;
    bool m_dirty = true;

    // View state
    double m_hZoom            = 1.0;
    double m_viewOffset       = 0.0;
    double m_playbackPosition = -1.0;
    bool   m_followPlayback   = true;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent)
};
