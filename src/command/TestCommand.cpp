#include "TestCommand.h"

#include "test/TestCase.h"

TestCommand::TestCommand (CLI::App* app, int& returnValue, FileSystem* fs)
{
    auto test = app->add_subcommand (
        "test", "Perform regression tests using a test configuration generated by the 'generate' command");
    test->add_option ("-i,--input", parameters.pluginPath, "Path to plugin")->required();
    test->add_option ("-o,--output", parameters.output,
                      "Path to output folder. Folder will be created if it doesn't exist")
        ->required();
    test->add_option ("-c,--config", parameters.config,
                      "Path to the test configuration provided by the 'generate' stage")
        ->required();
    test->add_flag ("-v,--verbose", parameters.verbose, "Print progress on console");

    test->parse_complete_callback (
        [fs, &returnValue, &parameters = parameters] { returnValue = execute (parameters, fs); });
}

int TestCommand::execute (const Parameters& parameters, FileSystem* fs)
{
    if ( ! fs->exists (parameters.output) )
        fs->createFolder (parameters.output);
    auto configs = fs->readJson<TestConfigurations> (parameters.config);

    std::cout << "Testing plugin at path: " << parameters.pluginPath << std::endl;
    std::cout << "Using config at path: " << parameters.config << std::endl;
    std::cout << "Saving results at: " << parameters.output << std::endl;

    TestCase tc (fs->openPlugin (parameters.pluginPath), parameters.output);
    return tc.execute (configs, parameters.verbose);
}
