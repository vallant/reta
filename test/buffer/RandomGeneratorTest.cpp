

#include "buffer/RandomGenerator.h"

#include <catch2/catch.hpp>

TEST_CASE ("RandomGeneratorTest", "[unit][buffer][random]")
{
    auto generator = std::make_unique<RandomGenerator> (42);

    juce::AudioProcessor::BusesLayout monoLayout;
    monoLayout.inputBuses.add (juce::AudioChannelSet::mono());
    monoLayout.outputBuses.add (juce::AudioChannelSet::mono());

    juce::AudioProcessor::BusesLayout stereoLayout;
    stereoLayout.inputBuses.add (juce::AudioChannelSet::stereo());
    stereoLayout.outputBuses.add (juce::AudioChannelSet::stereo());

    auto sum = [] (const juce::AudioBuffer<float>& block) {
        auto sum = 0.0f;
        for ( auto i = 0; i < block.getNumChannels(); ++i )
        {
            for ( auto j = 0; j < block.getNumChannels(); ++j )
            {
                sum += std::abs (block.getSample (i, j));
            }
        }
        return sum;
    };

    SECTION ("generator returns buffer with correct layout")
    {
        generator->prepare (monoLayout, 44100.0, 32);

        auto block = generator->getNextBlock();
        REQUIRE (block.getNumSamples() == 32);
        REQUIRE (block.getNumChannels() == 1);
    }

    SECTION ("generator returns buffer with correct layout")
    {
        generator->prepare (stereoLayout, 44100.0, 128);

        auto block = generator->getNextBlock();
        REQUIRE (block.getNumSamples() == 128);
        REQUIRE (block.getNumChannels() == 2);
    }

    SECTION ("generator actually fills buffer with data")
    {
        generator->prepare (monoLayout, 44100.0, 128);
        auto block = generator->getNextBlock();
        REQUIRE (sum (block) != Approx (0.0f).epsilon (0.001f));
    }

    SECTION ("generator fills buffer with data between -1.0f and 1.0f")
    {
        generator->prepare (monoLayout, 44100.0, 128);
        auto block = generator->getNextBlock();

        auto actual = block.findMinMax (0, 0, 128);
        juce::Range<float> reference (-1.0f, 1.0f);
        REQUIRE (reference.contains (actual));
    }

    SECTION ("second generator with the same seed returns the same sequence")
    {
        auto generator2 = std::make_unique<RandomGenerator> (42);

        generator->prepare (monoLayout, 44100.0, 128);
        generator2->prepare (monoLayout, 44100.0, 128);

        auto block1 = generator->getNextBlock();
        auto block2 = generator2->getNextBlock();

        REQUIRE (block1.getNumSamples() == block2.getNumSamples());
        REQUIRE (block1.getNumChannels() == block2.getNumChannels());

        auto diff = 0.0f;
        for ( int i = 0; i < block1.getNumChannels(); ++i )
        {
            for ( int j = 0; j < block1.getNumSamples(); ++j )
            {
                diff += block1.getSample (i, j) - block2.getSample (i, j);
            }
        }
        REQUIRE (diff == Approx (0.0f).epsilon (0.001f));
    }

    SECTION ("second generator with different seed returns a different sequence")
    {
        auto generator2 = std::make_unique<RandomGenerator> (84);

        generator->prepare (monoLayout, 44100.0, 128);
        generator2->prepare (monoLayout, 44100.0, 128);

        auto block1 = generator->getNextBlock();
        auto block2 = generator2->getNextBlock();

        REQUIRE (block1.getNumSamples() == block2.getNumSamples());
        REQUIRE (block1.getNumChannels() == block2.getNumChannels());

        auto diff = 0.0f;
        for ( int i = 0; i < block1.getNumChannels(); ++i )
        {
            for ( int j = 0; j < block1.getNumSamples(); ++j )
            {
                diff += block1.getSample (i, j) - block2.getSample (i, j);
            }
        }
        REQUIRE (diff != Approx (0.0f).epsilon (0.001f));
    }

    SECTION ("Reset")
    {
        generator->prepare (monoLayout, 44100.0, 10);
        auto block1 = generator->getNextBlock();
        generator->reset();
        auto block2 = generator->getNextBlock();

        auto diff = 0.0f;
        for ( int i = 0; i < block1.getNumChannels(); ++i )
        {
            for ( int j = 0; j < block1.getNumSamples(); ++j )
            {
                diff += block1.getSample (i, j) - block2.getSample (i, j);
            }
        }
        REQUIRE (diff == Approx (0.0f).epsilon (0.001f));
    }
}