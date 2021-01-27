#pragma once
#include "Command.h"

#include <CLI/CLI.hpp>
#include <fs/FileSystem.h>

/*  A class that runs the basic and audio tests using a set of test-configurations */
class TestCommand : public Command
{
public:
    struct Parameters
    {
        std::string pluginPath;
        std::string config;
        std::string output;
        bool verbose = false;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Parameters, pluginPath, config, output)
    };

    TestCommand (CLI::App* app, int& returnValue, FileSystem* fs);
    static int execute (const Parameters& parameters, FileSystem* fs);

private:
    Parameters parameters;
};