#include "ActsConfigAdapter.h"

#include <plugin/BusLayout.h>
#include <test/ConfigGenerator.h>

std::vector<TestConfiguration> ActsConfigAdapter::configFromActs (Plugin* plugin, const juce::String& originalInput)
{
    auto info = plugin->getInfo();
    return configFromActs (info.parameters, info.layouts, originalInput);
}

TestConfiguration ActsConfigAdapter::parseActsLine (const juce::String& line, const BusLayouts& layouts,
                                                    const std::vector<Plugin::ParameterInfo>& names)
{
    TestConfiguration config;
    std::stringstream stream (line.toStdString());
    std::string params;
    char comma;
    int busLayoutIndex;

    stream >> config.blockSize >> comma;
    stream >> config.sampleRate >> comma;
    stream >> busLayoutIndex >> comma;
    stream >> config.inputSignal >> comma;
    stream >> params;

    config.parameters = parseParameters (params, names);
    config.layout = layouts[busLayoutIndex];

    if ( stream.bad() )
        throw std::exception();

    return config;
}

std::vector<TestConfiguration::Parameter>
    ActsConfigAdapter::parseParameters (const std::string& input, const std::vector<Plugin::ParameterInfo>& parameters)
{
    std::vector<TestConfiguration::Parameter> testParameters;
    juce::StringArray paramArray;
    paramArray.addTokens (input, ",", "");
    testParameters.reserve (paramArray.size());
    for ( int i = 0; i < paramArray.size(); ++i )
    {
        auto val = std::stof (paramArray[i].toStdString());
        auto realName = parameters[i].name;
        testParameters.push_back ({realName, val});
    }
    return testParameters;
}

std::vector<TestConfiguration> ActsConfigAdapter::configFromActs (const std::vector<Plugin::ParameterInfo>& parameters,
                                                                  const std::vector<BusLayout>& layouts,
                                                                  const juce::String& originalInput)
{
    auto lines = splitIntoRelevantLines (originalInput);

    std::vector<TestConfiguration> configs;
    configs.reserve (lines.size());

    for ( auto& line : lines )
        configs.push_back (parseActsLine (line, layouts, parameters));

    return TestConfiguration::sort (configs);
}

juce::String ActsConfigAdapter::createParamLine (const Plugin::ParameterInfo& param)
{
    auto steps = param.discrete ? param.values : ConfigGenerator::values();

    return juce::String (param.name).removeCharacters (" ") + " (enum): " + vecToString (steps);
}

juce::String ActsConfigAdapter::inputToActs (Plugin* plugin)
{
    auto info = plugin->getInfo();
    return inputToActs (info.name, info.parameters, info.layouts);
}

std::vector<float> ActsConfigAdapter::createSteps (size_t numSteps)
{
    float inc = 1.0f / static_cast<float> (numSteps);
    std::vector<float> vals;
    vals.push_back (-1.0f);
    float i = 0.0f;
    while ( i < 1.0f )
    {
        vals.push_back (i);
        i += inc;
    }
    if ( ! juce::approximatelyEqual (*vals.rbegin(), 1.0f) )
        vals.push_back (1.0f);
    return vals;
}

juce::String ActsConfigAdapter::inputToActs (const juce::String& name, const std::vector<Plugin::ParameterInfo>& params,
                                             const std::vector<BusLayout>& layouts)
{
    std::vector<size_t> layoutIndices (layouts.size());
    for ( size_t i = 0; i < layoutIndices.size(); ++i )
        layoutIndices[i] = i;

    juce::StringArray lines;
    lines.add ("[System]");
    lines.add ("Name: " + name);

    lines.add ("[Parameter]");
    lines.add ("BlockSize (enum): " + vecToString (ConfigGenerator::blockSizes()));
    lines.add ("SampleRate (enum): " + vecToString (ConfigGenerator::sampleRates()));
    lines.add ("BusLayout (enum): " + vecToString (layoutIndices));
    lines.add ("Signal (enum): " + vecToString (ConfigGenerator::signals()));

    for ( auto& param : params )
        lines.add (createParamLine (param));

    return lines.joinIntoString ("\n");
}

juce::StringArray ActsConfigAdapter::splitIntoRelevantLines (const juce::String& originalInput)
{
    auto delimiter = "# ****************************************************";
    auto input = originalInput.fromFirstOccurrenceOf (delimiter, false, true)
                     .removeCharacters ("\"")
                     .removeCharacters ("\r")
                     .trim();

    juce::StringArray lines;
    lines.addTokens (input, "\n");
    lines.removeEmptyStrings (true);
    lines.remove (0);
    return lines;
}
