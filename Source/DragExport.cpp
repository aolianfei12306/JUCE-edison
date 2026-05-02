#include "DragExport.h"
#include "SelectionManager.h"
#include "AudioFileManager.h"
#include <juce_audio_formats/juce_audio_formats.h>

DragExport::DragExport(SelectionManager& selection, AudioFileManager& fileManager)
    : m_selection(selection), m_fileManager(fileManager) {}

void DragExport::startDragIfOverSelection(const juce::MouseEvent& /*e*/,
                                           juce::Component* sourceComponent)
{
    if (!m_selection.hasSelection() || !m_fileManager.hasAudio()) return;

    juce::File tempFile = exportSelectionAsWav(
        juce::File::getSpecialLocation(juce::File::tempDirectory));

    if (tempFile.existsAsFile())
    {
        performExternalDragDropOfFiles({tempFile.getFullPathName().toStdString()},
                                       false, sourceComponent);
    }
}

juce::File DragExport::exportSelectionAsWav(const juce::File& destDir)
{
    if (!m_selection.hasSelection() || !m_fileManager.hasAudio()) return {};

    auto* buffer = m_fileManager.getBuffer();
    if (!buffer) return {};

    double sr   = m_fileManager.getSampleRate();
    int startS  = static_cast<int>(m_selection.getSelectionStart() * sr);
    int endS    = static_cast<int>(m_selection.getSelectionEnd() * sr);
    int numSamp = endS - startS;
    int numCh   = buffer->getNumChannels();
    if (numSamp <= 0) return {};

    juce::File outFile = destDir.getChildFile(generateFileName());

    juce::WavAudioFormat wav;
    auto* writer = wav.createWriterFor(new juce::FileOutputStream(outFile),
                                        44100.0, numCh, 16, {}, 0);
    if (!writer) return {};

    juce::AudioBuffer<float> sel(numCh, numSamp);
    for (int ch = 0; ch < numCh; ++ch)
        sel.copyFrom(ch, 0, *buffer, ch, startS, numSamp);

    writer->writeFromAudioSampleBuffer(sel, 0, numSamp);
    delete writer;
    return outFile;
}

juce::String DragExport::generateFileName() const
{
    auto base = m_fileManager.getFileName().upToLastOccurrenceOf(".", false, false);
    if (base.isEmpty()) base = "export";

    auto fmt = [](double sec) -> juce::String {
        int m = static_cast<int>(sec) / 60;
        double r = sec - m * 60;
        return juce::String::formatted("%02d-%06.3f", m, r);
    };

    return base + "_selection_"
        + fmt(m_selection.getSelectionStart()) + "-"
        + fmt(m_selection.getSelectionEnd()) + ".wav";
}
