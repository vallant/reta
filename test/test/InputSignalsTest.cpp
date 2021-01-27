#include "../../src/test/InputSignals.h"

#include "catch2/catch.hpp"

TEST_CASE ("InputSignalsTest", "[unit][input][test]")
{
    SECTION ("Size correct")
    {
        REQUIRE (InputSignals::size() == 4);
    }

    SECTION ("Get with params works")
    {
        InputSignals signals;
        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add (juce::AudioChannelSet::mono());
        layout.outputBuses.add (juce::AudioChannelSet::mono());
        auto blocks = signals.get (0, 100, 48000.0, layout);

        REQUIRE (blocks.size() == 441);
        for ( auto& block : blocks )
        {
            REQUIRE (block.getSampleRate() == 48000.0);
            REQUIRE (block.getNumSamples() == 100);
            REQUIRE (block.getNumChannels() == 1);
        }
    }

    SECTION ("Get with params works when block sizes doesnt divide cleanly")
    {
        InputSignals signals;
        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add (juce::AudioChannelSet::mono());
        layout.outputBuses.add (juce::AudioChannelSet::mono());
        auto blocks = signals.get (0, 40000, 48000.0, layout);

        REQUIRE (blocks.size() == 2);
        REQUIRE (blocks[0].getNumSamples() == 40000);
        REQUIRE (blocks[1].getNumSamples() == 4100);
    }
}
