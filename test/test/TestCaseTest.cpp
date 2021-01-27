#include "../util/FileSystemStub.h"
#include "../util/PluginStub.h"

#include <catch2/catch.hpp>
#include <test/TestCase.h>

struct P : PluginStub
{
    size_t getNumParameters() const override
    {
        return 1;
    }
    void process (juce::AudioSampleBuffer& buffer) override
    {
        buffers.push_back (buffer);
    }

    void setParameter (const juce::String& name, float value) override
    {
        params2[name] = value;
    }
    void prepare (double rate, size_t size, const juce::AudioProcessor::BusesLayout& l) override
    {
        sampleRate = rate;
        numSamples = size;
        layout = l;
    }

    std::vector<juce::AudioSampleBuffer> buffers;

    double sampleRate;
    size_t numSamples;
    juce::AudioProcessor::BusesLayout layout;
    std::map<juce::String, float> params2;
};
TEST_CASE ("TestCase unit tests", "[unit][test]")
{
    auto plugin = new P;
    auto fs = new FileSystemStub;
    auto path = "foobar";

    TestCase c (std::unique_ptr<Plugin> (plugin), path, std::unique_ptr<FileSystem> (fs));

    TestConfiguration config;
    config.parameters.push_back ({"foo", 0.42f});
    config.sampleRate = 10000.0;
    config.blockSize = 1234;
    config.inputSignal = 3;

    juce::AudioProcessor::BusesLayout l;
    l.inputBuses.add (juce::AudioChannelSet::mono());
    l.outputBuses.add (juce::AudioChannelSet::mono());

    config.layout = BusLayout (l);

    SECTION ("plugin is prepared correctly")
    {
        c.execute ({config});

        REQUIRE (plugin->sampleRate == Approx (10000.0));
        REQUIRE (plugin->numSamples == 1234);
        REQUIRE (plugin->layout == l);
        REQUIRE (plugin->params2["foo"] == Approx (0.42f));
    }

    SECTION ("plugin receives correct signal with correct block sizes")
    {
        c.execute ({config});
        InputSignals signals;
        auto reference = signals.get (3, 1234, 10000.0, l);
        REQUIRE (reference.size() == plugin->buffers.size());
        float diff = 0.0f;
        for ( size_t block = 0; block < reference.size(); ++block )
        {
            auto& lhs = reference[block];
            auto& rhs = plugin->buffers[block];
            REQUIRE (lhs.getNumChannels() == rhs.getNumChannels());
            REQUIRE (lhs.getNumSamples() == rhs.getNumSamples());
            for ( int channel = 0; channel < lhs.getNumChannels(); ++channel )
            {
                for ( int sample = 0; sample < lhs.getNumSamples(); ++sample )
                {
                    diff += std::abs (lhs.getSample (channel, sample) - rhs.getSample (channel, sample));
                }
            }
        }
        REQUIRE (diff == Approx (0.0f).margin (0.0001f));
    }

    SECTION ("testcases executes multiple test configs")
    {
        auto config2 = config;
        config2.parameters[0].value = 0.2f;

        config2.sampleRate = 12345.6;
        config2.blockSize = 12345;
        c.execute ({config, config2});
        REQUIRE (plugin->params2["foo"] == Approx (0.2f));
        REQUIRE (plugin->sampleRate == Approx (12345.6));
        REQUIRE (plugin->numSamples == 12345);
    }

    SECTION ("testcase writes buffer to file system correctly")
    {
        config.sampleRate = 44100.0;
        c.execute ({config});
        REQUIRE (fs->writtenFiles[0] == "foobar" + FileSystem::sep() + "info.json");
        REQUIRE (fs->writtenFiles[1] == "foobar" + FileSystem::sep() + "config.json");
        REQUIRE (fs->writtenFiles[2] == "foobar" + FileSystem::sep() + "test0.json");
        REQUIRE (fs->writtenFiles[3] == "foobar" + FileSystem::sep() + "test0.wav");
        REQUIRE (fs->writtenBlocks[0].getSize() > 0);

        auto stream = std::make_unique<juce::MemoryInputStream> (fs->writtenBlocks[0], false);
        auto actual = Buffer::read (std::move (stream));

        auto reference = InputSignals().get (3);
        REQUIRE (actual.getSampleRate() == Approx (reference.getSampleRate()));
        REQUIRE (actual.getNumSamples() == reference.getNumSamples());
        REQUIRE (actual.getNumChannels() == 1);

        for ( int channel = 0; channel < reference.getNumChannels(); ++channel )
        {
            for ( int sample = 0; sample < reference.getNumSamples(); ++sample )
            {
                INFO (sample);
                INFO (channel);
                auto diff = std::abs (actual.getSample (channel, sample) - reference.getSample (channel, sample));
                REQUIRE (diff == Approx (0.0f).margin (0.0001f));
            }
        }
    }
}
