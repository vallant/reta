#include "App.h"

App::App (const juce::String& name) : name (name) {}

const juce::String App::getApplicationName()
{
    return name;
}

const juce::String App::getApplicationVersion()
{
    return "0.0.1";
}

bool App::moreThanOneInstanceAllowed()
{
    return true;
}

void App::initialise (const juce::String& commandLineParameters)
{
    params = getApplicationName() + " " + commandLineParameters;
    triggerAsyncUpdate();
}

void App::shutdown() {}

void App::anotherInstanceStarted (const juce::String&) {}

void App::systemRequestedQuit()
{
    quit();
}

void App::suspended() {}

void App::resumed() {}

std::vector<std::string> App::splitArguments (const std::string& s, char delimiter)
{
    std::stringstream ss (s);
    std::string item;
    std::vector<std::string> elements;
    while ( std::getline (ss, item, delimiter) )
    {
        std::replace (item.begin(), item.end(), delimiter, ' ');
        if ( ! item.empty() )
            elements.push_back (std::move (item));
    }
    return elements;
}

std::vector<const char*> App::getArgv (const std::vector<std::string>& arguments)
{
    std::vector<const char*> argv;
    argv.reserve (arguments.size());
    for ( auto& argument : arguments )
        argv.push_back (argument.data());
    return argv;
}

void App::handleAsyncUpdate()
{
    auto args = splitArguments (params.toStdString(), ' ');
    auto argv = getArgv (args);
    auto returnValue = execute (static_cast<int> (argv.size()), &argv[0]);
    setApplicationReturnValue (returnValue);
    quit();
}
void App::unhandledException (const std::exception*, const juce::String&, int) {}
