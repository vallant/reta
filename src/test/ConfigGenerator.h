#pragma once
#include "TestConfiguration.h"

#include <command/GenerateCommand.h>
#include <plugin/Plugin.h>
#include <vector>

/* A class to specify which parameters are used in the test and generate random test configurations */
class ConfigGenerator
{
public:
    static std::vector<double> sampleRates();
    static std::vector<size_t> blockSizes();
    static std::vector<size_t> signals();
    static std::vector<float> values();

    struct Info
    {
        std::vector<double> sampleRates;
        std::vector<size_t> blockSizes;
        std::vector<size_t> signals;
        std::vector<float> values;
    };

    static TestConfigurations generateRandom (const Plugin::Info& info, const Info& config, int seed, int numTests,
                                              const std::vector<std::string>& exclude);

    static TestConfigurations generateRandom (const Plugin::Info& info, int seed, int numTests);
    static TestConfigurations generateRandom (const Plugin::Info& info, const GenerateCommand::Parameters& parameters);
};
