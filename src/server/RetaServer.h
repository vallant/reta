#pragma once

#include <fs/FileSystem.h>
#include <httplib.h>
#include <juce_core/juce_core.h>

/* Server side of the reta server */
class RetaServer
{
public:
    /* Start a http-server listening on the specified port. The working directory will be used to store reference files.
     */
    static void serve (size_t port, const juce::String& workingDirectory, FileSystem* fs);

private:
    static void addHi (httplib::Server& server);
    static void addEcho (httplib::Server& server);
    static void addFileGet (httplib::Server& server, const juce::String& workingDirectory, FileSystem* fs);
    static void addFilePut (httplib::Server& server, const juce::String& workingDirectory, FileSystem* fs);
    static void addStop (httplib::Server& server);

    static bool parseParameters (const httplib::Request& request, std::map<juce::String, juce::String>& parameters);
    static void respondWithError (httplib::Response& response, int code, const juce::String& message);
};