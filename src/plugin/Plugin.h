#pragma once

#include "BusLayout.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <nlohmann/json.hpp>
#include <string>
#include <test/TestInfo.h>
#include <vector>

/* Abstract base class to represent a plugin */
class Plugin
{
public:
    virtual ~Plugin() = default;

    /* Returns whether the provided buslayout is supported by the plugin */
    virtual bool isLayoutSupported (const juce::AudioProcessor::BusesLayout& layout) const = 0;

    /* Return the number of buses, as a (input, output) pair */
    virtual std::pair<size_t, size_t> getNumBuses() const = 0;

    /* Prepare the plugin for audio processing */
    virtual void prepare (double sampleRate, size_t maxNumSamples, const juce::AudioProcessor::BusesLayout& layout) = 0;

    /* Set a parameter by name */
    virtual void setParameter (const juce::String& name, float value) = 0;

    /* Return the number of parameters */
    virtual size_t getNumParameters() const = 0;

    /* Process the audio sample buffer. Be sure having prepared the plugin before */
    virtual void process (juce::AudioSampleBuffer& buffer) = 0;

    /* Return the layouts that are supported by the plugin */
    virtual std::vector<BusLayout> getLayouts() const = 0;

    /* Return a vector of parameter names */
    virtual std::vector<juce::String> getParameterNames() const = 0;

    /* Return the current string representation of the parameter */
    virtual juce::String getCurrentParameterText (const juce::String& name) const = 0;

    /* Return the current float representation of the parameter */
    virtual float getValueFromText (const juce::String& name) const = 0;

    /* Reset the plugin to it initial state */
    virtual void reset() = 0;

    /* A struct holding all available information for plugin parameters */
    struct ParameterInfo
    {
        std::string name;
        std::string label;
        float value;
        int numSteps;
        int category;
        float defaultValue;
        int index;
        bool automatable;
        bool discrete;
        bool meta;
        bool inverted;
        bool boolean;
        std::vector<float> values;

        std::vector<TestInfo> compareTo (const ParameterInfo& other) const;

        static std::vector<TestInfos> compare (const std::vector<ParameterInfo>& lhs,
                                               const std::vector<ParameterInfo>& rhs);

        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Plugin::ParameterInfo, name, label, value, numSteps, category, defaultValue,
                                        index, automatable, discrete, meta, inverted, boolean, values)
    };

    /* A struct to hold all available information for plugins */
    struct Info
    {
        std::string name;
        int inputBuses;
        int outputBuses;
        bool acceptsMidi;
        bool isMidiEffect;
        bool producesMidi;
        int latency;
        double tail;
        std::string precision;
        bool hasEditor;

        std::vector<BusLayout> layouts;
        std::vector<ParameterInfo> parameters;

        std::vector<TestInfo> compareTo (const Info& other);

        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Plugin::Info, name, inputBuses, outputBuses, acceptsMidi, isMidiEffect,
                                        producesMidi, latency, tail, precision, hasEditor, layouts, parameters)
    };

    /* Return the plugin information of the plugin */
    virtual Info getInfo() const = 0;
};
