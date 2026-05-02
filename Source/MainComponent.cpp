#include "MainComponent.h"
#include "SelectionManager.h"
#include "AudioFileManager.h"
#include "WaveformThumbnail.h"
#include "SelectionOverlay.h"
#include "TransportBar.h"
#include "DragExport.h"
#include "SpectrogramComponent.h"
#include "UndoableActions.h"

#include <juce_audio_formats/juce_audio_formats.h>

MainComponent::MainComponent()
{
    m_selection        = std::make_unique<SelectionManager>();
    m_fileManager      = std::make_unique<AudioFileManager>();
    m_waveform         = std::make_unique<WaveformThumbnail>(*m_fileManager, *m_selection);
    m_selectionOverlay = std::make_unique<SelectionOverlay>(*m_selection, *m_waveform);
    m_transport        = std::make_unique<TransportBar>(*m_fileManager, *m_selection);
    m_dragExport       = std::make_unique<DragExport>(*m_selection, *m_fileManager);
    m_spectrogram      = std::make_unique<SpectrogramComponent>(*m_fileManager);
    m_commandManager   = std::make_unique<juce::ApplicationCommandManager>();

    m_deviceManager.initialiseWithDefaultDevices(2, 2);
    m_transport->setAudioDeviceManager(&m_deviceManager);

    addAndMakeVisible(*m_waveform);
    addAndMakeVisible(*m_selectionOverlay);
    addAndMakeVisible(*m_spectrogram);
    addAndMakeVisible(*m_transport);
    m_spectrogram->setVisible(false); // start with waveform view

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

void MainComponent::setViewMode(ViewMode mode)
{
    m_viewMode = mode;
    bool showWaveform   = (mode == WaveformView);
    bool showSpectrogram = (mode == SpectrogramView);

    m_waveform->setVisible(showWaveform);
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

    m_waveform->setBounds(r);
    m_selectionOverlay->setBounds(r);
    m_spectrogram->setBounds(r);
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
        menu.addCommandItem(m_commandManager.get(), cmdUndo);
        menu.addCommandItem(m_commandManager.get(), cmdRedo);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdPlaySel);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdSilence);
        menu.addSeparator();
        menu.addCommandItem(m_commandManager.get(), cmdReverse);
        menu.addCommandItem(m_commandManager.get(), cmdNormalize);
        menu.addCommandItem(m_commandManager.get(), cmdFadeIn);
        menu.addCommandItem(m_commandManager.get(), cmdFadeOut);
    }
    else if (idx == 2)
    {
        menu.addCommandItem(m_commandManager.get(), cmdToggleView);
        menu.addSeparator();
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
    cmds.addArray({ cmdOpen, cmdPlayPause, cmdStop, cmdPlaySel, cmdZoomIn,
                     cmdZoomOut, cmdToggleView, cmdUndo, cmdRedo, cmdSilence,
                     cmdReverse, cmdNormalize, cmdFadeIn, cmdFadeOut });
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
    case cmdToggleView:
        info.setInfo("Toggle View (Ctrl+S)", "Switch between waveform and spectrogram", "View", 0);
        info.addDefaultKeypress('s', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdUndo:
        info.setInfo("Undo", "Undo last action", "Edit", 0);
        info.addDefaultKeypress('z', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdRedo:
        info.setInfo("Redo", "Redo last undone action", "Edit", 0);
        info.addDefaultKeypress('z', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier);
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
        info.addDefaultKeypress('n', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdFadeIn:
        info.setInfo("Fade In Selection", "Fade in the selected audio", "Process", 0);
        info.addDefaultKeypress('i', juce::ModifierKeys::ctrlModifier);
        break;
    case cmdFadeOut:
        info.setInfo("Fade Out Selection", "Fade out the selected audio", "Process", 0);
        info.addDefaultKeypress('o', juce::ModifierKeys::ctrlModifier);
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
    case cmdUndo:
        m_undoManager.undo();
        m_waveform->repaint();
        m_spectrogram->rebuildFromAudio();
        return true;
    case cmdRedo:
        m_undoManager.redo();
        m_waveform->repaint();
        m_spectrogram->rebuildFromAudio();
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
    case cmdToggleView:
        setViewMode(m_viewMode == WaveformView ? SpectrogramView : WaveformView);
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

void MainComponent::silenceSelection()
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

    // Clamp to buffer bounds
    startSample = juce::jlimit(0, buf->getNumSamples() - 1, startSample);
    numSamples  = juce::jmin(numSamples, buf->getNumSamples() - startSample);

    auto action = new AudioModifyAction(*m_fileManager, startSample, numSamples,
        [](juce::AudioBuffer<float>& audio, int start, int num)
        {
            for (int ch = 0; ch < audio.getNumChannels(); ++ch)
                audio.clear(ch, start, num);
        });

    action->onAudioChanged = [this]
    {
        m_waveform->repaint();
        if (m_viewMode == SpectrogramView)
            m_spectrogram->rebuildFromAudio();
    };

    m_undoManager.perform(action);
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

        if (m_viewMode == SpectrogramView)
            m_spectrogram->rebuildFromAudio();

        if (auto* peer = getPeer())
            peer->setTitle("Open Edison - " + file.getFileName());

        resized();
        repaint();
    }
}
