#include "ConfigGenerator.h"

#include <test/InputSignals.h>
#include <util/Random.h>
#include <util/Util.h>

std::vector<double> ConfigGenerator::sampleRates()
{
    return {44100.0, 48000.0, 88200.0, 96000.0};
}
std::vector<size_t> ConfigGenerator::blockSizes()
{
    return {32, 64, 128, 256, 512, 1024, 2048, 4096};
}
std::vector<size_t> ConfigGenerator::signals()
{
    std::vector<size_t> s (InputSignals::size());
    for ( size_t i = 0; i < s.size(); ++i )
        s[i] = i;

    return s;
}
std::vector<float> ConfigGenerator::values()
{
    return {-1.0f, 0.0f, 0.25f, 0.5f, 0.75f, 1.0f};
}
TestConfigurations ConfigGenerator::generateRandom (const Plugin::Info& info,
                                                    const GenerateCommand::Parameters& parameters)
{
    return generateRandom (info, {sampleRates(), blockSizes(), signals(), values()}, parameters.seed,
                           parameters.numTests, parameters.exclude);
}

TestConfigurations ConfigGenerator::generateRandom (const Plugin::Info& info, int seed, int numTests)
{
    return generateRandom (info, {sampleRates(), blockSizes(), signals(), values()}, seed, numTests, {});
}

TestConfigurations ConfigGenerator::generateRandom (const Plugin::Info& info, const ConfigGenerator::Info& config,
                                                    int seed, int numTests, const std::vector<std::string>& exclude)
{
    TestConfigurations configs (numTests);
    Random random (static_cast<size_t> (seed));

    for ( int i = 0; i < numTests; ++i )
    {
        configs[i].blockSize = static_cast<int> (random.nextFrom (config.blockSizes));
        configs[i].inputSignal = random.nextFrom (config.signals);
        configs[i].sampleRate = random.nextFrom (config.sampleRates);
        configs[i].layout = info.layouts[random.next (static_cast<size_t> (0), info.layouts.size() - 1)];

        configs[i].parameters.reserve (info.parameters.size());
        for ( auto& param : info.parameters )
        {
            if ( util::matchedByAnyWildcard (exclude, param.name) )
                continue;

            float value;
            if ( param.discrete )
            {
                auto valuesWithNegative = param.values;
                valuesWithNegative.push_back (-1.0f);
                value = random.nextFrom (valuesWithNegative);
            }
            else
            {
                value = random.next (-0.1f, 1.0f);
            }

            configs[i].parameters.push_back ({param.name, value});
        }
    }

    return TestConfiguration::sort (configs);
}
