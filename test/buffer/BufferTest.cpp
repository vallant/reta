#include <buffer/Buffer.h>
#include <catch2/catch.hpp>

TEST_CASE ("BufferTest", "[unit]")
{
    SECTION ("Empty buffer is cleared")
    {
        Buffer buffer (2, 32, 44100.0);
        REQUIRE (buffer.hasBeenCleared());
        for ( int i = 0; i < 32; ++i )
        {
            REQUIRE (buffer.getSample (0, i) == Approx (0.0f));
            REQUIRE (buffer.getSample (1, i) == Approx (0.0f));
        }
    }

    SECTION ("Addition works")
    {
        Buffer lhs (1, 32, 44100.0);
        Buffer rhs (1, 32, 44100.0);
        lhs.fill (1.0f);
        rhs.fill (2.0f);

        lhs += rhs;

        REQUIRE (lhs.getNumSamples() == 64);
        REQUIRE (rhs.getNumSamples() == 32);
        for ( int sample = 0; sample < 64; ++sample )
        {
            auto target = sample < 32 ? 1.0f : 2.0f;
            REQUIRE (lhs.getSample (0, sample) == target);
        }

        for ( int sample = 0; sample < 32; ++sample )
        {
            REQUIRE (rhs.getSample (0, sample) == 2.0f);
        }
    }

    SECTION ("read/write works")
    {
        Buffer original (1, 32, 44100.0);
        original.fill (1.0f);

        juce::MemoryBlock block;
        auto output = std::make_unique<juce::MemoryOutputStream> (block, false);

        original.write (std::move (output));

        auto input = std::make_unique<juce::MemoryInputStream> (block, false);
        auto read = Buffer::read (std::move (input));

        REQUIRE (original.getNumSamples() == read.getNumSamples());
        REQUIRE (original.getNumChannels() == read.getNumChannels());
        for ( int channel = 0; channel < original.getNumChannels(); ++channel )
        {
            for ( int sample = 0; sample < original.getNumSamples(); ++sample )
            {
                REQUIRE (original.getSample (channel, sample)
                         == Approx (read.getSample (channel, sample)).epsilon (0.001f));
            }
        }
    }

    SECTION ("Fill works")
    {
        Buffer lhs (1, 32, 44100.0);
        lhs.fill (1.0f);
        for ( int i = 0; i < 32; ++i )
        {
            REQUIRE (lhs.getSample (0, i) == Approx (1.0f));
        }
    }

    SECTION ("foreach works")
    {
        Buffer lhs (2, 3, 44100.0);
        std::vector<std::pair<int, int>> reference = {{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}};
        std::vector<std::pair<int, int>> actual;

        lhs.forEach ([&actual] (int channel, int sample) { actual.emplace_back (channel, sample); });

        REQUIRE (reference == actual);
    }

    SECTION ("Override actually overrides, not appends")
    {
        Buffer original (1, 32, 44100.0);
        original.fill (1.0f);

        juce::MemoryBlock block;
        auto output = std::make_unique<juce::MemoryOutputStream> (block, false);
        original.write (std::move (output));

        auto input = std::make_unique<juce::MemoryInputStream> (block, false);
        auto read = Buffer::read (std::move (input));

        REQUIRE (original.getNumSamples() == read.getNumSamples());
        REQUIRE (original.getNumChannels() == read.getNumChannels());
        for ( int channel = 0; channel < original.getNumChannels(); ++channel )
        {
            for ( int sample = 0; sample < original.getNumSamples(); ++sample )
            {
                REQUIRE (original.getSample (channel, sample)
                         == Approx (read.getSample (channel, sample)).epsilon (0.001f));
            }
        }
    }

    SECTION ("Set sample rate works")
    {
        Buffer buffer (1, 1, 44100.0);
        REQUIRE (buffer.getSampleRate() == 44100.0);
        buffer.setSampleRate (1.0);
        REQUIRE (buffer.getSampleRate() == 1.0);
    }
}