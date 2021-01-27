#pragma once
#include "Command.h"

#include <CLI/CLI.hpp>
#include <fs/FileSystem.h>

/*  A command to generate a set of test-configurations from a plugin or a info.json */
class GenerateCommand : public Command
{
public:
    struct Parameters
    {
        std::string input;
        std::string json;
        std::string strategy = "dither";

        std::vector<std::string> exclude;
        int seed = 42;
        int interactions = 2;
        int numTests = 100;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Parameters, input, json, strategy, exclude, seed, interactions, numTests)
    };

    GenerateCommand (CLI::App* app, int& returnValue, FileSystem* fs);
    static int execute (const Parameters& parameters, FileSystem* fs);

private:
    Parameters parameters;
};
