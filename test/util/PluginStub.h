#pragma once
#include <plugin/Plugin.h>

struct PluginStub : Plugin
{
    bool isLayoutSupported (const juce::AudioProcessor::BusesLayout& /*layout*/) const override
    {
        return false;
    }
    std::pair<size_t, size_t> getNumBuses() const override
    {
        return std::pair<size_t, size_t>();
    }
    void prepare (double /*sampleRate*/, size_t /*maxNumSamples*/,
                  const juce::AudioProcessor::BusesLayout& /*layout*/) override
    {
    }
    void setParameter (const juce::String& /*name*/, float /*value*/) override {}
    size_t getNumParameters() const override
    {
        return 0;
    }
    void process (juce::AudioSampleBuffer& /*buffer*/) override {}
    BusLayouts getLayouts() const override
    {
        return {};
    }
    void reset() override {}
    Info getInfo() const override
    {
        return {};
    };

    std::vector<juce::String> getParameterNames() const override
    {
        return {};
    }
    juce::String getCurrentParameterText (const juce::String& /*name*/) const override
    {
        return {};
    }
    float getValueFromText (const juce::String& /*name*/) const override
    {
        return -1.0f;
    }
};