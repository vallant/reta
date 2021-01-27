#pragma once
#include "test/TestConfiguration.h"

/*  A class that translates between reta and ACTS test configuration representation.
    See the unit test for more information about the acts format.
*/
class ActsConfigAdapter
{
public:
    /*  Convert the acts-output to a set of test configurations */
    static std::vector<TestConfiguration> configFromActs (Plugin* plugin, const juce::String& actsOutput);
    static std::vector<TestConfiguration> configFromActs (const std::vector<Plugin::ParameterInfo>& parameters,
                                                          const std::vector<BusLayout>& layouts,
                                                          const juce::String& actsOutput);

    /*  Convert the plugin information into the input format of ACTS */
    static juce::String inputToActs (Plugin* plugin);
    static juce::String inputToActs (const juce::String& name, const std::vector<Plugin::ParameterInfo>& parameters,
                                     const std::vector<BusLayout>& layouts);

    /*  Extract a single test configuration from the provided line of acts-output */
    static TestConfiguration parseActsLine (const juce::String& line, const BusLayouts& layouts,
                                            const std::vector<Plugin::ParameterInfo>& parameters);

    /*  Split the interval [0,1] into numSteps */
    static std::vector<float> createSteps (size_t numSteps);

    /*  Parse the ACTS header line to extract the parameter names referenced by the file */
    static std::vector<TestConfiguration::Parameter>
        parseParameters (const std::string& input, const std::vector<Plugin::ParameterInfo>& parameters);
    /*  Convert a parameter-info into a ACTS-compatible input-format */
    static juce::String createParamLine (const Plugin::ParameterInfo& param);

    /* Parse the ACTS output-file and split all relevant lines into a string array */
    static juce::StringArray splitIntoRelevantLines (const juce::String& originalInput);

    /* Helper template to join a vector of string-convertible types into a single string using a comma */
    template <class T>
    static juce::String vecToString (const std::vector<T>& vec)
    {
        juce::StringArray array;
        for ( const auto& element : vec )
            array.add (juce::String (element));
        return array.joinIntoString (", ");
    }
};
