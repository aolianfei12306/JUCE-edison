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

    void rebuildFromAudio();
    void clear();

    void setZoom(double hZoom) noexcept;
    double getZoom() const noexcept { return m_hZoom; }
    void setViewOffset(double offsetSec) noexcept { m_viewOffset = offsetSec; }
    double getViewOffset() const noexcept { return m_viewOffset; }

private:
    void computeSTFT();
    void renderImage();
    static float hannWindow(int n, int N);
    static juce::Colour magnitudeToColour(float dB, float maxDB);

    AudioFileManager& m_fileManager;

    // STFT
    static constexpr int fftOrder = 10;   // 1024-point FFT
    static constexpr int fftSize  = 1 << fftOrder;
    static constexpr int hopSize  = 256;
    std::unique_ptr<juce::dsp::FFT> m_fft;

    // Window function
    std::vector<float> m_window;

    // Spectrogram data: data[t][f] where t = time frame, f = frequency bin (0..fftSize/2)
    std::vector<std::vector<float>> m_spectrogramData;
    int m_numTimeFrames = 0;
    int m_numFreqBins   = fftSize / 2; // only positive frequencies

    // Rendered image cache
    juce::Image m_spectrogramImage;
    bool m_dirty = true;

    // View state
    double m_hZoom      = 1.0;
    double m_viewOffset = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrogramComponent)
};
