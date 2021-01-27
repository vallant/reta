#pragma once

#include <CLI/CLI.hpp>
#include <command/Command.h>
#include <fs/FileSystem.h>
#include <nlohmann/json.hpp>
#include <string>
#include <test/TestConfiguration.h>
#include <test/TestInfo.h>

/*  A class to reduce failed test cases to relevant information */
class ReduceCommand : public Command
{
public:
    struct Parameters
    {
        std::string reference;
        std::string actual;
        std::string results;
        std::string output;
        std::string config;
        bool verbose;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Parameters, reference, actual, results, output, config, verbose)
    };

    ReduceCommand (CLI::App* app, int& returnValue, FileSystem* fs);
    static int execute (const Parameters& parameters, FileSystem* fs);

    /*  Entry point for the ddmin algorithm */
    static TestConfiguration ddmin (const TestConfiguration& failingConfig, Plugin* reference, Plugin* actual,
                                    size_t partitions = 2);

    /*  Runs the tests for the provided config with the reference and actual plugin.
        It returns if the test results are failing.
    */
    static bool failed (const TestConfiguration& config, Plugin* reference, Plugin* actual);

    /*  Splits the provided test configuration into numPartitions */
    static TestConfigurations split (const TestConfiguration& start, size_t numPartitions);

    /*  Computes the complements to a provided partition.
        For this to work you need to also specify the initial configuration.
    */
    static TestConfigurations computeComplements (const TestConfiguration& initialConfig,
                                                  const TestConfigurations& partitions);

    static void printReducedConfigs (const TestConfigurations& reduced);

private:
    Parameters parameters;
};
