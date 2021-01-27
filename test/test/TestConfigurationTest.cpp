#include "../util/PluginStub.h"
#include "test/ConfigGenerator.h"

#include <catch2/catch.hpp>
#include <test/InputSignals.h>
#include <test/TestConfiguration.h>

TEST_CASE ("TestConfiguration", "[unit][test][testconfig]")
{
    TestConfiguration config;
    config.blockSize = 1;
    config.sampleRate = 2.0;
    config.inputSignal = 1;
    juce::AudioProcessor::BusesLayout layout;
    layout.inputBuses.add (juce::AudioChannelSet::stereo());
    layout.inputBuses.add (juce::AudioChannelSet::stereo());
    layout.outputBuses.add (juce::AudioChannelSet::mono());
    config.layout = layout;
    config.parameters.push_back ({"foo", 0.5f});

    struct TestPlugin : PluginStub
    {
        void prepare (double sampleRate, size_t maxNumSamples, const juce::AudioProcessor::BusesLayout& layout) override
        {
            rate = sampleRate;
            numSamples = maxNumSamples;
            busLayout = layout;
        }
        void setParameter (const juce::String& name, float value) override
        {
            params[name] = value;
        }

        double rate;
        size_t numSamples;
        juce::AudioProcessor::BusesLayout busLayout;
        std::map<juce::String, float> params;
    };

    struct TestPlugin2 : PluginStub
    {
        size_t getNumParameters() const override
        {
            return 1;
        };
        std::vector<juce::String> getParameterNames() const override
        {
            return {"foo"};
        };
        BusLayouts getLayouts() const override
        {
            juce::AudioProcessor::BusesLayout layout;
            layout.inputBuses.add (juce::AudioChannelSet::mono());
            layout.outputBuses.add (juce::AudioChannelSet::mono());
            return {layout};
        }
    };

    SECTION ("default config has sensible defaults")
    {
        TestConfiguration empty;

        REQUIRE (empty.blockSize == 0);
        REQUIRE (empty.sampleRate == Approx (0.0));
        REQUIRE (empty.inputSignal == 0);
        juce::AudioProcessor::BusesLayout l = empty.layout;
        REQUIRE (l.inputBuses.isEmpty());
        REQUIRE (l.outputBuses.isEmpty());
        REQUIRE (empty.parameters.empty());
    }

    SECTION ("toJson works")
    {
        auto jsonString = nlohmann::json (config).dump();
        auto json = juce::JSON::fromString (jsonString);

        auto get = [&json] (const juce::String& property) { return json.getProperty (property, ""); };

        REQUIRE (json.isObject());
        REQUIRE (static_cast<int> (get ("blockSize")) == 1);
        REQUIRE (static_cast<double> (get ("sampleRate")) == 2.0);
        auto busLayout = get ("layout");
        auto parameters = get ("parameters");
        REQUIRE (static_cast<int> (get ("inputSignal")) == 1);

        REQUIRE (busLayout.isObject());
        REQUIRE (parameters.isArray());
        REQUIRE (parameters.size() == 1);

        auto inputs = busLayout.getProperty ("inputLayouts", "");
        auto outputs = busLayout.getProperty ("outputLayouts", "");

        REQUIRE (inputs.isArray());
        REQUIRE (inputs.size() == 2);
        REQUIRE (outputs.isArray());
        REQUIRE (outputs.size() == 1);

        REQUIRE (inputs[0].toString() == "Stereo");
        REQUIRE (inputs[1].toString() == "Stereo");
        REQUIRE (outputs[0].toString() == "Mono");

        auto param = parameters[0];
        REQUIRE (param.isObject());
        REQUIRE (param.getProperty ("name", "").toString() == "foo");
        REQUIRE (static_cast<float> (param.getProperty ("value", "")) == Approx (0.5f));
    }

    SECTION ("fromJson works")
    {
        auto json = nlohmann::json (config).dump();
        auto other = nlohmann::json::parse (json).get<TestConfiguration>();
        REQUIRE (config.sampleRate == Approx (other.sampleRate));
        REQUIRE (config.blockSize == other.blockSize);
        REQUIRE (config.inputSignal == other.inputSignal);
        REQUIRE (config.parameters.size() == other.parameters.size());

        for ( size_t i = 0; i < config.parameters.size(); ++i )
        {
            REQUIRE (config.parameters[i].name == other.parameters[i].name);
            REQUIRE (juce::approximatelyEqual (config.parameters[i].value, other.parameters[i].value));
        }

        juce::AudioProcessor::BusesLayout r = config.layout;
        juce::AudioProcessor::BusesLayout l = other.layout;
        REQUIRE (l.outputBuses.size() == r.outputBuses.size());
        REQUIRE (l.inputBuses.size() == r.inputBuses.size());
    }

    SECTION ("apply works")
    {
        TestPlugin plugin;
        config.applyTo (&plugin);
        REQUIRE (plugin.numSamples == 1);
        REQUIRE (plugin.rate == 2.0);
        REQUIRE (plugin.params["foo"] == 0.5f);
        juce::AudioProcessor::BusesLayout l;
        l.inputBuses.add (juce::AudioChannelSet::stereo());
        l.inputBuses.add (juce::AudioChannelSet::stereo());
        l.outputBuses.add (juce::AudioChannelSet::mono());
        config.layout = l;

        REQUIRE (plugin.busLayout.inputBuses[0] == juce::AudioChannelSet::stereo());
        REQUIRE (plugin.busLayout.inputBuses[1] == juce::AudioChannelSet::stereo());

        REQUIRE (plugin.busLayout.outputBuses.size() == 1);
        REQUIRE (plugin.busLayout.outputBuses[0] == juce::AudioChannelSet::mono());
    }

    SECTION ("generate works")
    {
        std::vector<double> rates = {1.0, 2.0};
        std::vector<size_t> sizes = {3, 4, 5};
        juce::AudioProcessor::BusesLayout l;
        l.inputBuses.add (juce::AudioChannelSet::mono());
        l.outputBuses.add (juce::AudioChannelSet::mono());

        Plugin::Info info;
        info.layouts = {l};
        ConfigGenerator::Info c;
        c.sampleRates = {1.0, 2.0};
        c.blockSizes = {3, 4, 5};
        c.signals = {0};

        auto configurations = ConfigGenerator::generateRandom (info, c, 1, 6, {});
        REQUIRE (configurations.size() == 6);

        REQUIRE (configurations[0].blockSize == 3);
        REQUIRE (configurations[0].sampleRate == 2.0);
        REQUIRE (configurations[0].layout == l);
        REQUIRE (configurations[0].inputSignal == 0);

        REQUIRE (configurations[1].blockSize == 5);
        REQUIRE (configurations[1].sampleRate == 1.0);
        REQUIRE (configurations[1].layout == l);
        REQUIRE (configurations[0].inputSignal == 0);

        REQUIRE (configurations[2].blockSize == 5);
        REQUIRE (configurations[2].sampleRate == 1.0);
        REQUIRE (configurations[2].layout == l);
        REQUIRE (configurations[0].inputSignal == 0);

        REQUIRE (configurations[3].blockSize == 3);
        REQUIRE (configurations[3].sampleRate == 2.0);
        REQUIRE (configurations[3].layout == l);
        REQUIRE (configurations[0].inputSignal == 0);

        REQUIRE (configurations[4].blockSize == 3);
        REQUIRE (configurations[4].sampleRate == 2.0);
        REQUIRE (configurations[4].layout == l);
        REQUIRE (configurations[0].inputSignal == 0);

        REQUIRE (configurations[5].blockSize == 4);
        REQUIRE (configurations[5].sampleRate == 2.0);
        REQUIRE (configurations[5].layout == l);
        REQUIRE (configurations[0].inputSignal == 0);
    }

    SECTION ("generateRandom works")
    {
        juce::AudioProcessor::BusesLayout l;
        l.inputBuses.add (juce::AudioChannelSet::mono());
        l.outputBuses.add (juce::AudioChannelSet::mono());
        Plugin::Info info;
        info.layouts = {l};
        info.parameters.push_back ({"foo", "foo", 1.0f, 1, 1, 1.0f, 0, true, true, false, true, false});

        auto configs1 = ConfigGenerator::generateRandom (info, 42, 6);

        std::vector<double> frequencies = {44100.0, 48000.0, 88200.0, 96000.0};
        std::vector<size_t> blockSizes = {32, 64, 128, 256, 512, 1024, 2048, 4096};

        for ( size_t i = 0; i < configs1.size(); ++i )
        {
            auto c = configs1[i];
            INFO (i);
            REQUIRE (c.parameters.size() == 1);
            juce::AudioProcessor::BusesLayout ll = c.layout;
            REQUIRE (ll.inputBuses.size() == 1);
            REQUIRE (ll.outputBuses.size() == 1);
            REQUIRE (ll.inputBuses[0] == juce::AudioChannelSet::mono());
            REQUIRE (ll.outputBuses[0] == juce::AudioChannelSet::mono());
        }
    }

    SECTION ("fromJson / toJson")
    {
        juce::String jsonString = "[{"
                                  "'blockSize': 1, "
                                  "'sampleRate': 2.0, "
                                  "'inputSignal': 1, "
                                  "'layout': {'inputLayouts' : ['Stereo', 'Stereo'], 'outputLayouts': ['Mono']}, "
                                  "'parameters' : [{'name': 'foo', 'value': 0.5}]"
                                  "}]";

        auto configs = nlohmann::json::parse (jsonString.replace ("'", "\"").toStdString()).get<TestConfigurations>();

        REQUIRE (configs.size() == 1);
        REQUIRE (configs[0].blockSize == 1);
        REQUIRE (configs[0].sampleRate == 2.0);
        REQUIRE (configs[0].inputSignal == 1);
        REQUIRE (configs[0].layout == config.layout);
        REQUIRE (configs[0].parameters.size() == 1);
        REQUIRE (configs[0].parameters[0].name == "foo");
        REQUIRE (configs[0].parameters[0].value == 0.5f);
    }
}
