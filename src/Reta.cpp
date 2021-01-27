#include "Reta.h"

#include <command/CompareCommand.h>
#include <command/FastCommand.h>
#include <command/GenerateCommand.h>
#include <command/InfoCommand.h>
#include <command/ReduceCommand.h>
#include <command/ServeCommand.h>
#include <command/TestCommand.h>
#include <command/UpdateCommand.h>

Reta::Reta (std::unique_ptr<FileSystem> fs) : fs (std::move (fs)) {}

int Reta::execute (int argc, char const* const argv[])
{
    CLI::App app {"reta"};
    app.require_subcommand();

    int returnValue = -1;
    commands.push_back (std::make_unique<InfoCommand> (&app, returnValue, fs.get()));
    commands.push_back (std::make_unique<GenerateCommand> (&app, returnValue, fs.get()));
    commands.push_back (std::make_unique<TestCommand> (&app, returnValue, fs.get()));
    commands.push_back (std::make_unique<CompareCommand> (&app, returnValue, fs.get()));
    commands.push_back (std::make_unique<ReduceCommand> (&app, returnValue, fs.get()));
    commands.push_back (std::make_unique<ServeCommand> (&app, returnValue, fs.get()));
    commands.push_back (std::make_unique<UpdateCommand> (&app, returnValue, fs.get()));
    commands.push_back (std::make_unique<FastCommand> (&app, returnValue, fs.get()));

    try
    {
        app.parse (argc, argv);
        return returnValue;
    }
    catch ( const CLI::ParseError& e )
    {
        return app.exit (e);
    }
    catch ( const std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
}
