#include "MainComponent.h"
#include "SelectionManager.h"
#include "AudioFileManager.h"
#include "WaveformThumbnail.h"
#include "SelectionOverlay.h"
#include "TransportBar.h"
#include "DragExport.h"

#include <juce_audio_formats/juce_audio_formats.h>

MainComponent::MainComponent()
{
    m_selection        = std::make_unique<SelectionManager>();
    m_fileManager      = std::make_unique<AudioFileManager>();
    m_waveform         = std::make_unique<WaveformThumbnail>(*m_fileManager, *m_selection);
    m_selectionOverlay = std::make_unique<SelectionOverlay>(*m_selection, *m_waveform);
    m_transport        = std::make_unique<TransportBar>(*m_fileManager, *m_selection);
    m_dragExport       = std::make_unique<DragExport>(*m_selection, *m_fileManager);
    m_commandManager   = std::make_unique<juce::ApplicationCommandManager>();

    m_deviceManager.initialiseWithDefaultDevices(2, 2);
    m_transport->setAudioDeviceManager(&m_deviceManager);

    addAndMakeVisible(*m_waveform);
    addAndMakeVisible(*m_selectionOverlay);
    addAndMakeVisible(*m_transport);

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
    }

    setInterceptsMouseClicks(true, true);

    // Recording finished callback
    m_transport->onPositionChanged = [this](double pos) {
        if (pos >= 0.0) return; // normal position update, ignore
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
    // Draw selection info in transport bar area
    auto r = getLocalBounds().removeFromBottom(36).removeFromLeft(250);
    m_selectionOverlay->drawSelectionInfo(g, r);
}

void MainComponent::resized()
{
    auto r = getLocalBounds();
    auto menuR = r.removeFromTop(24);
    m_menuBar->setBounds(menuR);

    auto transportR = r.removeFromBottom(36);
    m_transport->setBounds(transportR);

    m_waveform->setBounds(r);
    m_selectionOverlay->setBounds(r);
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
        menu.addCommandItem(m_commandManager.get(), cmdOpen);
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
        menu.addCommandItem(m_commandManager.get(), cmdPlaySel);
    }
    else if (idx == 2)
    {
        menu.addCommandItem(m_commandManager.get(), cmdZoomIn);
        menu.addCommandItem(m_commandManager.get(), cmdZoomOut);
    }
    return menu;
}

void MainComponent::menuItemSelected(int, int) {}

// ── Commands ──

juce::ApplicationCommandTarget* MainComponent::getNextCommandTarget() { return nullptr; }

void MainComponent::getAllCommands(juce::Array<juce::CommandID>& cmds)
{
    cmds.addArray({ cmdOpen, cmdPlayPause, cmdStop, cmdPlaySel, cmdZoomIn, cmdZoomOut });
}

void MainComponent::getCommandInfo(juce::CommandID id, juce::ApplicationCommandInfo& info)
{
    switch (id)
    {
    case cmdOpen:
        info.setInfo("Open...", "Open an audio file", "File", 0);
        info.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdPlayPause:
        info.setInfo("Play / Pause", "Toggle playback", "Transport", 0);
        info.addDefaultKeypress(juce::KeyPress::spaceKey, 0);
        break;
    case cmdStop:
        info.setInfo("Stop", "Stop playback", "Transport", 0);
        break;
    case cmdPlaySel:
        info.setInfo("Play Selection", "Play selection only", "Transport", 0);
        break;
    case cmdZoomIn:
        info.setInfo("Zoom In", "Zoom in", "View", 0);
        break;
    case cmdZoomOut:
        info.setInfo("Zoom Out", "Zoom out", "View", 0);
        break;
    default: break;
    }
}

bool MainComponent::perform(const juce::ApplicationCommandTarget::InvocationInfo& info)
{
    switch (info.commandID)
    {
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
    case cmdPlayPause:
        m_transport->isPlaying() ? m_transport->pause() : m_transport->play();
        return true;
    case cmdStop:
        m_transport->stop();
        return true;
    case cmdPlaySel:
        m_transport->playSelection();
        return true;
    case cmdZoomIn:
        m_waveform->setZoom(m_waveform->getZoom() * 1.5);
        m_waveform->repaint();
        return true;
    case cmdZoomOut:
        m_waveform->setZoom(m_waveform->getZoom() / 1.5);
        m_waveform->repaint();
        return true;
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

void MainComponent::loadAudioFile(const juce::File& file)
{
    if (!file.existsAsFile()) return;

    m_transport->stop();
    m_selection->clearSelection();

    if (m_fileManager->loadFile(file))
    {
        m_selection->setTotalDuration(m_fileManager->getDurationSec());
        if (auto* thumb = m_fileManager->getThumbnail())
            thumb->addChangeListener(this);

        m_transport->setPosition(0.0);

        if (auto* peer = getPeer())
            peer->setTitle("Open Edison - " + file.getFileName());

        resized();
        repaint();
    }
}
