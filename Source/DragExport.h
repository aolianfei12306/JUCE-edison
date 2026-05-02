#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class SelectionManager;
class AudioFileManager;

class DragExport : public juce::DragAndDropContainer
{
public:
    DragExport(SelectionManager& selection, AudioFileManager& fileManager);
    ~DragExport() override = default;

    void startDragIfOverSelection(const juce::MouseEvent& e, juce::Component* sourceComponent);
    juce::File exportSelectionAsWav(const juce::File& destinationDir);

private:
    SelectionManager& m_selection;
    AudioFileManager& m_fileManager;
    juce::String generateFileName() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DragExport)
};
