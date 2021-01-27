#pragma once
#include "BusLayout.h"
#include "Plugin.h"

#include <fs/FileSystemImpl.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

/* Implementation of the abstract plugin class */
class PluginImpl : public Plugin
{
public:
    explicit PluginImpl (juce::String fileName, std::unique_ptr<FileSystem> fs = std::make_unique<FileSystemImpl>());

    void prepare (double sampleRate, size_t maxNumSamples, const juce::AudioProcessor::BusesLayout& layout) override;
    void setParameter (const juce::String& name, float value) override;
    juce::String getCurrentParameterText (const juce::String& name) const override;
    float getValueFromText (const juce::String& name) const override;
    size_t getNumParameters() const override;
    void process (juce::AudioSampleBuffer& buffer) override;

    std::pair<size_t, size_t> getNumBuses() const override;

    Plugin::Info getInfo() const override;

    std::vector<BusLayout> getLayouts() const override;
    bool isLayoutSupported (const juce::AudioProcessor::BusesLayout& layout) const override;
    void reset() override;
    std::vector<juce::String> getParameterNames() const override;

private:
    void createPluginInstance();

    std::vector<float> getDiscreteValues (const juce::AudioProcessorParameter* parameter) const;
    std::vector<Plugin::ParameterInfo> getParameterInfo() const;

    juce::OwnedArray<juce::PluginDescription> plugins;
    std::unique_ptr<FileSystem> fs;
    std::unique_ptr<juce::AudioPluginInstance> instance;
    juce::String fileName;
    juce::MemoryBlock defaultConfig;
    bool init = false;
};
