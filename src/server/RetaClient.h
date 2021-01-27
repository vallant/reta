#pragma once
#include <fs/FileSystem.h>

/* Client side for the reta server */
class RetaClient
{
public:
    /* Retrive the reference file for the provided url and key and save it to the specified folder */
    static void fetchReference (const juce::String& url, const juce::String& key, const juce::String& outputFolder,
                                FileSystem* fs);
    /* Replace the reference file on the server with the specified local one */
    static void postReference (const juce::String& url, const juce::String& key, const juce::String& inputFolder,
                               FileSystem* fs);
};