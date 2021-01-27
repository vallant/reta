#pragma once
#include <CLI/CLI.hpp>
#include <command/Command.h>
#include <fs/FileSystem.h>
#include <nlohmann/json.hpp>

/*  A class to run a simple http server that can be used for cross-platform regression testing */
class ServeCommand : public Command
{
public:
    struct Parameters
    {
        std::string workingDirectory;
        size_t port;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Parameters, workingDirectory, port)
    };

    ServeCommand (CLI::App* app, int& returnValue, FileSystem* fs);
    static int execute (const Parameters& parameters, FileSystem* fs);

private:
    Parameters parameters;
};