#include "../util/PluginStub.h"

#include <catch2/catch.hpp>
#include <ipog/ActsConfigAdapter.h>

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

TEST_CASE ("ActsConfigAdapter", "[unit][acts]")
{
    SECTION ("Translation from plugin info to acts input works")
    {
        TestPlugin p;
        juce::String reference = "[System]\n"
                                 "Name: foobar\n"
                                 "[Parameter]\n"
                                 "BlockSize (enum): 32, 64, 128, 256, 512, 1024, 2048, 4096\n"
                                 "SampleRate (enum): 44100, 48000, 88200, 96000\n"
                                 "BusLayout (enum): 0, 1, 2, 3\n"
                                 "Signal (enum): 0, 1, 2, 3\n"
                                 "Parameter0 (enum): -1, 0, 0.25, 0.5, 0.75, 1\n"
                                 "Parameter1 (enum): -1, 0, 0.25, 0.5, 0.75, 1\n"
                                 "Parameter2 (enum): -1, 0, 0.25, 0.5, 0.75, 1\n"
                                 "Parameter3 (enum): -1, 0, 0.25, 0.5, 0.75, 1\n"
                                 "Parameter4 (enum): -1, 0, 0.25, 0.5, 0.75, 1";
        REQUIRE (ActsConfigAdapter::inputToActs (&p) == reference);
    }

    SECTION ("Translation from acts output to testconfig works")
    {
        TestPlugin plugin;
        juce::String input =
            "# ACTS Test Suite Generation: Thu Sep 24 13:39:42 CEST 2020\n"
            "#  * represents don't care value \n"
            "# Degree of interaction coverage: 2\n"
            "# Number of parameters: 10\n"
            "# Maximum number of values per parameter: 7\n"
            "# Number of configurations: 45\n"
            "# ****************************************************\n"
            "BlockSize,SampleRate,BusLayout,Signal,Parameter0,Parameter1,Parameter2,Parameter3,Parameter4\n";

        juce::StringArray values1 = {"32", "48000", "1", "1", "0.0", "0.25", "0.25", "0.25", "0.25"};
        juce::StringArray values2 = {"32", "88200", "2", "2", "0.25", "0.5", "0.5", "0.5", "0.5"};
        juce::StringArray values3 = {"32", "96000", "3", "3", "0.5", "0.75", "0.75", "0.75", "0.75"};
        for ( auto& value : {values1, values2, values3} )
        {
            input += "\"";
            input += value.joinIntoString ("\",\"");
            input += "\"\n";
        }
        juce::AudioProcessor::BusesLayout layout1;
        layout1.inputBuses.add (juce::AudioChannelSet::mono());
        layout1.outputBuses.add (juce::AudioChannelSet::stereo());

        juce::AudioProcessor::BusesLayout layout2;
        layout2.inputBuses.add (juce::AudioChannelSet::stereo());
        layout2.outputBuses.add (juce::AudioChannelSet::mono());

        juce::AudioProcessor::BusesLayout layout3;
        layout3.inputBuses.add (juce::AudioChannelSet::stereo());
        layout3.outputBuses.add (juce::AudioChannelSet::stereo());

        TestConfiguration config1;
        config1.blockSize = 32;
        config1.sampleRate = 48000.0;
        config1.layout = BusLayout (layout1);
        config1.inputSignal = 1;
        config1.parameters.resize (5);

        config1.parameters[0] = {"Parameter0", 0.0f};
        config1.parameters[1] = {"Parameter1", 0.25f};
        config1.parameters[2] = {"Parameter2", 0.25f};
        config1.parameters[3] = {"Parameter3", 0.25f};
        config1.parameters[4] = {"Parameter4", 0.25f};

        TestConfiguration config2;
        config2.blockSize = 32;
        config2.sampleRate = 88200.0;
        config2.layout = BusLayout (layout2);
        config2.inputSignal = 2;
        config2.parameters.resize (5);
        config2.parameters[0] = {"Parameter0", 0.25f};
        config2.parameters[1] = {"Parameter1", 0.5f};
        config2.parameters[2] = {"Parameter2", 0.5f};
        config2.parameters[3] = {"Parameter3", 0.5f};
        config2.parameters[4] = {"Parameter4", 0.5f};

        TestConfiguration config3;
        config3.blockSize = 32;
        config3.sampleRate = 96000.0;
        config3.layout = BusLayout (layout3);
        config3.inputSignal = 3;
        config3.parameters.resize (5);

        config3.parameters[0] = {"Parameter0", 0.5f};
        config3.parameters[1] = {"Parameter1", 0.75f};
        config3.parameters[2] = {"Parameter2", 0.75f};
        config3.parameters[3] = {"Parameter3", 0.75f};
        config3.parameters[4] = {"Parameter4", 0.75f};

        std::vector<TestConfiguration> reference {config1, config2, config3};
        auto actual = ActsConfigAdapter::configFromActs (&plugin, input);
        REQUIRE (reference.size() == actual.size());
        auto results = TestConfiguration::compare (reference, actual);
        for ( auto& result : results )
        {
            bool equal = true;
            std::for_each (result.begin(), result.end(), [&equal] (const auto& test) { equal &= test.passed; });
            REQUIRE (equal);
        }
    }
}
