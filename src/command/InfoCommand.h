#pragma once
#include "Command.h"
#include "fs/FileSystem.h"

#include <CLI/App.hpp>
#include <plugin/Plugin.h>

/*  A command to extract the exposed plugin info and save it in a info.json and print it on the command line */
class InfoCommand : public Command
{
public:
    struct Parameters
    {
        std::string input;
        std::string output;
        bool verbose = false;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Parameters, input, output, verbose)
    };

    InfoCommand (CLI::App* app, int& returnValue, FileSystem* fs);
    static int execute (const Parameters& parameters, FileSystem* fs);

private:
    Parameters parameters;
};
