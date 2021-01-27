
#include "BusLayout.h"

#include "Plugin.h"

#include <util/Permutation.h>

std::vector<BusLayout> BusLayout::getLayouts (const Plugin* plugin)
{
    std::vector<BusLayout> supportedLayouts;
    auto possibleSets = getChannelSetStrings();
    auto inputPermutations = layoutsPerBus (possibleSets, plugin->getNumBuses().first);
    auto outputPermutations = layoutsPerBus (possibleSets, plugin->getNumBuses().second);
    for ( auto& input : inputPermutations )
    {
        for ( auto& output : outputPermutations )
        {
            BusLayout layout;
            layout.inputLayouts = input;
            layout.outputLayouts = output;

            if ( plugin->isLayoutSupported (layout) )
                supportedLayouts.push_back (layout);
        }
    }
    return supportedLayouts;
}

std::vector<std::string> BusLayout::getChannelSetStrings()
{
    const auto& sets = getChannelSets();
    std::vector<std::string> strings;
    for ( auto& set : sets )
        strings.push_back (set.getDescription().toStdString());
    return strings;
}

std::vector<juce::AudioChannelSet> BusLayout::getChannelSets()
{
    return {juce::AudioChannelSet::mono(), juce::AudioChannelSet::stereo()};
}

juce::AudioChannelSet BusLayout::fromDescription (const juce::String& description)
{
    const auto& sets = getChannelSets();
    auto findLambda = [description] (const juce::AudioChannelSet& element) {
        return element.getDescription() == description;
    };
    auto it = std::find_if (sets.begin(), sets.end(), findLambda);
    return it != sets.end() ? *it : juce::AudioChannelSet();
}

std::vector<std::vector<std::string>> BusLayout::layoutsPerBus (const std::vector<std::string>& channelSets,
                                                                size_t numBuses)
{
    std::vector<std::vector<std::string>> channels;

    const auto permutations = Permutation::generatePermutations (channelSets.size(), numBuses);
    for ( const auto& permutation : permutations )
    {
        std::vector<std::string> busConfig;
        for ( const auto& index : permutation )
        {
            busConfig.push_back (channelSets[index]);
        }
        channels.push_back (busConfig);
    }

    return channels;
}

size_t BusLayout::getNumChannels() const
{
    size_t sumInput = 0;
    size_t sumOutput = 0;
    for ( const auto& layout : inputLayouts )
    {
        sumInput += fromDescription (layout).size();
    }

    for ( const auto& layout : outputLayouts )
    {
        sumOutput += fromDescription (layout).size();
    }
    return juce::jmax (sumInput, sumOutput);
}

BusLayout::operator juce::AudioProcessor::BusesLayout() const
{
    juce::AudioProcessor::BusesLayout layout;
    for ( auto& l : inputLayouts )
        layout.inputBuses.add (fromDescription (l));
    for ( auto& l : outputLayouts )
        layout.outputBuses.add (fromDescription (l));
    return layout;
}

BusLayout::BusLayout (const juce::AudioProcessor::BusesLayout& layout)
{
    init (layout);
}

BusLayout::BusLayout (const juce::AudioChannelSet& input, const juce::AudioChannelSet& output)
{
    init ({input, output});
}

BusLayout::BusLayout (const std::vector<juce::AudioChannelSet>& inputs,
                      const std::vector<juce::AudioChannelSet>& outputs)
{
    juce::AudioProcessor::BusesLayout layout;
    for ( const auto& input : inputs )
        layout.inputBuses.add (input);
    for ( const auto& output : outputs )
        layout.outputBuses.add (output);

    init (layout);
}

bool BusLayout::operator== (const BusLayout& other) const
{
    return nlohmann::json (*this) == nlohmann::json (other);
}
bool BusLayout::operator!= (const BusLayout& other) const
{
    return ! (*this == other);
}

bool BusLayout::operator< (const BusLayout& other) const
{
    return nlohmann::json (*this) < nlohmann::json (other);
}

void BusLayout::init (const juce::AudioProcessor::BusesLayout& layout)
{
    for ( auto& i : layout.inputBuses )
        inputLayouts.push_back (i.getDescription().toStdString());

    for ( auto& o : layout.outputBuses )
        outputLayouts.push_back (o.getDescription().toStdString());
}

std::vector<TestInfo> BusLayout::compare (const std::vector<BusLayout>& lhs, const std::vector<BusLayout>& rhs)
{
    std::vector<TestInfo> tests;

    tests.push_back ({"Number of supported layout matches", std::to_string (lhs.size()), std::to_string (rhs.size()),
                      lhs.size() == rhs.size()});
    for ( size_t i = 0; i < lhs.size() && i < rhs.size(); ++i )
    {
        auto expected = lhs[i].getDescription();
        auto actual = rhs[i].getDescription();

        tests.push_back ({"BusLayout #" + std::to_string (i) + " matches", expected, actual, expected == actual});
    }

    return tests;
}
std::string BusLayout::join (const std::vector<std::string>& vector)
{
    juce::StringArray array;
    array.addArray (vector.begin(), vector.end());
    return array.joinIntoString ("-").toStdString();
}

TestInfo BusLayout::compareTo (const BusLayout& other) const
{
    auto expected = join (inputLayouts) + " <=> " + join (outputLayouts);
    auto actual = join (other.inputLayouts) + " <=> " + join (other.outputLayouts);

    return {"BusLayout matches", expected, actual, expected == actual};
}

std::string BusLayout::getDescription() const
{
    return join (inputLayouts) + " <=> " + join (outputLayouts);
}