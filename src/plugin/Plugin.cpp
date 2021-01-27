#include "Plugin.h"

std::vector<TestInfo> Plugin::ParameterInfo::compareTo (const Plugin::ParameterInfo& other) const
{
    std::vector<TestInfo> tests;

    tests.push_back (RETA_CREATE_TEST_STR (name));
    tests.push_back (RETA_CREATE_TEST_STR (label));
    tests.push_back (RETA_CREATE_TEST (numSteps));
    tests.push_back (RETA_CREATE_TEST (category));
    tests.push_back (RETA_CREATE_TEST_FLOAT (defaultValue));
    tests.push_back (RETA_CREATE_TEST (index));
    tests.push_back (RETA_CREATE_TEST (automatable));
    tests.push_back (RETA_CREATE_TEST (discrete));
    tests.push_back (RETA_CREATE_TEST (meta));
    tests.push_back (RETA_CREATE_TEST (inverted));
    tests.push_back (RETA_CREATE_TEST (boolean));

    bool matches = values.size() == other.values.size();
    for ( size_t i = 0; i < values.size() && i < other.values.size(); ++i )
        matches &= juce::approximatelyEqual (values[i], other.values[i]);
    tests.push_back ({"Possible values match", "", "", matches});
    return tests;
}
std::vector<TestInfos> Plugin::ParameterInfo::compare (const std::vector<ParameterInfo>& lhs,
                                                       const std::vector<ParameterInfo>& rhs)
{
    std::vector<TestInfos> infos;

    for ( size_t i = 0; i < lhs.size() && i < rhs.size(); ++i )
        infos.push_back (lhs[i].compareTo (rhs[i]));

    return infos;
}
std::vector<TestInfo> Plugin::Info::compareTo (const Plugin::Info& other)
{
    std::vector<TestInfo> tests;

    tests.push_back (RETA_CREATE_TEST_STR (name));
    tests.push_back (RETA_CREATE_TEST (inputBuses));
    tests.push_back (RETA_CREATE_TEST (outputBuses));
    tests.push_back (RETA_CREATE_TEST (acceptsMidi));
    tests.push_back (RETA_CREATE_TEST (producesMidi));
    tests.push_back (RETA_CREATE_TEST (latency));
    tests.push_back (RETA_CREATE_TEST_FLOAT (tail));
    tests.push_back (RETA_CREATE_TEST_STR (precision));
    tests.push_back (RETA_CREATE_TEST (hasEditor));

    return tests;
}
