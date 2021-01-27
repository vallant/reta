#include "DitherAdapter.h"

#include <dither.h>
#include <test/ConfigGenerator.h>
#include <util/Util.h>

TestConfigurations DitherAdapter::generate (Plugin* plugin, const GenerateCommand::Parameters& parameters)
{
    return generate (plugin->getInfo(), parameters);
}

TestConfigurations DitherAdapter::generate (const Plugin::Info& info, const GenerateCommand::Parameters& parameters)
{
    auto blockSizes = ConfigGenerator::blockSizes();
    auto sampleRates = ConfigGenerator::sampleRates();
    auto signals = ConfigGenerator::signals();
    auto values = ConfigGenerator::values();

    auto blockSizeIndices = util::vecToIndices (blockSizes);
    auto sampleRateIndices = util::vecToIndices (sampleRates);
    auto signalIndices = util::vecToIndices (signals);
    auto layoutIndices = util::vecToIndices (info.layouts);
    auto valueIndices = util::vecToIndices (values);

    auto handle = dither_ipog_new (parameters.interactions);

    int index = 0;
    dither_ipog_add_parameter_int (handle, index++, blockSizeIndices.data(),
                                   static_cast<int> (blockSizeIndices.size()));
    dither_ipog_add_parameter_int (handle, index++, sampleRateIndices.data(),
                                   static_cast<int> (sampleRateIndices.size()));
    dither_ipog_add_parameter_int (handle, index++, signalIndices.data(), static_cast<int> (signalIndices.size()));
    dither_ipog_add_parameter_int (handle, index++, layoutIndices.data(), static_cast<int> (layoutIndices.size()));

    std::vector<Plugin::ParameterInfo> includedParameters;

    for ( auto& param : info.parameters )
    {
        if ( ! util::matchedByAnyWildcard (parameters.exclude, param.name) )
            includedParameters.push_back (param);
    }

    for ( auto& param : includedParameters )
    {
        auto steps = param.discrete ? util::vecToIndices (param.values) : valueIndices;
        dither_ipog_add_parameter_int (handle, index++, steps.data(), static_cast<int> (steps.size()));
    }

    dither_ipog_run (handle);

    auto numParameters = index;
    auto numTests = dither_ipog_size (handle);

    std::vector<int> result (numParameters * numTests);
    dither_ipog_fill (handle, result.data());
    TestConfigurations configs (numTests);

    for ( int i = 0; i < numTests; ++i )
    {
        auto offset = i * numParameters;
        configs[i].blockSize = static_cast<int> (blockSizes[result[offset + 0]]);
        configs[i].sampleRate = sampleRates[result[offset + 1]];
        configs[i].inputSignal = signals[result[offset + 2]];
        configs[i].layout = info.layouts[result[offset + 3]];

        for ( size_t parameterIndex = 0; parameterIndex < includedParameters.size(); ++parameterIndex )
        {
            auto& param = includedParameters[parameterIndex];
            auto viableValues = param.discrete ? param.values : ConfigGenerator::values();
            auto value = viableValues[result[offset + 4 + parameterIndex]];
            configs[i].parameters.push_back ({param.name, value});
        }
    }

    return configs;
}