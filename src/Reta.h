#pragma once

#include <App.h>
#include <CLI/CLI.hpp>
#include <command/Command.h>
#include <fs/FileSystemImpl.h>

/* Main program */
class Reta
{
public:
    Reta (std::unique_ptr<FileSystem> fs = std::make_unique<FileSystemImpl>());
    int execute (int argc, char const* const argv[]);

private:
    std::vector<std::unique_ptr<Command>> commands;
    std::unique_ptr<FileSystem> fs;
};
