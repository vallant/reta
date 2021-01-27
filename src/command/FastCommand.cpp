#include "FastCommand.h"

#include "CompareCommand.h"
#include "GenerateCommand.h"
#include "InfoCommand.h"
#include "TestCommand.h"

FastCommand::FastCommand (CLI::App* app, int& returnValue, FileSystem* fs)
{
    auto fast = app->add_subcommand ("fast", "Quickly compare two local plugins using default options");
    fast->add_option ("-r,--reference", parameters.reference, "Path to reference plugin")->required();
    fast->add_option ("-a,--actual", parameters.actual, "Path to actual plugin")->required();
    fast->add_option ("-o,--output", parameters.output, "Path to output json")->required();
    fast->parse_complete_callback (
        [fs, &returnValue, &parameters = parameters]() { returnValue = execute (parameters, fs); });
}

int FastCommand::execute (const Parameters& parameters, FileSystem* fs)
{
    int returnValue = 0;
    auto folder = fs->createTempFolder();
    auto info = folder + fs->sep() + "info.json";
    auto config = folder + fs->sep() + "config.json";
    auto outputReference = folder + fs->sep() + "outReference";
    auto outputActual = folder + fs->sep() + "outActual";
    InfoCommand::execute ({parameters.reference, info.toStdString()}, fs);
    GenerateCommand::execute ({info.toStdString(), config.toStdString()}, fs);
    TestCommand::execute ({parameters.reference, config.toStdString(), outputReference.toStdString()}, fs);
    TestCommand::execute ({parameters.actual, config.toStdString(), outputActual.toStdString()}, fs);
    CompareCommand::execute ({outputReference.toStdString(), outputActual.toStdString(), parameters.output}, fs);
    return returnValue;
}
