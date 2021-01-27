#include "../util/FileSystemStub.h"

#include <catch2/catch.hpp>
#include <future>
#include <plugin/PluginImpl.h>

juce::String getPluginPath()
{
    // plugin is a sibling file to the executable
    auto folder =
        juce::File::getSpecialLocation (juce::File::SpecialLocationType::currentApplicationFile).getFullPathName();
    return folder.upToLastOccurrenceOf (juce::File::getSeparatorString(), false, true) + "/reta.vst3";
}

TEST_CASE ("PluginImpl constructor throws if file doesn't exist", "[unit][plugin]")
{
    auto fs = std::make_unique<FileSystemStub>();
    fs->fileExists = false;

    REQUIRE_THROWS (PluginImpl ("foo", std::move (fs)));
}

TEST_CASE ("PluginImpl constructor throws if file exists but isn't a valid plugin", "[integration][plugin]")
{
    juce::ScopedJuceInitialiser_GUI init;
    REQUIRE_THROWS (PluginImpl (__FILE__));
}

TEST_CASE ("PluginImpl constructor doesn't throw for valid plugin", "[integration][plugin]")
{
    std::cout << getPluginPath() << std::endl;
    REQUIRE_NOTHROW (PluginImpl (getPluginPath()));
}

TEST_CASE ("PluginImpl returns correct specs about test plugin", "[integration][plugin]")
{
    PluginImpl plugin (getPluginPath());

    REQUIRE (plugin.getNumParameters() == 5);  // 2 params + bypass
}

TEST_CASE ("PluginImpl reports correct parameters", "[integration][plugin]")
{
    PluginImpl plugin (getPluginPath());

    auto names = plugin.getParameterNames();

    REQUIRE (names[0] == "foo");
    REQUIRE (names[1] == "bar");
    REQUIRE (names[2] == "even");
    REQUIRE (names[3] == "odd");
    REQUIRE (names[4] == "Bypass");
}

TEST_CASE ("SetParameter works", "[integration][plugin]")
{
    PluginImpl plugin (getPluginPath());

    plugin.setParameter ("foo", 0.5f);
    plugin.setParameter ("bar", 0.5f);
    auto info = plugin.getInfo();

    INFO (info.parameters[0].name);
    INFO (info.parameters[1].name);
    REQUIRE (info.parameters[0].value == 0.5f);
    REQUIRE (info.parameters[1].value == 0.5f);
}

TEST_CASE ("Num buses and layout reported correctly", "[integration][plugin]")
{
    PluginImpl plugin (getPluginPath());
    REQUIRE (plugin.getNumBuses() == std::pair<size_t, size_t> (1, 1));

    juce::AudioProcessor::BusesLayout layout1;
    layout1.inputBuses.add (juce::AudioChannelSet::stereo());
    layout1.outputBuses.add (juce::AudioChannelSet::stereo());

    REQUIRE (plugin.isLayoutSupported (layout1));

    juce::AudioProcessor::BusesLayout layout2;
    layout2.inputBuses.add (juce::AudioChannelSet::mono());
    layout2.outputBuses.add (juce::AudioChannelSet::mono());

    REQUIRE (plugin.isLayoutSupported (layout2));

    layout2.inputBuses.add (juce::AudioChannelSet::mono());
    layout2.outputBuses.add (juce::AudioChannelSet::mono());

    REQUIRE_FALSE (plugin.isLayoutSupported (layout2));
}

TEST_CASE ("process works properly", "[integration][plugin]")
{
    PluginImpl plugin (getPluginPath());
    plugin.setParameter ("even", 0.1f);
    plugin.setParameter ("odd", 0.9f);

    juce::AudioProcessor::BusesLayout stereo;
    stereo.inputBuses.add (juce::AudioChannelSet::stereo());
    stereo.outputBuses.add (juce::AudioChannelSet::stereo());
    plugin.prepare (44100.0, 1024, stereo);

    juce::AudioSampleBuffer buffer;
    buffer.setSize (2, 1024);
    plugin.process (buffer);

    for ( int channel = 0; channel < buffer.getNumChannels(); ++channel )
    {
        for ( int sample = 0; sample < buffer.getNumSamples(); ++sample )
        {
            std::ostringstream stream;
            stream << "channel: " << channel << " sample: " << sample;
            INFO (stream.str());
            auto target = sample % 2 == 0 ? 0.1f : 0.9f;
            REQUIRE (buffer.getSample (channel, sample) == Approx (target));
        }
    }
}

TEST_CASE ("plugin correctly produces json", "[integration][plugin]")
{
    PluginImpl plugin (getPluginPath());

    auto jsonString = nlohmann::json (plugin.getInfo()).dump (2);
    INFO (jsonString);
    auto json = juce::JSON::fromString (jsonString);
    REQUIRE (json.isObject());
    REQUIRE (json.getProperty ("name", "") == "reta");
    REQUIRE (json.getProperty ("inputBuses", "") == "1");
    REQUIRE (json.getProperty ("outputBuses", "") == "1");
    REQUIRE (json.getProperty ("tail", "") == "0.0");
    REQUIRE (json.getProperty ("latency", "") == "0");
    REQUIRE (static_cast<bool> (json.getProperty ("hasEditor", "")));
    REQUIRE_FALSE (static_cast<bool> (json.getProperty ("producesMidi", "")));
    REQUIRE_FALSE (static_cast<bool> (json.getProperty ("isMidiEffect", "")));
    REQUIRE_FALSE (static_cast<bool> (json.getProperty ("acceptsMidi", "")));
    REQUIRE (json.getProperty ("precision", "") == "single");

    auto parameters = json.getProperty ("parameters", "");
    REQUIRE (parameters.isArray());
    REQUIRE (parameters.size() == 5);

    auto parameterArray = parameters.getArray();

    // just check if they are valid json-objects
    // the actual structure is checked by other tests
    for ( auto& parameter : *parameterArray )
    {
        REQUIRE (parameter.isObject());
    }

    auto layouts = json.getProperty ("layouts", "");
    REQUIRE (layouts.isArray());
    REQUIRE (layouts.size() == 2);

    auto monoLayout = layouts[0];
    auto stereoLayout = layouts[1];
    REQUIRE (monoLayout.isObject());
    REQUIRE (stereoLayout.isObject());

    auto monoInputChannels = monoLayout.getProperty ("inputLayouts", "");
    auto monoOutputChannels = monoLayout.getProperty ("outputLayouts", "");
    REQUIRE (monoInputChannels.isArray());
    REQUIRE (monoOutputChannels.isArray());

    REQUIRE (monoInputChannels.size() == 1);
    REQUIRE (monoOutputChannels.size() == 1);

    REQUIRE (monoInputChannels[0] == "Mono");
    REQUIRE (monoOutputChannels[0] == "Mono");

    auto stereoInputChannels = stereoLayout.getProperty ("inputLayouts", "");
    auto stereoOutputChannels = stereoLayout.getProperty ("outputLayouts", "");
    REQUIRE (stereoInputChannels.isArray());
    REQUIRE (stereoOutputChannels.isArray());

    REQUIRE (stereoInputChannels.size() == 1);
    REQUIRE (stereoOutputChannels.size() == 1);

    REQUIRE (stereoInputChannels[0] == "Stereo");
    REQUIRE (stereoOutputChannels[0] == "Stereo");
}

TEST_CASE ("Supported bus layouts are reported correctly", "[integration][plugin][channels]")
{
    PluginImpl plugin (getPluginPath());
    auto layouts = plugin.getLayouts();

    REQUIRE (layouts.size() == 2);
    juce::AudioProcessor::BusesLayout layout = layouts[0];
    REQUIRE (layout.inputBuses.size() == 1);
    REQUIRE (layout.outputBuses.size() == 1);
    REQUIRE (layout.inputBuses[0] == juce::AudioChannelSet::mono());
    REQUIRE (layout.outputBuses[0] == juce::AudioChannelSet::mono());

    layout = layouts[1];
    REQUIRE (layout.inputBuses.size() == 1);
    REQUIRE (layout.outputBuses.size() == 1);
    REQUIRE (layout.inputBuses[0] == juce::AudioChannelSet::stereo());
    REQUIRE (layout.outputBuses[0] == juce::AudioChannelSet::stereo());
}

TEST_CASE ("fromjson works")
{
    PluginImpl plugin (getPluginPath());
    auto jsonString = nlohmann::json (plugin.getInfo()).dump();
    auto json = nlohmann::json::parse (jsonString);
    auto info = json.get<Plugin::Info>();

    REQUIRE (info.name == "reta");
    REQUIRE (info.inputBuses == 1);
    REQUIRE (info.outputBuses == 1);
    REQUIRE (info.tail == 0.0);
    REQUIRE (info.latency == 0);
    REQUIRE (info.hasEditor);
    REQUIRE_FALSE (info.producesMidi);
    REQUIRE_FALSE (info.isMidiEffect);
    REQUIRE_FALSE (info.acceptsMidi);
    REQUIRE (info.precision == "single");
}
