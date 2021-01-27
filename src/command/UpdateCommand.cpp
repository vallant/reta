
#include "UpdateCommand.h"

#include <server/RetaClient.h>

UpdateCommand::UpdateCommand (CLI::App* app, int& returnValue, FileSystem* fs)
{
    auto update = app->add_subcommand ("update", "Update the reference for a plugin on the reta server.");
    update->add_option ("-u,--url", parameters.url, "URL of the reference server")->required();
    update->add_option ("-k,--key", parameters.key, "Key of the plugin as used on the server")->required();
    update->add_option ("-r,--reference", parameters.localFolder, "Local reference folder")->required();
    update->parse_complete_callback (
        [&returnValue, fs, &parameters = parameters] { returnValue = execute (parameters, fs); });
}

int UpdateCommand::execute (const UpdateCommand::Parameters& parameters, FileSystem* fs)
{
    std::cout << "Updating reference at path: " << parameters.localFolder << std::endl;
    std::cout << "Server url: " << parameters.url << ", key: " << parameters.key << std::endl;
    RetaClient::postReference (parameters.url, parameters.key, parameters.localFolder, fs);
    return 0;
}
