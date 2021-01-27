#include "ServeCommand.h"

#include <server/RetaServer.h>
int ServeCommand::execute (const ServeCommand::Parameters& parameters, FileSystem* fs)
{
    std::cout << "Starting server at port: " << parameters.port << std::endl;
    std::cout << "Working directory: " << parameters.workingDirectory << std::endl;
    RetaServer::serve (parameters.port, parameters.workingDirectory, fs);
    return 0;
}

ServeCommand::ServeCommand (CLI::App* app, int& returnValue, FileSystem* fs)
{
    auto serve =
        app->add_subcommand ("serve", "Launch a local http server to serve references. This command will block!");
    serve->add_option ("-p,--port", parameters.port, "Port to be listening on")->required();
    serve->add_option ("-d,--directory", parameters.workingDirectory, "Working directory")->required();
    serve->parse_complete_callback (
        [&returnValue, fs, &parameters = parameters] { returnValue = execute (parameters, fs); });
}
