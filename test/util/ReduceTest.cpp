#include "command/ReduceCommand.h"

#include <catch2/catch.hpp>

TEST_CASE ("ReduceTest", "[unit][reduce]")
{
    TestConfiguration config;

    config.blockSize = 1;
    config.inputSignal = 2;
    config.sampleRate = 3.0;
    config.layout = BusLayout (juce::AudioChannelSet::mono(), juce::AudioChannelSet::mono());
    config.parameters.push_back ({"1", 0.0f});
    config.parameters.push_back ({"2", 0.5f});
    config.parameters.push_back ({"3", 1.0f});

    SECTION ("Partition keeps doesn't change basic config")
    {
        auto partitions = ReduceCommand::split (config, 2);

        for ( auto& partition : partitions )
        {
            REQUIRE (partition.blockSize == config.blockSize);
            REQUIRE (partition.inputSignal == config.inputSignal);
            REQUIRE (partition.sampleRate == Approx (3.0));
            REQUIRE (partition.layout == config.layout);
        }
    }

    SECTION ("Partition computes correct sizes and parameters")
    {
        auto partitions = ReduceCommand::split (config, 2);
        REQUIRE (partitions.size() == 2);
        REQUIRE (partitions[0].parameters.size() == 2);
        REQUIRE (partitions[0].parameters[0].name == "1");
        REQUIRE (partitions[0].parameters[1].name == "2");

        REQUIRE (partitions[1].parameters.size() == 1);
        REQUIRE (partitions[1].parameters[0].name == "3");

        config.parameters.push_back ({"4", 1.5f});
        partitions = ReduceCommand::split (config, 2);
        REQUIRE (partitions.size() == 2);
        REQUIRE (partitions[0].parameters.size() == 2);
        REQUIRE (partitions[1].parameters.size() == 2);
    }

    SECTION ("Partition edge cases")
    {
        auto partitions = ReduceCommand::split (config, 3);
        REQUIRE (partitions.size() == 3);

        partitions = ReduceCommand::split (config, 4);
        REQUIRE (partitions.size() == 1);

        TestConfiguration emptyConfig;
        partitions = ReduceCommand::split (emptyConfig, 1);
        REQUIRE (partitions.size() == 1);
    }

    SECTION ("Complements basic")
    {
        auto partitions = ReduceCommand::split (config, 2);
        auto complements = ReduceCommand::computeComplements (config, partitions);
        REQUIRE (complements.size() == 2);
        REQUIRE (complements[0].parameters.size() == 1);
        REQUIRE (complements[0].parameters[0].name == "3");

        REQUIRE (complements[1].parameters.size() == 2);
        REQUIRE (complements[1].parameters[0].name == "1");
        REQUIRE (complements[1].parameters[1].name == "2");
    }
}