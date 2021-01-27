#include "plugin/BusLayout.h"

#include "../util/PluginStub.h"

#include <catch2/catch.hpp>

struct Stub : PluginStub
{
    bool isLayoutSupported (const juce::AudioProcessor::BusesLayout& layout) const override
    {
        return layout.inputBuses.size() == 1 && layout.outputBuses.size() == 1
               && layout.inputBuses[0] == juce::AudioChannelSet::mono()
               && layout.outputBuses[0] == juce::AudioChannelSet::mono();
    }
    std::pair<size_t, size_t> getNumBuses() const override
    {
        return {1, 1};
    }
};

TEST_CASE ("possible bus layouts are calculated correctly", "[unit][plugin][channels][buslayout]")
{
    SECTION ("for 2 layouts and 1 bus")
    {
        auto layouts = BusLayout::layoutsPerBus ({juce::AudioChannelSet::stereo().getDescription().toStdString(),
                                                  juce::AudioChannelSet::mono().getDescription().toStdString()},
                                                 1);
        REQUIRE (layouts.size() == 2);
        REQUIRE (layouts[0].size() == 1);
        REQUIRE (layouts[0][0] == juce::AudioChannelSet::stereo().getDescription().toStdString());
        REQUIRE (layouts[1].size() == 1);
        REQUIRE (layouts[1][0] == juce::AudioChannelSet::mono().getDescription().toStdString());
    }

    SECTION ("for 3 layouts and 1 bus")
    {
        auto layouts = BusLayout::layoutsPerBus ({juce::AudioChannelSet::stereo().getDescription().toStdString(),
                                                  juce::AudioChannelSet::mono().getDescription().toStdString(),
                                                  juce::AudioChannelSet::disabled().getDescription().toStdString()},
                                                 1);
        REQUIRE (layouts.size() == 3);
        REQUIRE (layouts[0].size() == 1);
        REQUIRE (layouts[0][0] == juce::AudioChannelSet::stereo().getDescription().toStdString());
        REQUIRE (layouts[1].size() == 1);
        REQUIRE (layouts[1][0] == juce::AudioChannelSet::mono().getDescription().toStdString());
        REQUIRE (layouts[2].size() == 1);
        REQUIRE (layouts[2][0] == juce::AudioChannelSet::disabled().getDescription().toStdString());
    }

    SECTION ("for 1 layout and 2 bus")
    {
        auto layouts = BusLayout::layoutsPerBus ({juce::AudioChannelSet::stereo().getDescription().toStdString()}, 2);
        REQUIRE (layouts.size() == 1);
        REQUIRE (layouts[0].size() == 2);
        REQUIRE (layouts[0][0] == juce::AudioChannelSet::stereo().getDescription().toStdString());
        REQUIRE (layouts[0][1] == juce::AudioChannelSet::stereo().getDescription().toStdString());
    }

    SECTION ("for 2 layout and 2 bus")
    {
        auto layouts = BusLayout::layoutsPerBus ({juce::AudioChannelSet::stereo().getDescription().toStdString(),
                                                  juce::AudioChannelSet::mono().getDescription().toStdString()},
                                                 2);
        REQUIRE (layouts.size() == 4);
        for ( auto& layout : layouts )
        {
            REQUIRE (layout.size() == 2);
        }

        REQUIRE (layouts[0][0] == juce::AudioChannelSet::stereo().getDescription().toStdString());
        REQUIRE (layouts[0][1] == juce::AudioChannelSet::stereo().getDescription().toStdString());

        REQUIRE (layouts[1][0] == juce::AudioChannelSet::stereo().getDescription().toStdString());
        REQUIRE (layouts[1][1] == juce::AudioChannelSet::mono().getDescription().toStdString());

        REQUIRE (layouts[2][0] == juce::AudioChannelSet::mono().getDescription().toStdString());
        REQUIRE (layouts[2][1] == juce::AudioChannelSet::stereo().getDescription().toStdString());

        REQUIRE (layouts[3][0] == juce::AudioChannelSet::mono().getDescription().toStdString());
        REQUIRE (layouts[3][1] == juce::AudioChannelSet::mono().getDescription().toStdString());
    }
}

TEST_CASE ("BusLayout reports correct channel count", "[unit][plugin][channels]")
{
    SECTION ("2 stereo + 3 mono")
    {
        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add (juce::AudioChannelSet::stereo());
        layout.inputBuses.add (juce::AudioChannelSet::stereo());

        layout.outputBuses.add (juce::AudioChannelSet::mono());
        layout.outputBuses.add (juce::AudioChannelSet::mono());
        layout.outputBuses.add (juce::AudioChannelSet::mono());

        REQUIRE (BusLayout (layout).getNumChannels() == 4);
    }

    SECTION ("1 stereo + 3 mono")
    {
        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add (juce::AudioChannelSet::stereo());

        layout.outputBuses.add (juce::AudioChannelSet::mono());
        layout.outputBuses.add (juce::AudioChannelSet::mono());
        layout.outputBuses.add (juce::AudioChannelSet::mono());

        REQUIRE (BusLayout (layout).getNumChannels() == 3);
    }

    SECTION ("1 stereo + 1 stereo")
    {
        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add (juce::AudioChannelSet::stereo());
        layout.outputBuses.add (juce::AudioChannelSet::stereo());

        REQUIRE (BusLayout (layout).getNumChannels() == 2);
    }

    SECTION ("toJson works")
    {
        juce::AudioProcessor::BusesLayout layout;
        layout.inputBuses.add (juce::AudioChannelSet::stereo());
        layout.inputBuses.add (juce::AudioChannelSet::mono());
        layout.outputBuses.add (juce::AudioChannelSet::stereo());
        layout.outputBuses.add (juce::AudioChannelSet::mono());

        auto jsonString = nlohmann::json (BusLayout (layout)).dump();
        auto json = juce::JSON::fromString (jsonString);
        REQUIRE (json.isObject());
        auto inputs = json.getProperty ("inputLayouts", "");
        auto outputs = json.getProperty ("outputLayouts", "");

        REQUIRE (inputs.size() == 2);
        REQUIRE (outputs.size() == 2);

        REQUIRE (inputs[0].toString() == "Stereo");
        REQUIRE (inputs[1].toString() == "Mono");

        REQUIRE (outputs[0].toString() == "Stereo");
        REQUIRE (outputs[1].toString() == "Mono");
    }

    SECTION ("fromJson works")
    {
        auto json = "{\"inputLayouts\" : [\"Stereo\", \"Mono\"], \"outputLayouts\" : [\"Stereo\", \"Mono\"]}";
        auto l = nlohmann::json::parse (json).get<BusLayout>();
        juce::AudioProcessor::BusesLayout layout = l;
        REQUIRE (layout.inputBuses.size() == 2);
        REQUIRE (layout.outputBuses.size() == 2);
        REQUIRE (layout.inputBuses[0] == juce::AudioChannelSet::stereo());
        REQUIRE (layout.inputBuses[1] == juce::AudioChannelSet::mono());
        REQUIRE (layout.outputBuses[0] == juce::AudioChannelSet::stereo());
        REQUIRE (layout.outputBuses[1] == juce::AudioChannelSet::mono());
    }

    SECTION ("retrieval of buslayouts from plugin works")
    {
        juce::AudioProcessor::BusesLayout reference;
        reference.inputBuses.add (juce::AudioChannelSet::mono());
        reference.outputBuses.add (juce::AudioChannelSet::mono());

        Stub stub;

        auto actual = BusLayout::getLayouts (&stub);

        REQUIRE (actual.size() == 1);
        REQUIRE (actual[0] == reference);
    }

    SECTION ("toJson works")
    {
        Stub stub;
        auto json = nlohmann::json (BusLayout::getLayouts (&stub)).dump();
        auto reference = "[{\"inputLayouts\" : [\"Mono\"], \"outputLayouts\" : [\"Mono\"]}]";
        REQUIRE (juce::JSON::toString (juce::JSON::fromString (json))
                 == juce::JSON::toString (juce::JSON::fromString (reference)));
    }

    SECTION ("fromJsonAll works")
    {
        juce::AudioProcessor::BusesLayout reference;
        reference.inputBuses.add (juce::AudioChannelSet::mono());
        reference.outputBuses.add (juce::AudioChannelSet::mono());

        auto input = "[{\"inputLayouts\" : [\"Mono\"], \"outputLayouts\" : [\"Mono\"]}]";
        auto actual = nlohmann::json::parse (input).get<std::vector<BusLayout>>();
        REQUIRE (actual.size() == 1);
        REQUIRE (actual[0] == reference);
    }

    SECTION ("Constructors")
    {
        juce::AudioProcessor::BusesLayout reference;
        reference.inputBuses.add (juce::AudioChannelSet::mono());
        reference.outputBuses.add (juce::AudioChannelSet::stereo());

        REQUIRE (BusLayout (juce::AudioChannelSet::mono(), juce::AudioChannelSet::stereo()) == reference);

        reference.inputBuses.add (juce::AudioChannelSet::mono());
        reference.outputBuses.add (juce::AudioChannelSet::stereo());

        REQUIRE (BusLayout ({juce::AudioChannelSet::mono(), juce::AudioChannelSet::mono()},
                            {juce::AudioChannelSet::stereo(), juce::AudioChannelSet::stereo()})
                 == reference);
    }
}