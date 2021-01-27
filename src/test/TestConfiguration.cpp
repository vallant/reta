#include "TestConfiguration.h"

void TestConfiguration::applyTo (Plugin* plugin) const
{
    plugin->prepare (sampleRate, static_cast<size_t> (blockSize), layout);
    for ( const auto& pair : parameters )
        plugin->setParameter (pair.name, pair.value);
}

std::vector<TestConfiguration> TestConfiguration::sort (const std::vector<TestConfiguration>& input)
{
    auto i = input;
    auto sort = [] (const TestConfiguration& lhs, const TestConfiguration& rhs) { return lhs.layout < rhs.layout; };
    std::sort (i.begin(), i.end(), sort);
    return i;
}
std::vector<TestInfos> TestConfiguration::compare (const std::vector<TestConfiguration>& lhs,
                                                   const std::vector<TestConfiguration>& rhs)
{
    std::vector<TestInfos> infos;
    for ( size_t i = 0; i < lhs.size() && i < rhs.size(); ++i )
        infos.push_back (lhs[i].compareTo (rhs[i]));
    return infos;
}

std::vector<TestInfo> TestConfiguration::compareTo (const TestConfiguration& other) const
{
    std::vector<TestInfo> infos;
    infos.push_back (RETA_CREATE_TEST (inputSignal));
    infos.push_back (RETA_CREATE_TEST (blockSize));
    infos.push_back (RETA_CREATE_TEST_FLOAT (sampleRate));
    infos.push_back (RETA_CREATE_TEST (inputSignal));
    infos.push_back (layout.compareTo (other.layout));
    infos.push_back ({"number of values match", std::to_string (parameters.size()),
                      std::to_string (other.parameters.size()), parameters.size() == other.parameters.size()});
    bool matching = true;
    for ( size_t i = 0; i < parameters.size() && i < other.parameters.size(); ++i )
    {
        matching &= juce::approximatelyEqual (parameters[i].value, other.parameters[i].value);
        matching &= parameters[i].name == other.parameters[i].name;
    }

    infos.push_back ({"parameters match", "true", matching ? "true" : "false", matching});
    return infos;
}
