cmake_minimum_required(VERSION 3.21)
project(OpenEdison VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── JUCE FetchContent ──
include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG        8.0.5
  GIT_SHALLOW    ON
)
FetchContent_GetProperties(JUCE)
if(NOT juce_POPULATED)
  set(JUCE_BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
  set(JUCE_BUILD_SHARED_LIBRARY ON CACHE BOOL "" FORCE)
  FetchContent_Populate(JUCE)
  add_subdirectory(${juce_SOURCE_DIR} ${juce_BINARY_DIR})
endif()

# ── OpenEdison executable ──
file(GLOB_RECURSE SOURCES Source/*.cpp Source/*.h)

juce_add_standalone_plugin(OpenEdison
  COMPANY_NAME     "OpenEdison"
  PLUGIN_NAME      "Open Edison"
  PLUGIN_MANUFACTURER_CODE "OEdi"
  PLUGIN_CODE      "OEd1"
  DESCRIPTION      "Lightweight waveform editor"
  VERSION          "0.1.0"
  IS_SYNTH         FALSE
  IS_MIDI_EFFECT   FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(OpenEdison PRIVATE ${SOURCES})

target_compile_definitions(OpenEdison PRIVATE
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
)

# Link JUCE modules
juce_generate_juce_header(OpenEdison)
target_link_libraries(OpenEdison PRIVATE
  juce::juce_audio_basics
  juce::juce_audio_devices
  juce::juce_audio_formats
  juce::juce_audio_processors
  juce::juce_audio_utils
  juce::juce_core
  juce::juce_data_structures
  juce::juce_events
  juce::juce_graphics
  juce::juce_gui_basics
  juce::juce_gui_extra
)

# Linux flags
if(UNIX AND NOT APPLE)
  target_link_options(OpenEdison PRIVATE
    "-Wl,--no-undefined"
  )
endif()
