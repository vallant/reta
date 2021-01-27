#include "InfoCommand.h"

#include <plugin/PluginImpl.h>

#define RETA_PRINT_FIELD(variable, fieldName) #fieldName << ": " << variable.fieldName << std::endl
#define RETA_PRINT_FIELD_BOOL(variable, fieldName) \
#fieldName << ": " << (variable.fieldName ? "true" : "false") << std::endl

InfoCommand::InfoCommand (CLI::App* app, int& returnValue, FileSystem* fs)
{
    auto command =
        app->add_subcommand ("info", "Gather information about a local plugin binary and save it in JSON format");
    command->add_option ("-i,--input", parameters.input, "Path to plugin")->required();
    command->add_option ("-o,--output", parameters.output, "Path to output JSON")->required();
    command->add_flag ("-v,--verbose", parameters.verbose, "Print gathered info on command line");
    command->parse_complete_callback (
        [fs, &returnValue, &params = parameters]() { returnValue = InfoCommand::execute (params, fs); });
}

int InfoCommand::execute (const Parameters& parameters, FileSystem* fs)
{
    std::cout << "Opening plugin at path: " << parameters.input << std::endl;
    std::cout << "Saving info at path: " << parameters.output << std::endl;
    auto plugin = fs->openPlugin (parameters.input);
    auto info = plugin->getInfo();
    fs->writeJson (parameters.output, info, true);
    if ( parameters.verbose )
    {
        std::cout << "Basic Plugin Info: " << std::endl;
        std::cout << "   " << RETA_PRINT_FIELD (info, name);
        std::cout << "   " << RETA_PRINT_FIELD (info, inputBuses);
        std::cout << "   " << RETA_PRINT_FIELD (info, outputBuses);
        std::cout << "   " << RETA_PRINT_FIELD_BOOL (info, acceptsMidi);
        std::cout << "   " << RETA_PRINT_FIELD_BOOL (info, isMidiEffect);
        std::cout << "   " << RETA_PRINT_FIELD_BOOL (info, producesMidi);
        std::cout << "   " << RETA_PRINT_FIELD_BOOL (info, hasEditor);
        std::cout << "   " << RETA_PRINT_FIELD (info, latency);
        std::cout << "   " << RETA_PRINT_FIELD (info, tail);
        std::cout << "   " << RETA_PRINT_FIELD (info, precision);

        std::cout << "Supported Layouts: " << std::endl;
        for ( auto& layout : info.layouts )
            std::cout << " - " << layout.getDescription() << std::endl;

        std::cout << "Parameters: " << std::endl;
        for ( auto& parameter : info.parameters )
        {
            std::cout << " - Parameter index #" << parameter.index << std::endl;
            std::cout << "   " << RETA_PRINT_FIELD (parameter, name);
            std::cout << "   " << RETA_PRINT_FIELD (parameter, label);
            std::cout << "   " << RETA_PRINT_FIELD (parameter, value);
            std::cout << "   " << RETA_PRINT_FIELD (parameter, numSteps);
            std::cout << "   " << RETA_PRINT_FIELD (parameter, category);
            std::cout << "   " << RETA_PRINT_FIELD (parameter, defaultValue);
            std::cout << "   " << RETA_PRINT_FIELD (parameter, index);
            std::cout << "   " << RETA_PRINT_FIELD_BOOL (parameter, automatable);
            std::cout << "   " << RETA_PRINT_FIELD_BOOL (parameter, discrete);
            std::cout << "   " << RETA_PRINT_FIELD_BOOL (parameter, meta);
            std::cout << "   " << RETA_PRINT_FIELD_BOOL (parameter, inverted);
            std::cout << "   " << RETA_PRINT_FIELD_BOOL (parameter, boolean);
        }
    }

    return 0;
}
