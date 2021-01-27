#pragma once

#include "TestInfo.h"

#include <buffer/Buffer.h>
#include <plugin/BusLayout.h>
#include <plugin/Plugin.h>

/* A class to hold the configuration used for audio testing */
struct TestConfiguration
{
    /* Apply the configuration to the plugin */
    void applyTo (Plugin* plugin) const;

    /* Index of the input signal that should be used */
    size_t inputSignal = 0;

    /* BlockSize used for audio processing */
    int blockSize = 0;

    /* Sample-Rate used for audio processing */
    double sampleRate = 0.0;

    /* BusLayout used for the audio processing */
    BusLayout layout;

    /* Struct holding the value that a parameter should hold */
    struct Parameter
    {
        std::string name;
        float value;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE (TestConfiguration::Parameter, name, value)
    };

    std::vector<Parameter> parameters;

    /* Sort the configurations by bus layout */
    static std::vector<TestConfiguration> sort (const std::vector<TestConfiguration>& input);

    /* Compare the test configuration to another config */
    std::vector<TestInfo> compareTo (const TestConfiguration& other) const;

    /* Compare a vector of configs to another */
    static std::vector<TestInfos> compare (const std::vector<TestConfiguration>& lhs,
                                           const std::vector<TestConfiguration>& rhs);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE (TestConfiguration, inputSignal, blockSize, sampleRate, layout, parameters)
};

using TestConfigurations = std::vector<TestConfiguration>;
