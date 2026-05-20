#include "MainComponent.h"
#include "SelectionManager.h"
#include "AudioFileManager.h"
#include "GridManager.h"
#include "GridOverlay.h"
#include "WaveformThumbnail.h"
#include "SelectionOverlay.h"
#include "TransportBar.h"
#include "DragExport.h"
#include "SpectrogramComponent.h"
#include "UndoableActions.h"
#include "LoopOverlay.h"

#include <juce_audio_formats/juce_audio_formats.h>
#include <memory>
#include <vector>

MainComponent::MainComponent()
{
    m_selection        = std::make_unique<SelectionManager>();
    m_fileManager      = std::make_unique<AudioFileManager>();
    m_waveform         = std::make_unique<WaveformThumbnail>(*m_fileManager, *m_selection);
    m_gridManager      = std::make_unique<GridManager>();
    m_gridOverlay      = std::make_unique<GridOverlay>(*m_gridManager, *m_fileManager, *m_waveform);
    m_selectionOverlay = std::make_unique<SelectionOverlay>(*m_selection, *m_waveform, *m_fileManager, *m_gridManager);
    m_selectionOverlay->onExportDragStarted = [this](auto* src) {
        const auto now = juce::Time::getCurrentTime();
        m_dragExport->startDragIfOverSelection(
            juce::MouseEvent(juce::Desktop::getInstance().getMainMouseSource(),
                             juce::Point<float>{},
                             juce::ModifierKeys{},
                             0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                             src, src,
                             now,
                             juce::Point<float>{},
                             now,
                             1,
                             true),
            src);
    };
    m_transport        = std::make_unique<TransportBar>(*m_fileManager, *m_selection);
    m_dragExport       = std::make_unique<DragExport>(*m_selection, *m_fileManager);
    m_spectrogram      = std::make_unique<SpectrogramComponent>(*m_fileManager);

    m_markerManager = std::make_unique<MarkerManager>();
    m_markerOverlay = std::make_unique<MarkerOverlay>(*m_markerManager, *m_fileManager, *m_waveform);
    m_regionManager = std::make_unique<RegionManager>();
    m_regionOverlay = std::make_unique<RegionOverlay>(*m_regionManager, *m_fileManager, *m_selection, *m_waveform);
    m_loopManager = std::make_unique<LoopManager>();
    m_loopOverlay = std::make_unique<LoopOverlay>(*m_loopManager, *m_fileManager, *m_waveform);

    // ── View sync: keep waveform and spectrogram zoom/offset in sync ──
    // When user zooms or scrolls either view, the other view matches,
    // ensuring marker/loop overlays (which use waveform coordinates) stay aligned.
    m_waveform->onUserViewChanged = [this](double zoom, double offsetSec) {
        m_spectrogram->setZoom(zoom);
        m_spectrogram->setViewOffset(offsetSec);
        m_spectrogram->refreshViewport();
        m_zoomedToSelection = false;
    };
    m_spectrogram->onUserViewChanged = [this](double zoom, double offsetSec) {
        m_waveform->setZoom(zoom);
        double totalDur = m_fileManager->getDurationSec();
        if (totalDur > 0.0)
            m_waveform->setHorizontalOffset(offsetSec / totalDur);
        m_waveform->repaint();
        m_zoomedToSelection = false;
    };

    m_regionOverlay->onRegionSelected = [this](const RegionManager::Region& region) {
        m_selection->setSelection(region.startTime, region.endTime);

        const double totalDuration = m_fileManager->getDurationSec();
        const double regionDuration = region.endTime - region.startTime;
        if (totalDuration > 0.0 && regionDuration > 0.0)
        {
            const double visibleDuration = juce::jmin(totalDuration, regionDuration * 2.0);
            const double offsetTime = juce::jmax(0.0, region.startTime - regionDuration * 0.5);

            m_waveform->setZoom(totalDuration / visibleDuration);
            m_waveform->setHorizontalOffset(offsetTime / totalDuration);
        }

        m_waveform->repaint();
        m_selectionOverlay->repaint();
        m_markerOverlay->repaint();
        m_loopOverlay->repaint();
    };

    m_commandManager   = std::make_unique<juce::ApplicationCommandManager>();

    m_deviceManager.initialiseWithDefaultDevices(2, 2);
    m_transport->setAudioDeviceManager(&m_deviceManager);
    m_transport->setLoopManager(m_loopManager.get());
    m_transport->setGridManager(m_gridManager.get());

    addAndMakeVisible(*m_waveform);
    addAndMakeVisible(*m_spectrogram);
    addAndMakeVisible(*m_gridOverlay);
    addAndMakeVisible(*m_selectionOverlay);
    addAndMakeVisible(*m_regionOverlay);
    addAndMakeVisible(*m_markerOverlay);
    addAndMakeVisible(*m_loopOverlay);
    addAndMakeVisible(*m_transport);
    m_spectrogram->setVisible(false); // start with waveform view
    m_gridOverlay->setVisible(false);
    m_markerOverlay->setAlwaysOnTop(true);
    m_markerOverlay->setInterceptsMouseClicks(true, true);
    m_markerOverlay->onMarkerClicked = [this](double time) {
        m_transport->setPosition(time);
        m_spectrogram->setPlaybackPosition(time);
    };

    m_menuBar = std::make_unique<juce::MenuBarComponent>(this);
    addAndMakeVisible(*m_menuBar);

    setApplicationCommandManagerToWatch(m_commandManager.get());
    m_commandManager->registerAllCommandsForTarget(this);
    addKeyListener(m_commandManager->getKeyMappings());
    if (auto* mappings = m_commandManager->getKeyMappings())
    {
        mappings->resetToDefaultMappings();
        mappings->addKeyPress(cmdOpen, juce::KeyPress('o', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(cmdPlayPause, juce::KeyPress(juce::KeyPress::spaceKey, 0, 0));
        mappings->addKeyPress(cmdToggleView, juce::KeyPress('s', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(cmdSilence, juce::KeyPress(juce::KeyPress::deleteKey, 0, 0));
        mappings->addKeyPress(cmdSilence, juce::KeyPress(juce::KeyPress::backspaceKey, 0, 0));
        mappings->addKeyPress(cmdStop, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));
        mappings->addKeyPress(cmdCut, juce::KeyPress('x', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(cmdCopy, juce::KeyPress('c', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(cmdPaste, juce::KeyPress('v', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(cmdAddMarker, juce::KeyPress('m', 0, 0));
        mappings->addKeyPress(cmdRemoveMarker, juce::KeyPress('m', juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(cmdNextMarker, juce::KeyPress(']', 0, 0));
        mappings->addKeyPress(cmdPrevMarker, juce::KeyPress('[', 0, 0));
        mappings->addKeyPress(cmdAddRegion, juce::KeyPress('r', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(cmdRemoveRegion, juce::KeyPress(juce::KeyPress::backspaceKey, juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(cmdNextRegion, juce::KeyPress(juce::KeyPress::tabKey, juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(cmdPrevRegion, juce::KeyPress(juce::KeyPress::tabKey, juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(cmdZoomToSelection, juce::KeyPress('z', 0, 0));
        mappings->addKeyPress(cmdFitAll, juce::KeyPress('f', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(cmdToggleLoop, juce::KeyPress('l', 0, 0));
        mappings->addKeyPress(cmdToggleSnap, juce::KeyPress('x', 0, 0));
        mappings->addKeyPress(cmdToggleGridSnap, juce::KeyPress('g', 0, 0));
        mappings->addKeyPress(cmdFadeOut, juce::KeyPress('o', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier, 0));
        mappings->addKeyPress(cmdSelectAll, juce::KeyPress('a', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(cmdNew, juce::KeyPress('n', juce::ModifierKeys::ctrlModifier, 0));
        mappings->addKeyPress(cmdZoomIn, juce::KeyPress('=', 0, 0));
        mappings->addKeyPress(cmdZoomOut, juce::KeyPress('-', 0, 0));
    }
    setInterceptsMouseClicks(true, true);

    // Recording finished callback + spectrogram playhead + waveform auto-scroll
    m_transport->onPositionChanged = [this](double pos) {
        if (pos >= 0.0) {
            // Forward playback position to spectrogram for playhead cursor
            m_markerOverlay->setPlaybackPosition(pos);
            m_spectrogram->setPlaybackPosition(pos);

            // Waveform auto-scroll during playback (follow the playhead)
            if (m_viewMode == WaveformView && m_fileManager->hasAudio())
            {
                double totalDur = m_fileManager->getDurationSec();
                if (totalDur > 0.0)
                {
                    double viewDur = m_waveform->getVisibleDuration();
                    double offset  = m_waveform->getHorizontalOffset();
                    double rightEdge = offset * totalDur + viewDur;
                    double threshold = viewDur * 0.15; // 85% into viewport

                    if (pos > rightEdge - threshold)
                    {
                        double newOffsetTime = pos - viewDur * 0.7;
                        m_waveform->setHorizontalOffset(newOffsetTime / totalDur);
                        m_waveform->repaint();
                        m_selectionOverlay->repaint();
                        m_markerOverlay->repaint();
                        m_gridOverlay->repaint();
                        m_loopOverlay->repaint();
                    }
                }
            }
            return;
        }
        // pos < 0 means recording finished
        auto* recorded = m_transport->getRecordedBuffer();
        if (recorded && recorded->getNumSamples() > 0)
        {
            juce::WavAudioFormat wav;
            juce::File tmp = juce::File::createTempFile("wav");
            auto* out = new juce::FileOutputStream(tmp);
            if (auto* writer = wav.createWriterFor(out,
                    m_transport->getRecordedSampleRate(),
                    recorded->getNumChannels(), 16, {}, 0))
            {
                writer->writeFromAudioSampleBuffer(*recorded, 0, recorded->getNumSamples());
                writer->flush();
                delete writer;
                juce::MessageManager::callAsync([this, tmp] { loadAudioFile(tmp); });
            }
            else
            {
                delete out;
            }
        }
    };

    setSize(900, 400);
}

MainComponent::~MainComponent() = default;

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xFF1A1A2E));
}

void MainComponent::setViewMode(ViewMode mode)
{
    m_viewMode = mode;
    bool showWaveform   = (mode == WaveformView);
    bool showSpectrogram = (mode == SpectrogramView);

    m_waveform->setVisible(showWaveform);
    m_gridOverlay->setVisible(showWaveform && m_gridManager->isEnabled());
    m_selectionOverlay->setVisible(showWaveform);
    m_spectrogram->setVisible(showSpectrogram);

    if (showSpectrogram)
        m_spectrogram->rebuildFromAudio();

    repaint();
}

void MainComponent::resized()
{
    auto r = getLocalBounds();
    auto menuR = r.removeFromTop(24);
    m_menuBar->setBounds(menuR);

    auto transportR = r.removeFromBottom(36);
    m_transport->setBounds(transportR);

    auto regionBarR = r.removeFromTop(22);
    m_regionOverlay->setBounds(regionBarR);

    m_waveform->setBounds(r);
    m_gridOverlay->setBounds(r);
    m_selectionOverlay->setBounds(r);
    m_spectrogram->setBounds(r);
    m_markerOverlay->setBounds(r);
    m_loopOverlay->setBounds(r);
    // m_regionOverlay bounds set above (already in regionBarR)
}

// ── Menu ──

juce::StringArray MainComponent::getMenuBarNames()
{
    return { "File", "Edit", "View" };
}

juce::PopupMenu MainComponent::getMenuForIndex(int idx, const juce::String&)
{
    juce::PopupMenu menu;
    if (idx == 0)
    {
        menu.addCommandItem(m_commandManager.get(), cmdNew);
        menu.addCommandItem(m_commandManager.get(), cmdOpen);
        menu.addCommandItem(m_commandManager.get(), cmdSaveAs);
        menu.addSeparator();
        juce::PopupMenu inputMenu;
        auto* currentDevice = m_deviceManager.getCurrentAudioDevice();
        auto currentInputName = currentDevice ? currentDevice->getName() : juce::String{};
        auto& devices = m_deviceManager.getAvailableDeviceTypes();
        for (auto* type : devices)
        {
            for (auto& name : type->getDeviceNames(true))
            {
                auto* t = type;
                juce::String n = name;
                inputMenu.addItem(n, true, n == currentInputName, [this, t] {
                    m_deviceManager.setCurrentAudioDeviceType(t->getTypeName(), false);
                });
            }
        }
        menu.addSubMenu("Input Device", inputMenu);
    }
    else if (idx == 1)
    {
        menu.addCommandItem(m_commandManager.get(), cmdUndo);
        menu.addCommandItem(m_commandManager.get(), cmdRedo);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdSelectAll);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdCut);
        menu.addCommandItem(m_commandManager.get(), cmdCopy);
        menu.addCommandItem(m_commandManager.get(), cmdPaste);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdPlaySel);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdSilence);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdReverse);
        menu.addCommandItem(m_commandManager.get(), cmdNormalize);
        menu.addCommandItem(m_commandManager.get(), cmdFadeIn);
        menu.addCommandItem(m_commandManager.get(), cmdFadeOut);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdAddRegion);
        menu.addCommandItem(m_commandManager.get(), cmdRemoveRegion);
        menu.addCommandItem(m_commandManager.get(), cmdNextRegion);
        menu.addCommandItem(m_commandManager.get(), cmdPrevRegion);
    }
    else if (idx == 2)
    {
        menu.addCommandItem(m_commandManager.get(), cmdToggleView);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdZoomIn);
        menu.addCommandItem(m_commandManager.get(), cmdZoomOut);
        menu.addCommandItem(m_commandManager.get(), cmdZoomToSelection);
        menu.addCommandItem(m_commandManager.get(), cmdFitAll);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdToggleSnap);
        menu.addCommandItem(m_commandManager.get(), cmdToggleGridSnap);
        menu.addCommandItem(m_commandManager.get(), cmdToggleLoop);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdSetBPM);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdAddMarker);
        menu.addCommandItem(m_commandManager.get(), cmdRemoveMarker);
        menu.addCommandItem(m_commandManager.get(), cmdNextMarker);
        menu.addCommandItem(m_commandManager.get(), cmdPrevMarker);
    }
    return menu;
}

void MainComponent::menuItemSelected(int, int) {}

// ── Commands ──

juce::ApplicationCommandTarget* MainComponent::getNextCommandTarget() { return nullptr; }

void MainComponent::getAllCommands(juce::Array<juce::CommandID>& cmds)
{
    cmds.addArray({ cmdNew, cmdOpen, cmdSaveAs, cmdPlayPause, cmdStop, cmdPlaySel, cmdZoomIn,
                     cmdZoomOut, cmdToggleView, cmdUndo, cmdRedo, cmdSilence,
                     cmdReverse, cmdNormalize, cmdFadeIn, cmdFadeOut,
                     cmdAddMarker, cmdRemoveMarker, cmdNextMarker, cmdPrevMarker,
                     cmdAddRegion, cmdRemoveRegion, cmdNextRegion, cmdPrevRegion,
                     cmdZoomToSelection, cmdFitAll, cmdToggleLoop, cmdToggleSnap,
                     cmdCut, cmdCopy, cmdPaste, cmdSelectAll,
                     cmdToggleGridSnap, cmdSetBPM });
}

void MainComponent::getCommandInfo(juce::CommandID id, juce::ApplicationCommandInfo& info)
{
    switch (id)
    {
    case cmdNew:
        info.setInfo("New Project", "Clear the current project and start fresh", "File", 0);
        info.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdOpen:
        info.setInfo("Open...", "Open an audio file", "File", 0);
        info.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdSaveAs:
        info.setInfo("Save As...", "Save the edited audio to a WAV file", "File", 0);
        info.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
        break;
    case cmdPlayPause:
        info.setInfo("Play / Pause", "Toggle playback", "Transport", 0);
        info.addDefaultKeypress(juce::KeyPress::spaceKey, 0);
        break;
    case cmdStop:
        info.setInfo("Stop (Esc)", "Stop playback and reset position", "Transport", 0);
        info.addDefaultKeypress(juce::KeyPress::escapeKey, 0);
        break;
    case cmdPlaySel:
        info.setInfo("Play Selection", "Play selection only", "Transport", 0);
        break;
    case cmdZoomIn:
        info.setInfo("Zoom In (+)", "Zoom in", "View", 0);
        info.addDefaultKeypress('=', 0);
        break;
    case cmdZoomOut:
        info.setInfo("Zoom Out (-)", "Zoom out", "View", 0);
        info.addDefaultKeypress('-', 0);
        break;
    case cmdToggleView:
        info.setInfo("Toggle View (Ctrl+S)", "Switch between waveform and spectrogram", "View", 0);
        info.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdUndo:
        info.setInfo("Undo", "Undo last action", "Edit", 0);
        info.addDefaultKeypress('z', juce::ModifierKeys::ctrlModifier);
        info.setActive(m_undoManager.canUndo());
        break;
    case cmdRedo:
        info.setInfo("Redo", "Redo last undone action", "Edit", 0);
        info.addDefaultKeypress('z', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
        info.setActive(m_undoManager.canRedo());
        break;
    case cmdSilence:
        info.setInfo("Silence Selection (Delete/Backspace)", "Set selected audio to silence", "Process", 0);
        info.addDefaultKeypress('d', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdReverse:
        info.setInfo("Reverse Selection", "Reverse the selected audio", "Process", 0);
        info.addDefaultKeypress('r', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdNormalize:
        info.setInfo("Normalize Selection", "Normalize volume of the selected audio", "Process", 0);
        info.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
        break;
    case cmdFadeIn:
        info.setInfo("Fade In Selection", "Fade in the selected audio", "Process", 0);
        info.addDefaultKeypress('i', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdFadeOut:
        info.setInfo("Fade Out Selection", "Fade out the selected audio", "Process", 0);
        info.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
        break;
    case cmdAddMarker:
        info.setInfo("Add Marker (M)", "Add a marker at playhead position", "Markers", 0);
        info.addDefaultKeypress('m', 0);
        break;
    case cmdRemoveMarker:
        info.setInfo("Remove Marker (Shift+M)", "Remove marker at playhead position", "Markers", 0);
        info.addDefaultKeypress('m', juce::ModifierKeys::shiftModifier);
        break;
    case cmdNextMarker:
        info.setInfo("Next Marker (])", "Jump to next marker", "Markers", 0);
        info.addDefaultKeypress(']', 0);
        break;
    case cmdPrevMarker:
        info.setInfo("Previous Marker ([)", "Jump to previous marker", "Markers", 0);
        info.addDefaultKeypress('[', 0);
        break;
    case cmdZoomToSelection:
        info.setInfo("Zoom to Selection (Z)", "Zoom waveform to fit the current selection", "View", 0);
        info.addDefaultKeypress('z', 0);
        break;
    case cmdFitAll:
        info.setInfo("Fit All (Ctrl+Shift+F)", "Reset zoom to show the entire audio file", "View", 0);
        info.addDefaultKeypress('f', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
        break;
    case cmdAddRegion:
        info.setInfo("Add Region (Ctrl+Shift+R)", "Add region from current selection", "Regions", 0);
        info.addDefaultKeypress('r', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
        break;
    case cmdRemoveRegion:
        info.setInfo("Remove Current Region (Shift+Backspace)", "Remove the currently selected region", "Regions", 0);
        info.addDefaultKeypress(juce::KeyPress::backspaceKey, juce::ModifierKeys::shiftModifier);
        break;
    case cmdNextRegion:
        info.setInfo("Next Region (Ctrl+Tab)", "Switch to next region", "Regions", 0);
        info.addDefaultKeypress(juce::KeyPress::tabKey, juce::ModifierKeys::ctrlModifier);
        break;
    case cmdPrevRegion:
        info.setInfo("Previous Region (Ctrl+Shift+Tab)", "Switch to previous region", "Regions", 0);
        info.addDefaultKeypress(juce::KeyPress::tabKey, juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
        break;
    case cmdToggleLoop:
        info.setInfo("Toggle Loop (L)", "Toggle loop/cycle playback mode", "Transport", 0);
        info.addDefaultKeypress('l', 0);
        break;
    case cmdSetBPM:
        info.setInfo("Set BPM...", "Set the BPM and grid division", "View", 0);
        break;
    case cmdToggleSnap:
        info.setInfo("Toggle Snap to Zero (X)", "Toggle zero crossing snapping for selection edges", "Snap", 0);
        info.addDefaultKeypress('x', 0);
        info.setTicked(m_selection != nullptr && m_selection->isSnapToZero());
        break;
    case cmdToggleGridSnap:
        info.setInfo("Toggle Snap to Grid (G)", "Toggle beat/grid snapping for selection edges", "Snap", 0);
        info.addDefaultKeypress('g', 0);
        info.setTicked(m_gridManager != nullptr && m_gridManager->isEnabled());
        break;
    case cmdCut:
        info.setInfo("Cut (Ctrl+X)", "Copy selection to clipboard and silence it", "Edit", 0);
        info.addDefaultKeypress('x', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdSelectAll:
        info.setInfo("Select All (Ctrl+A)", "Select the entire audio file", "Edit", 0);
        info.addDefaultKeypress('a', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdCopy:
        info.setInfo("Copy (Ctrl+C)", "Copy selection to clipboard", "Edit", 0);
        info.addDefaultKeypress('c', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdPaste:
        info.setInfo("Paste (Ctrl+V)", "Paste clipboard content at playhead", "Edit", 0);
        info.addDefaultKeypress('v', juce::ModifierKeys::ctrlModifier);
        break;
    default: break;
    }
}

bool MainComponent::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    switch (info.commandID)
    {
    case cmdNew:
        newProject();
        return true;
    case cmdOpen: {
        auto chooser = std::make_shared<juce::FileChooser>(
            "Select audio file", juce::File{},
            m_fileManager->getFormatManager().getWildcardForAllFormats());
        chooser->launchAsync(juce::FileBrowserComponent::openMode,
            [this, chooser](const juce::FileChooser& fc) {
                if (fc.getResult().existsAsFile())
                    loadAudioFile(fc.getResult());
            });
        return true;
    }
    case cmdSaveAs:
        saveAudioAs();
        return true;
    case cmdPlayPause:
        m_transport->isPlaying() ? m_transport->pause() : m_transport->play();
        return true;
    case cmdStop:
        m_transport->stop();
        m_spectrogram->setPlaybackPosition(-1.0);
        return true;
    case cmdPlaySel:
        m_transport->playSelection();
        return true;
    case cmdZoomIn: {
        const double newZoom = m_waveform->getZoom() * 1.5;
        m_waveform->setZoom(newZoom);
        m_waveform->repaint();
        // Sync spectrogram
        m_spectrogram->setZoom(newZoom);
        m_spectrogram->refreshViewport();
        return true;
    }
    case cmdZoomOut: {
        const double newZoom = m_waveform->getZoom() / 1.5;
        m_waveform->setZoom(newZoom);
        m_waveform->repaint();
        // Sync spectrogram
        m_spectrogram->setZoom(newZoom);
        m_spectrogram->refreshViewport();
        return true;
    }
    case cmdUndo:
        m_undoManager.undo();
        m_waveform->repaint();
        m_spectrogram->rebuildFromAudio();
        m_zoomedToSelection = false;
        m_commandManager->commandStatusChanged();
        return true;
    case cmdRedo:
        m_undoManager.redo();
        m_waveform->repaint();
        m_spectrogram->rebuildFromAudio();
        m_zoomedToSelection = false;
        m_commandManager->commandStatusChanged();
        return true;
    case cmdSilence:
        silenceSelection();
        return true;
    case cmdReverse:
        reverseSelection();
        return true;
    case cmdNormalize:
        normalizeSelection();
        return true;
    case cmdFadeIn:
        fadeSelectionIn();
        return true;
    case cmdFadeOut:
        fadeSelectionOut();
        return true;
    case cmdCut:
        cutSelection();
        return true;
    case cmdSelectAll:
        if (m_fileManager->hasAudio()) {
            m_selection->setSelection(0.0, m_fileManager->getDurationSec());
            m_waveform->repaint();
            m_selectionOverlay->repaint();
        }
        return true;
    case cmdCopy:
        copySelection();
        return true;
    case cmdPaste:
        pasteClipboard();
        return true;
    case cmdAddMarker: {
        if (!m_fileManager->hasAudio()) return true;
        double pos = m_transport->getPosition();
        if (pos < 0.0) pos = 0.0;
        m_markerManager->addMarker(pos);
        m_markerOverlay->repaint();
        return true;
    }
    case cmdRemoveMarker: {
        if (!m_fileManager->hasAudio()) return true;
        double pos = m_transport->getPosition();
        if (pos < 0.0) pos = 0.0;
        m_markerManager->removeMarkerAt(pos);
        m_markerOverlay->repaint();
        return true;
    }
    case cmdNextMarker: {
        if (!m_fileManager->hasAudio()) return true;
        double pos = m_transport->getPosition();
        double next = m_markerManager->getNextMarker(pos);
        if (next >= 0.0) {
            m_transport->setPosition(next);
            m_spectrogram->setPlaybackPosition(next);
            m_markerOverlay->repaint();
        }
        return true;
    }
    case cmdPrevMarker: {
        if (!m_fileManager->hasAudio()) return true;
        double pos = m_transport->getPosition();
        double prev = m_markerManager->getPrevMarker(pos);
        if (prev >= 0.0) {
            m_transport->setPosition(prev);
            m_spectrogram->setPlaybackPosition(prev);
            m_markerOverlay->repaint();
        }
        return true;
    }
    case cmdToggleView:
        setViewMode(m_viewMode == WaveformView ? SpectrogramView : WaveformView);
        return true;
    case cmdZoomToSelection:
        zoomToSelection();
        return true;
    case cmdFitAll:
        fitAll();
        return true;
    case cmdToggleLoop:
        toggleLoop();
        return true;
    case cmdToggleSnap:
        toggleSnapToZero();
        return true;
    case cmdToggleGridSnap:
        toggleGridSnap();
        return true;
    case cmdSetBPM:
        showBPMDialog();
        return true;
    case cmdAddRegion:
        addRegionFromSelection();
        return true;
    case cmdRemoveRegion: {
        int curId = m_regionManager->getCurrentRegionId();
        if (curId >= 0) {
            m_regionManager->removeRegion(curId);
            m_regionOverlay->repaint();
        }
        return true;
    }
    case cmdNextRegion: {
        if (m_regionManager->getNumRegions() == 0) return true;
        int curId = m_regionManager->getCurrentRegionId();
        int nextId = m_regionManager->getNextRegion(curId);
        if (nextId < 0 && m_regionManager->getNumRegions() > 0)
            nextId = m_regionManager->getRegions().front().id;
        if (nextId >= 0) {
            const auto* r = m_regionManager->getRegionById(nextId);
            if (r) {
                m_regionManager->setCurrentRegionId(nextId);
                m_selection->setSelection(r->startTime, r->endTime);
                m_waveform->repaint();
                m_selectionOverlay->repaint();
                m_regionOverlay->repaint();
            }
        }
        return true;
    }
    case cmdPrevRegion: {
        if (m_regionManager->getNumRegions() == 0) return true;
        int curId = m_regionManager->getCurrentRegionId();
        int prevId = m_regionManager->getPrevRegion(curId);
        if (prevId < 0 && m_regionManager->getNumRegions() > 0)
            prevId = m_regionManager->getRegions().back().id;
        if (prevId >= 0) {
            const auto* r = m_regionManager->getRegionById(prevId);
            if (r) {
                m_regionManager->setCurrentRegionId(prevId);
                m_selection->setSelection(r->startTime, r->endTime);
                m_waveform->repaint();
                m_selectionOverlay->repaint();
                m_regionOverlay->repaint();
            }
        }
        return true;
    }
    default:
        return false;
    }
}

// ── Drag-and-drop ──

bool MainComponent::isInterestedInFileDrag(const juce::StringArray& files)
{
    for (auto& f : files)
    {
        auto ext = juce::File(f).getFileExtension().toLowerCase();
        if (ext == ".wav" || ext == ".mp3" || ext == ".flac") return true;
    }
    return false;
}

void MainComponent::filesDropped(const juce::StringArray& files, int, int)
{
    for (auto& f : files)
    {
        auto file = juce::File(f);
        auto ext = file.getFileExtension().toLowerCase();
        if (ext == ".wav" || ext == ".mp3" || ext == ".flac")
        {
            loadAudioFile(file);
            break;
        }
    }
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster*)
{
    m_waveform->repaint();
}

void MainComponent::zoomToSelection()
{
    if (!m_fileManager->hasAudio() || !m_selection->hasSelection())
        return;

    // Toggle: if already zoomed to selection, restore previous view
    if (m_zoomedToSelection)
    {
        m_waveform->setZoom(m_preZoomLevel);
        m_waveform->setHorizontalOffset(m_preZoomOffset);
        m_spectrogram->setZoom(m_preZoomLevel);
        m_spectrogram->setViewOffset(m_preZoomOffset * m_fileManager->getDurationSec());
        m_zoomedToSelection = false;
        m_waveform->repaint();
        m_spectrogram->repaint();
        return;
    }

    // Store current view state before zooming
    m_preZoomLevel = m_waveform->getZoom();
    m_preZoomOffset = m_waveform->getHorizontalOffset();

    double totalDur = m_fileManager->getDurationSec();
    double selStart = m_selection->getSelectionStart();
    double selEnd   = m_selection->getSelectionEnd();
    double selDur   = selEnd - selStart;

    if (selDur <= 0.0) return;

    // Zoom to fit selection with 20% padding
    double padding = selDur * 0.2;
    double visibleDur = selDur + padding;
    double newZoom = totalDur / visibleDur;
    m_waveform->setZoom(newZoom);

    // Offset view so selection starts at ~6% from left edge
    double offsetTime = std::max(0.0, selStart - padding * 0.3);
    m_waveform->setHorizontalOffset(offsetTime / totalDur);

    // Sync spectrogram view to same zoom/offset
    m_spectrogram->setZoom(newZoom);
    m_spectrogram->setViewOffset(offsetTime);

    m_zoomedToSelection = true;
    m_waveform->repaint();
    m_spectrogram->repaint();
}

void MainComponent::fitAll()
{
    if (!m_fileManager->hasAudio())
        return;

    // Restore zoom to show entire file
    m_waveform->setZoom(1.0);
    m_waveform->setHorizontalOffset(0.0);
    m_spectrogram->setZoom(1.0);
    m_spectrogram->setViewOffset(0.0);
    m_zoomedToSelection = false;
    m_waveform->repaint();
    m_spectrogram->repaint();
}

void MainComponent::toggleLoop()
{
    if (m_loopManager->isLoopEnabled())
    {
        // Turn off loop
        m_loopManager->clearLoop();
    }
    else
    {
        // Turn on loop
        if (m_selection->hasSelection())
        {
            m_loopManager->setLoopRange(
                m_selection->getSelectionStart(),
                m_selection->getSelectionEnd());
            m_loopManager->setLoopEnabled(true);
        }
        else if (m_fileManager->hasAudio())
        {
            // No selection - loop entire file
            m_loopManager->setLoopRange(0.0, m_fileManager->getDurationSec());
            m_loopManager->setLoopEnabled(true);
        }
    }

    m_loopOverlay->repaint();
    m_waveform->repaint();
    m_spectrogram->repaint();
}

void MainComponent::addRegionFromSelection()
{
    if (!m_fileManager->hasAudio() || !m_selection->hasSelection())
        return;

    double startSec = m_selection->getSelectionStart();
    double endSec   = m_selection->getSelectionEnd();

    int id = m_regionManager->addRegion({}, startSec, endSec);
    if (id >= 0)
    {
        m_regionManager->setCurrentRegionId(id);
        m_regionOverlay->repaint();
        m_waveform->repaint();
        m_selectionOverlay->repaint();
    }
}

void MainComponent::silenceSelection()
{
    if (!m_fileManager->hasAudio() || !m_selection->hasSelection())
        return;

    auto* buf = m_fileManager->getBuffer();
    if (buf == nullptr)
        return;

    double sampleRate = m_fileManager->getSampleRate();
    auto startTime  = m_selection->getSelectionStart();
    auto endTime    = m_selection->getSelectionEnd();
    int startSample = static_cast<int>(std::round(startTime * sampleRate));
    int endSample   = static_cast<int>(std::round(endTime * sampleRate));
    int numSamples  = endSample - startSample;

    if (numSamples <= 0)
        return;

    // Clamp to buffer bounds
    startSample = juce::jlimit(0, buf->getNumSamples() - 1, startSample);
    numSamples  = juce::jmin(numSamples, buf->getNumSamples() - startSample);

    auto action = new AudioModifyAction(*m_fileManager, startSample, numSamples,
        [sampleRate](juce::AudioBuffer<float>& audio, int start, int num)
        {
            const int maxCrossfadeSamples = static_cast<int>(0.005 * sampleRate);
            const int crossfadeSamples = juce::jmin(maxCrossfadeSamples, num / 3);
            const int middleStart = start + crossfadeSamples;
            const int middleLength = juce::jmax(0, num - (crossfadeSamples * 2));

            for (int ch = 0; ch < audio.getNumChannels(); ++ch)
            {
                auto* readSamples = audio.getReadPointer(ch);
                auto* writeSamples = audio.getWritePointer(ch);

                std::vector<float> leftEdge(static_cast<size_t>(crossfadeSamples));
                std::vector<float> rightEdge(static_cast<size_t>(crossfadeSamples));

                for (int i = 0; i < crossfadeSamples; ++i)
                {
                    leftEdge[static_cast<size_t>(i)] = readSamples[start + i];
                    rightEdge[static_cast<size_t>(i)] = readSamples[start + num - crossfadeSamples + i];
                }

                if (crossfadeSamples > 1)
                {
                    const float fadeDenominator = static_cast<float>(crossfadeSamples - 1);

                    for (int i = 0; i < crossfadeSamples; ++i)
                    {
                        const float leftGain = 1.0f - (static_cast<float>(i) / fadeDenominator);
                        writeSamples[start + i] = leftEdge[static_cast<size_t>(i)] * leftGain;

                        const float rightGain = static_cast<float>(i) / fadeDenominator;
                        writeSamples[start + num - crossfadeSamples + i]
                            = rightEdge[static_cast<size_t>(i)] * rightGain;
                    }
                }
                else if (crossfadeSamples == 1)
                {
                    writeSamples[start] = 0.0f;
                    writeSamples[start + num - 1] = 0.0f;
                }

                if (middleLength > 0)
                    juce::FloatVectorOperations::clear(writeSamples + middleStart, middleLength);
            }
        });

    action->onAudioChanged = [this]
    {
        m_waveform->repaint();
        if (m_viewMode == SpectrogramView)
            m_spectrogram->rebuildFromAudio();
    };

    m_undoManager.perform(action);
    m_commandManager->commandStatusChanged();
}

void MainComponent::reverseSelection()
{
    if (!m_fileManager->hasAudio() || !m_selection->hasSelection())
        return;

    auto* buf = m_fileManager->getBuffer();
    if (buf == nullptr)
        return;

    double sr       = m_fileManager->getSampleRate();
    auto startTime  = m_selection->getSelectionStart();
    auto endTime    = m_selection->getSelectionEnd();
    int startSample = static_cast<int>(std::round(startTime * sr));
    int endSample   = static_cast<int>(std::round(endTime * sr));
    int numSamples  = endSample - startSample;

    if (numSamples <= 0)
        return;

    startSample = juce::jlimit(0, buf->getNumSamples() - 1, startSample);
    numSamples  = juce::jmin(numSamples, buf->getNumSamples() - startSample);

    auto action = new AudioModifyAction(*m_fileManager, startSample, numSamples,
        [](juce::AudioBuffer<float>& audio, int start, int num)
        {
            for (int ch = 0; ch < audio.getNumChannels(); ++ch)
            {
                auto* samples = audio.getWritePointer(ch);
                for (int i = 0; i < num / 2; ++i)
                    std::swap(samples[start + i], samples[start + num - 1 - i]);
            }
        });

    action->onAudioChanged = [this]
    {
        m_waveform->repaint();
        if (m_viewMode == SpectrogramView)
            m_spectrogram->rebuildFromAudio();
    };

    m_undoManager.perform(action);
    m_commandManager->commandStatusChanged();
}

void MainComponent::normalizeSelection()
{
    if (!m_fileManager->hasAudio() || !m_selection->hasSelection())
        return;

    auto* buf = m_fileManager->getBuffer();
    if (buf == nullptr)
        return;

    double sr       = m_fileManager->getSampleRate();
    auto startTime  = m_selection->getSelectionStart();
    auto endTime    = m_selection->getSelectionEnd();
    int startSample = static_cast<int>(std::round(startTime * sr));
    int endSample   = static_cast<int>(std::round(endTime * sr));
    int numSamples  = endSample - startSample;

    if (numSamples <= 0)
        return;

    startSample = juce::jlimit(0, buf->getNumSamples() - 1, startSample);
    numSamples  = juce::jmin(numSamples, buf->getNumSamples() - startSample);

    auto action = new AudioModifyAction(*m_fileManager, startSample, numSamples,
        [](juce::AudioBuffer<float>& audio, int start, int num)
        {
            float peak = 0.0f;
            for (int ch = 0; ch < audio.getNumChannels(); ++ch)
            {
                auto* samples = audio.getReadPointer(ch);
                for (int i = 0; i < num; ++i)
                    peak = std::max(peak, std::abs(samples[start + i]));
            }
            if (peak > 0.0f)
            {
                float gain = 1.0f / peak;
                for (int ch = 0; ch < audio.getNumChannels(); ++ch)
                {
                    auto* samples = audio.getWritePointer(ch);
                    for (int i = 0; i < num; ++i)
                        samples[start + i] *= gain;
                }
            }
        });

    action->onAudioChanged = [this]
    {
        m_waveform->repaint();
        if (m_viewMode == SpectrogramView)
            m_spectrogram->rebuildFromAudio();
    };

    m_undoManager.perform(action);
    m_commandManager->commandStatusChanged();
}

void MainComponent::fadeSelectionIn()
{
    if (!m_fileManager->hasAudio() || !m_selection->hasSelection())
        return;

    auto* buf = m_fileManager->getBuffer();
    if (buf == nullptr)
        return;

    double sr       = m_fileManager->getSampleRate();
    auto startTime  = m_selection->getSelectionStart();
    auto endTime    = m_selection->getSelectionEnd();
    int startSample = static_cast<int>(std::round(startTime * sr));
    int endSample   = static_cast<int>(std::round(endTime * sr));
    int numSamples  = endSample - startSample;

    if (numSamples <= 0)
        return;

    startSample = juce::jlimit(0, buf->getNumSamples() - 1, startSample);
    numSamples  = juce::jmin(numSamples, buf->getNumSamples() - startSample);

    auto action = new AudioModifyAction(*m_fileManager, startSample, numSamples,
        [](juce::AudioBuffer<float>& audio, int start, int num)
        {
            for (int ch = 0; ch < audio.getNumChannels(); ++ch)
            {
                auto* samples = audio.getWritePointer(ch);
                for (int i = 0; i < num; ++i)
                {
                    float gain = static_cast<float>(i) / num;
                    samples[start + i] *= gain;
                }
            }
        });

    action->onAudioChanged = [this]
    {
        m_waveform->repaint();
        if (m_viewMode == SpectrogramView)
            m_spectrogram->rebuildFromAudio();
    };

    m_undoManager.perform(action);
    m_commandManager->commandStatusChanged();
}

void MainComponent::fadeSelectionOut()
{
    if (!m_fileManager->hasAudio() || !m_selection->hasSelection())
        return;

    auto* buf = m_fileManager->getBuffer();
    if (buf == nullptr)
        return;

    double sr       = m_fileManager->getSampleRate();
    auto startTime  = m_selection->getSelectionStart();
    auto endTime    = m_selection->getSelectionEnd();
    int startSample = static_cast<int>(std::round(startTime * sr));
    int endSample   = static_cast<int>(std::round(endTime * sr));
    int numSamples  = endSample - startSample;

    if (numSamples <= 0)
        return;

    startSample = juce::jlimit(0, buf->getNumSamples() - 1, startSample);
    numSamples  = juce::jmin(numSamples, buf->getNumSamples() - startSample);

    auto action = new AudioModifyAction(*m_fileManager, startSample, numSamples,
        [](juce::AudioBuffer<float>& audio, int start, int num)
        {
            for (int ch = 0; ch < audio.getNumChannels(); ++ch)
            {
                auto* samples = audio.getWritePointer(ch);
                for (int i = 0; i < num; ++i)
                {
                    float gain = static_cast<float>(num - i) / num;
                    samples[start + i] *= gain;
                }
            }
        });

    action->onAudioChanged = [this]
    {
        m_waveform->repaint();
        if (m_viewMode == SpectrogramView)
            m_spectrogram->rebuildFromAudio();
    };

    m_undoManager.perform(action);
    m_commandManager->commandStatusChanged();
}

void MainComponent::newProject()
{
    m_transport->stop();
    m_fileManager->unload();
    m_selection->clearSelection();
    m_markerManager->clear();
    m_regionManager->clear();
    m_loopManager->clearLoop();
    m_undoManager.clearUndoHistory();
    m_commandManager->commandStatusChanged();
    m_zoomedToSelection = false;
    m_preZoomLevel = 1.0;
    m_preZoomOffset = 0.0;
    m_clipboardBuffer = nullptr;

    m_waveform->setZoom(1.0);
    m_waveform->setHorizontalOffset(0.0);
    m_waveform->setVerticalZoom(1.0);
    m_spectrogram->setZoom(1.0);
    m_spectrogram->setViewOffset(0.0);
    m_spectrogram->clear();

    if (auto* peer = getPeer())
        peer->setTitle("Open Edison");

    repaint();
}

void MainComponent::toggleSnapToZero()
{
    m_selection->toggleSnapToZero();
    repaint();
}

void MainComponent::toggleGridSnap()
{
    m_gridManager->toggleEnabled();
    m_selection->setSnapToGrid(m_gridManager->isEnabled());
    m_gridOverlay->setVisible(m_viewMode == WaveformView && m_gridManager->isEnabled());
    m_gridOverlay->repaint();
    repaint();
}

void MainComponent::showBPMDialog()
{
    double currentBPM = m_gridManager->getBPM();
    int currentDiv = m_gridManager->getDivision();

    // Capture values with shared_ptr to survive the dialog's anonymous lifetime
    auto resultBPM = std::make_shared<double>(currentBPM);
    auto resultDiv = std::make_shared<int>(currentDiv);

    auto* dialog = new juce::DialogWindow(
        "Set Grid BPM",
        juce::Colour(0xFF252540),
        true,  // close button
        true   // modal
    );

    auto* content = new juce::Component();
    content->setSize(300, 120);

    auto* bpmLabel = new juce::Label("bpmLabel", "BPM:");
    bpmLabel->setBounds(10, 10, 50, 22);
    content->addAndMakeVisible(bpmLabel);

    auto* bpmEditor = new juce::TextEditor("bpm");
    bpmEditor->setBounds(65, 10, 100, 22);
    bpmEditor->setText(juce::String(currentBPM, 1), false);
    bpmEditor->setInputRestrictions(6, "0123456789.");
    content->addAndMakeVisible(bpmEditor);

    auto* divLabel = new juce::Label("divLabel", "Division:");
    divLabel->setBounds(180, 10, 55, 22);
    content->addAndMakeVisible(divLabel);

    auto* divEditor = new juce::TextEditor("div");
    divEditor->setBounds(240, 10, 50, 22);
    divEditor->setText(juce::String(currentDiv), false);
    divEditor->setInputRestrictions(2, "123456789");
    content->addAndMakeVisible(divEditor);

    auto* okBtn = new juce::TextButton("OK");
    okBtn->setBounds(80, 50, 60, 28);
    content->addAndMakeVisible(okBtn);

    auto* cancelBtn = new juce::TextButton("Cancel");
    cancelBtn->setBounds(160, 50, 60, 28);
    content->addAndMakeVisible(cancelBtn);

    // Capture values before calling exitModalState (dialog deletes content after)
    okBtn->onClick = [dialog, resultBPM, resultDiv, bpmEditor, divEditor, this]() {
        *resultBPM = bpmEditor->getText().getDoubleValue();
        *resultDiv = divEditor->getText().getIntValue();
        // Apply immediately while dialog/editors are still alive
        if (*resultBPM >= 20.0 && *resultBPM <= 500.0 && *resultDiv >= 1 && *resultDiv <= 32)
        {
            m_gridManager->setBPM(*resultBPM);
            m_gridManager->setDivision(*resultDiv);
            m_gridOverlay->repaint();
            m_transport->repaint();
            repaint();
        }
        dialog->exitModalState(1);
    };
    cancelBtn->onClick = [dialog]() {
        dialog->exitModalState(0);
    };

    dialog->setContentOwned(content, true);
    dialog->centreAroundComponent(this, getWidth(), getHeight());
    dialog->setVisible(true);
    // enterModalState with deleteWhenDismissed=true: JUKE deletes the DialogWindow
    // (and its owned content) when exitModalState() is called
    dialog->enterModalState(true, nullptr, true);
}

void MainComponent::saveAudioAs()
{
    if (!m_fileManager->hasAudio())
        return;

    auto* buffer = m_fileManager->getBuffer();
    if (buffer == nullptr || buffer->getNumSamples() <= 0)
        return;

    auto chooser = std::make_shared<juce::FileChooser>(
        "Save Audio As...",
        m_fileManager->getFile().withFileExtension(".wav"),
        "*.wav");

    chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this, chooser](const juce::FileChooser& fc)
        {
            auto result = fc.getResult();
            if (result == juce::File{})
                return;

            auto* buffer = m_fileManager->getBuffer();
            if (buffer == nullptr)
                return;

            auto outFile = result.withFileExtension(".wav");

            juce::WavAudioFormat wavFormat;
            auto* writer = wavFormat.createWriterFor(
                new juce::FileOutputStream(outFile),
                m_fileManager->getSampleRate(),
                buffer->getNumChannels(),
                m_fileManager->getBitsPerSample(),
                {}, 0);

            if (writer == nullptr)
                return;

            writer->writeFromAudioSampleBuffer(*buffer, 0, buffer->getNumSamples());
            writer->flush();
            juce::Thread::sleep(50); // let the OS flush the file
            delete writer;

            // Update title to reflect saved file
            if (auto* peer = getPeer())
                peer->setTitle("Open Edison - " + outFile.getFileName());
        });
}

void MainComponent::loadAudioFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;

    m_transport->stop();
    m_selection->clearSelection();
    m_markerManager->clear();
    m_regionManager->clear();
    m_loopManager->clearLoop();
    m_undoManager.clearUndoHistory();
    m_commandManager->commandStatusChanged();
    m_zoomedToSelection = false;
    m_preZoomLevel = 1.0;
    m_preZoomOffset = 0.0;

    if (m_fileManager->loadFile(file))
    {
        m_selection->setTotalDuration(m_fileManager->getDurationSec());
        if (auto* thumb = m_fileManager->getThumbnail())
        {
            thumb->removeChangeListener(this);
            thumb->addChangeListener(this);
        }

        m_transport->setPosition(0.0);
        m_markerOverlay->setTotalDuration(m_fileManager->getDurationSec());

        if (m_viewMode == SpectrogramView)
            m_spectrogram->rebuildFromAudio();

        if (auto* peer = getPeer())
            peer->setTitle("Open Edison - " + file.getFileName());

        resized();
        repaint();
    }
}

void MainComponent::copySelection()
{
    if (!m_fileManager->hasAudio() || !m_selection->hasSelection())
        return;

    auto* buf = m_fileManager->getBuffer();
    if (buf == nullptr) return;

    double sr = m_fileManager->getSampleRate();
    int startS = static_cast<int>(m_selection->getSelectionStart() * sr);
    int endS   = static_cast<int>(m_selection->getSelectionEnd() * sr);
    int numS   = endS - startS;
    if (numS <= 0) return;

    startS = juce::jlimit(0, buf->getNumSamples() - 1, startS);
    numS   = juce::jmin(numS, buf->getNumSamples() - startS);

    m_clipboardBuffer = std::make_unique<juce::AudioBuffer<float>>(buf->getNumChannels(), numS);
    for (int ch = 0; ch < buf->getNumChannels(); ++ch)
        m_clipboardBuffer->copyFrom(ch, 0, *buf, ch, startS, numS);

    m_clipboardSampleRate = sr;
}

void MainComponent::cutSelection()
{
    copySelection();
    silenceSelection();
}

void MainComponent::pasteClipboard()
{
    if (!m_fileManager->hasAudio() || m_clipboardBuffer == nullptr)
        return;

    if (m_clipboardBuffer->getNumSamples() <= 0)
        return;

    auto* buf = m_fileManager->getBuffer();
    if (buf == nullptr) return;

    double sr = m_fileManager->getSampleRate();
    double pos = m_transport->getPosition();
    if (pos < 0.0) pos = 0.0;

    int writePos = static_cast<int>(pos * sr);
    int numS = m_clipboardBuffer->getNumSamples();
    writePos = juce::jlimit(0, buf->getNumSamples(), writePos);
    numS = juce::jmin(numS, buf->getNumSamples() - writePos);

    int numCh = juce::jmin(buf->getNumChannels(), m_clipboardBuffer->getNumChannels());

    // Save original samples for undo
    auto action = new AudioModifyAction(*m_fileManager, writePos, numS,
        [this, writePos, numS, numCh](juce::AudioBuffer<float>& audio, int /*start*/, int /*num*/)
        {
            for (int ch = 0; ch < numCh; ++ch)
                audio.copyFrom(ch, writePos, *m_clipboardBuffer, ch, 0, numS);
        });

    action->onAudioChanged = [this]
    {
        m_waveform->repaint();
        if (m_viewMode == SpectrogramView)
            m_spectrogram->rebuildFromAudio();
    };

    m_undoManager.perform(action);
    m_commandManager->commandStatusChanged();
}
