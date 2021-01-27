#pragma once
#include <CLI/CLI.hpp>
#include <command/Command.h>
#include <fs/FileSystem.h>
#include <nlohmann/json.hpp>

/*  A command to update the reference located at the server provided by @ServeCommand */
class UpdateCommand : public Command
{
public:
    struct Parameters
    {
        std::string url;
        std::string key;
        std::string localFolder;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Parameters, url, key, localFolder);
    };

    UpdateCommand (CLI::App* app, int& returnValue, FileSystem* fs);
    static int execute (const Parameters& parameters, FileSystem* fs);

private:
    Parameters parameters;
};