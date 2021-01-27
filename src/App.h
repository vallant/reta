#pragma once
#include <juce_events/juce_events.h>

class App : public juce::JUCEApplicationBase, juce::AsyncUpdater
{
public:
    explicit App (const juce::String& name);

    /*  Override this method for implementing your application */
    virtual int execute (int argc, char const* const argv[]) = 0;

    /*  Create vector of strings from a continuous string */
    static std::vector<std::string> splitArguments (const std::string& s, char delimiter);

    /*  Create a argv-compatible vector of char-pointers from a vector of strings.
        Beware that the strings don't go out of scope!
    */
    static std::vector<const char*> getArgv (const std::vector<std::string>& arguments);

    /*  Called by the juce framework. Triggers @see handleAsyncUpdate */
    void initialise (const juce::String& commandLineParameters) override;

    /*  Extracts the commandline parameters and passes it to @see execute */
    void handleAsyncUpdate() override;

private:
    /* Necessary overrides */
    const juce::String getApplicationName() override;
    const juce::String getApplicationVersion() override;
    bool moreThanOneInstanceAllowed() override;
    void shutdown() override;
    void anotherInstanceStarted (const juce::String& /*commandLine*/) override;
    void systemRequestedQuit() override;
    void suspended() override;
    void resumed() override;
    void unhandledException (const std::exception*, const juce::String& /*sourceFilename*/,
                             int /*lineNumber*/) override;

    juce::String params;
    juce::String name;
};