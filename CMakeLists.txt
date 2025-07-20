CMakeLists.txt

cmake_minimum_required(VERSION 3.15)
project(AnalogDelay VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# JUCE setup
set(JUCE_DIR "$ENV{JUCE_DIR}")
if(NOT JUCE_DIR)
    message(FATAL_ERROR "Please set JUCE_DIR environment variable to your JUCE folder")
endif()
add_subdirectory(${JUCE_DIR} JUCE)

juce_add_plugin(AnalogDelay
    COMPANY_NAME "YourCompany"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE AnDl
    FORMATS VST3 AU
    PRODUCT_NAME "Analog Delay"
)

target_sources(AnalogDelay PRIVATE
    PluginProcessor.cpp
    PluginEditor.cpp
    PluginProcessor.h
    PluginEditor.h
)

target_link_libraries(AnalogDelay PRIVATE
    juce::juce_audio_utils
)
```