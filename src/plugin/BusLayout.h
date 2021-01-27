#pragma once

#include <juce_audio_plugin_client/juce_audio_plugin_client.h>
#include <nlohmann/json.hpp>
#include <test/TestInfo.h>

// forward declaration of the plugin class
class Plugin;

/* A class representing the BusLayout of a plugin */
class BusLayout
{
public:
    BusLayout() = default;

    /*  Constructor using the juce BusesLayout */
    // cppcheck-suppress noExplicitConstructor
    BusLayout (const juce::AudioProcessor::BusesLayout& layout);

    /*  Constructor using a single input- and output-set */
    BusLayout (const juce::AudioChannelSet& input, const juce::AudioChannelSet& output);

    /*  Constructor using a vector of input- and output-sets */
    BusLayout (const std::vector<juce::AudioChannelSet>& inputs, const std::vector<juce::AudioChannelSet>& outputs);

    /*  Return available layouts of the plugin */
    static std::vector<BusLayout> getLayouts (const Plugin* plugin);

    /*  Helper method that */
    static std::vector<std::vector<std::string>> layoutsPerBus (const std::vector<std::string>& channelSets,
                                                                size_t numBuses);

    /*  Return the number of channels of this buslayout */
    size_t getNumChannels() const;

    /*  Implicit conversion to a juce buslayout */
    operator juce::AudioProcessor::BusesLayout() const;

    /*  Comparison operators */
    bool operator== (const BusLayout& other) const;
    bool operator!= (const BusLayout& other) const;
    bool operator< (const BusLayout& other) const;

    /*  Method to compare it to another layout, generating a test info struct */
    TestInfo compareTo (const BusLayout& other) const;

    /*  Method to compare a vector of buslayouts to another vector of buslayouts */
    static std::vector<TestInfo> compare (const std::vector<BusLayout>& lhs, const std::vector<BusLayout>& rhs);

    std::string getDescription() const;

private:
    void init (const juce::AudioProcessor::BusesLayout& layout);
    static juce::AudioChannelSet fromDescription (const juce::String& description);

    static std::string join (const std::vector<std::string>& vector);
    std::vector<std::string> inputLayouts;
    std::vector<std::string> outputLayouts;

    static std::vector<std::string> getChannelSetStrings();
    static std::vector<juce::AudioChannelSet> getChannelSets();

    NLOHMANN_DEFINE_TYPE_INTRUSIVE (BusLayout, inputLayouts, outputLayouts);
};

using BusLayouts = std::vector<BusLayout>;
