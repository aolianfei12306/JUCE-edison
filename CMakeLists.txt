cmake_minimum_required(VERSION 3.21)
project(OpenEdison VERSION 0.1.0 LANGUAGES C CXX)

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
  FetchContent_Populate(JUCE)
  add_subdirectory(${juce_SOURCE_DIR} ${juce_BINARY_DIR})
endif()

# ── OpenEdison executable (manual add_executable to avoid juce_add_* cmake bugs) ──
add_executable(OpenEdison)

file(GLOB_RECURSE SOURCES Source/*.cpp)
target_sources(OpenEdison PRIVATE ${SOURCES})

target_include_directories(OpenEdison PRIVATE ${juce_SOURCE_DIR}/modules)

target_compile_definitions(OpenEdison PRIVATE
  JUCE_STANDALONE_APPLICATION=1
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
)

target_link_libraries(OpenEdison PRIVATE
  juce::juce_core
  juce::juce_graphics
  juce::juce_gui_basics
  juce::juce_gui_extra
  juce::juce_events
  juce::juce_data_structures
  juce::juce_audio_basics
  juce::juce_audio_devices
  juce::juce_audio_formats
  juce::juce_audio_utils
)

# Linux flags
if(UNIX AND NOT APPLE)
  target_link_options(OpenEdison PRIVATE
    "-Wl,--no-undefined"
  )
endif()
