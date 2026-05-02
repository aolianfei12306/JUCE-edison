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
class SpectrogramComponent;
class AudioModifyAction;
#include "MarkerManager.h"
#include "MarkerOverlay.h"
#include "RegionManager.h"
#include "RegionOverlay.h"

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

    enum ViewMode { WaveformView, SpectrogramView };

    void setViewMode(ViewMode mode);

    enum CommandIDs
    {
        cmdOpen      = 0x1001,
        cmdPlayPause = 0x1002,
        cmdStop      = 0x1003,
        cmdPlaySel   = 0x1004,
        cmdZoomIn    = 0x1005,
        cmdZoomOut   = 0x1006,
        cmdToggleView = 0x1007,
        cmdUndo      = 0x1008,
        cmdRedo      = 0x1009,
        cmdSilence    = 0x100A,
        cmdReverse    = 0x100B,
        cmdNormalize  = 0x100C,
        cmdFadeIn     = 0x100D,
        cmdFadeOut    = 0x100E,
        cmdAddMarker    = 0x1010,
        cmdRemoveMarker = 0x1011,
        cmdNextMarker   = 0x1012,
        cmdPrevMarker   = 0x1013,
        cmdAddRegion         = 0x1020,
        cmdRemoveRegion       = 0x1021,
        cmdNextRegion         = 0x1022,
        cmdPrevRegion         = 0x1023,
        cmdZoomToSelection    = 0x1030,
        cmdFitAll             = 0x1031,
    };

    void loadAudioFile(const juce::File& file);
    void addRegionFromSelection();
    void zoomToSelection();
    void fitAll();
    void silenceSelection();
    void reverseSelection();
    void normalizeSelection();
    void fadeSelectionIn();
    void fadeSelectionOut();

private:
    std::unique_ptr<SelectionManager>    m_selection;
    std::unique_ptr<AudioFileManager>    m_fileManager;
    std::unique_ptr<WaveformThumbnail>   m_waveform;
    std::unique_ptr<SelectionOverlay>    m_selectionOverlay;
    std::unique_ptr<TransportBar>        m_transport;
    std::unique_ptr<DragExport>          m_dragExport;
    std::unique_ptr<SpectrogramComponent> m_spectrogram;
    std::unique_ptr<juce::MenuBarComponent> m_menuBar;
    ViewMode m_viewMode = WaveformView;
    std::unique_ptr<juce::ApplicationCommandManager> m_commandManager;
    juce::UndoManager                     m_undoManager;
    std::unique_ptr<MarkerManager>        m_markerManager;
    std::unique_ptr<MarkerOverlay>        m_markerOverlay;
    std::unique_ptr<RegionManager>        m_regionManager;
    std::unique_ptr<RegionOverlay>        m_regionOverlay;
    int m_currentRegionCycler = -1;
    bool m_zoomedToSelection = false;
    double m_preZoomLevel = 1.0;
    double m_preZoomOffset = 0.0;
    juce::AudioDeviceManager              m_deviceManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
