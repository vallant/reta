#pragma once
#include <CLI/App.hpp>
#include <command/Command.h>
#include <test/TestCase.h>
#include <test/TestInfo.h>

/*  Comparison of two test results */
class CompareCommand : public Command
{
public:
    struct Parameters
    {
        std::string reference;
        std::string actual;
        std::string output;
        float delta = 0.001f;
        std::string strategy;
        std::string key;
        std::string url;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Parameters, reference, actual, output, delta, strategy, key, url)
    };

    CompareCommand (CLI::App* app, int& returnValue, FileSystem* fs);
    static int execute (const Parameters& parameters, FileSystem* fs);

private:
    /*  Compare output files from a single processing tests referenced by i */
    static std::vector<TestInfo> performProcessingTests (const juce::String& reference, const juce::String& actual,
                                                         size_t i, float delta, FileSystem* fs);

    /*  Compare output file that contain basic plugin information */
    static TestResult performBasicTests (const juce::String& reference, const juce::String& actual, FileSystem* fs,
                                         size_t& numTests);

    /*  Log results to console and to the file */
    static int logResults (const TestResult& result, const std::string& outputFile, FileSystem* fs);

    /*  Log a vector of testinfos */
    static size_t log (const std::vector<TestInfo>& infos);

    /*  Log a vector of vectors of testinfos s*/
    static size_t log (const std::vector<TestInfos>& infos);
    Parameters parameters;
};
