#include "../util/PluginStub.h"

#include <catch2/catch.hpp>
#include <ipog/DitherAdapter.h>
#include <util/Util.h>

struct TestPlugin : PluginStub
{
    size_t getNumParameters() const override
    {
        return 5;
    }
    std::vector<juce::String> getParameterNames() const override
    {
        return {"Parameter0", "Parameter1", "Parameter2", "Parameter3", "Parameter4"};
    }
    BusLayouts getLayouts() const override
    {
        juce::AudioProcessor::BusesLayout layout1;
        layout1.inputBuses.add (juce::AudioChannelSet::mono());
        layout1.outputBuses.add (juce::AudioChannelSet::mono());

        juce::AudioProcessor::BusesLayout layout2;
        layout2.inputBuses.add (juce::AudioChannelSet::mono());
        layout2.outputBuses.add (juce::AudioChannelSet::stereo());

        juce::AudioProcessor::BusesLayout layout3;
        layout3.inputBuses.add (juce::AudioChannelSet::stereo());
        layout3.outputBuses.add (juce::AudioChannelSet::mono());

        juce::AudioProcessor::BusesLayout layout4;
        layout4.inputBuses.add (juce::AudioChannelSet::stereo());
        layout4.outputBuses.add (juce::AudioChannelSet::stereo());
        return {BusLayout (layout1), BusLayout (layout2), BusLayout (layout3), BusLayout (layout4)};
    }
    Info getInfo() const override
    {
        Info info;
        info.name = "foobar";
        info.parameters.resize (5);
        for ( int i = 0; i < 5; ++i )
            info.parameters[i].name = ("Parameter" + juce::String (i)).toStdString();
        info.layouts = getLayouts();
        return info;
    }
};

TEST_CASE ("DitherAdapter works without params", "[unit][ipog][dither]")
{
    TestPlugin plugin;
    auto configs = DitherAdapter::generate (&plugin, GenerateCommand::Parameters());
    REQUIRE (configs.size() == 62);
}

TEST_CASE ("DitherAdapter works when excluding params", "[unit][ipog][dither]")
{
    TestPlugin plugin;
    GenerateCommand::Parameters parameters;
    parameters.exclude.push_back ("Parameter0");
    auto configs = DitherAdapter::generate (&plugin, parameters);
    auto findLambda = [] (const TestConfiguration::Parameter& param) { return param.name == "Parameter0"; };
    for ( auto& config : configs )
    {
        auto it = std::find_if (config.parameters.begin(), config.parameters.end(), findLambda);
        REQUIRE (it == config.parameters.end());
    }
    REQUIRE (configs.size() == 59);
}

TEST_CASE ("DitherAdapter excludes with wildcard", "[unit][ipog][dither]")
{
    TestPlugin plugin;
    GenerateCommand::Parameters parameters;
    parameters.exclude.push_back ("*0");
    auto configs = DitherAdapter::generate (&plugin, parameters);
    auto findLambda = [] (const TestConfiguration::Parameter& param) { return param.name == "Parameter0"; };
    for ( auto& config : configs )
    {
        auto it = std::find_if (config.parameters.begin(), config.parameters.end(), findLambda);
        REQUIRE (it == config.parameters.end());
    }
    REQUIRE (configs.size() == 59);
}