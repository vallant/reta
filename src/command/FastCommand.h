#pragma once
#include "Command.h"
#include "fs/FileSystem.h"

#include <CLI/CLI.hpp>
#include <string>

/*  A command for quickly running through all stages of the app with default values */
class FastCommand : public Command
{
public:
    struct Parameters
    {
        std::string reference;
        std::string actual;
        std::string output;
    };

    FastCommand (CLI::App* app, int& returnValue, FileSystem* fs);
    static int execute (const Parameters& parameters, FileSystem* fs);

private:
    Parameters parameters;
};