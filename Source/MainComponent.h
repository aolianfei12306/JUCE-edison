#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <memory>

class SelectionManager;
class AudioFileManager;
class WaveformThumbnail;
class SelectionOverlay;
class TransportBar;
class DragExport;

class MainComponent : public juce::Component,
                      public juce::MenuBarModel,
                      public juce::ApplicationCommandTarget,
                      public juce::FileDragAndDropTarget,
                      public juce::ChangeListener
{
public:
    MainComponent();
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    // MenuBarModel
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int topLevelMenuIndex,
                                     const juce::String& menuName) override;
    void menuItemSelected(int menuItemID, int topLevelMenuIndex) override;

    // ApplicationCommandTarget
    ApplicationCommandTarget* getNextCommandTarget() override;
    void getAllCommands(juce::Array<juce::CommandID>& commands) override;
    void getCommandInfo(juce::CommandID commandID,
                         juce::ApplicationCommandInfo& result) override;
    bool perform(const juce::ApplicationCommandTarget::InvocationInfo& info) override;

    // FileDragAndDropTarget
    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;

    // ChangeListener
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    enum CommandIDs
    {
        cmdOpen      = 0x1001,
        cmdPlayPause = 0x1002,
        cmdStop      = 0x1003,
        cmdPlaySel   = 0x1004,
        cmdZoomIn    = 0x1005,
        cmdZoomOut   = 0x1006,
    };

    void loadAudioFile(const juce::File& file);

private:
    std::unique_ptr<SelectionManager>    m_selection;
    std::unique_ptr<AudioFileManager>    m_fileManager;
    std::unique_ptr<WaveformThumbnail>   m_waveform;
    std::unique_ptr<SelectionOverlay>    m_selectionOverlay;
    std::unique_ptr<TransportBar>        m_transport;
    std::unique_ptr<DragExport>          m_dragExport;
    std::unique_ptr<juce::MenuBarComponent> m_menuBar;
    std::unique_ptr<juce::ApplicationCommandManager> m_commandManager;
    juce::AudioDeviceManager              m_deviceManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
