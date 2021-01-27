#include "RetaServer.h"

void RetaServer::serve (size_t port, const juce::String& workingDirectory, FileSystem* fs)
{
    if ( ! juce::File::isAbsolutePath (workingDirectory) )
        throw std::runtime_error ("Working Directory Path must be absolute");

    httplib::Server server;

    addHi (server);
    addEcho (server);
    addFileGet (server, workingDirectory, fs);
    addFilePut (server, workingDirectory, fs);
    addStop (server);

    auto success = server.listen ("0.0.0.0", port);
    if ( ! success )
        throw std::runtime_error ("Failed to open server");
}
void RetaServer::addHi (httplib::Server& server)
{
    server.Get ("/hi", [] (const httplib::Request&, httplib::Response& response) {
        response.set_content ("Hello World!", "text/plain");
    });
}
void RetaServer::addEcho (httplib::Server& server)
{
    server.Get ("/echo", [] (const httplib::Request& request, httplib::Response& response) {
        std::map<juce::String, juce::String> params {{"text", ""}};
        if ( ! parseParameters (request, params) )
            return respondWithError (response, 400, "text parameter is missing");
        response.set_content (params["text"].toStdString(), "text/plain");
    });
}
void RetaServer::addFileGet (httplib::Server& server, const juce::String& workingDirectory, FileSystem* fs)
{
    server.Get ("/file", [fs, workingDirectory] (const httplib::Request& request, httplib::Response& response) {
        std::map<juce::String, juce::String> params {{"key", ""}};
        if ( ! parseParameters (request, params) )
            return respondWithError (response, 400, "key parameter missing");

        auto key = params["key"];
        auto fileName = workingDirectory + FileSystem::sep() + key + ".zip";

        if ( ! fs->exists (fileName) )
            return respondWithError (response, 404, "File not found on server");

        auto block = std::make_shared<juce::MemoryBlock>();
        fs->createInputStream (fileName)->readIntoMemoryBlock (*block);
        auto writeLambda = [block] (size_t offset, size_t length, httplib::DataSink& sink) {
            const auto d = reinterpret_cast<const char*> (block->getData());
            sink.write (&d[offset], std::min (length, static_cast<size_t> (1024)));
            return true;
        };

        response.set_content_provider (block->getSize(), "application/zip", writeLambda);
    });
}

void RetaServer::addStop (httplib::Server& server)
{
    server.Get ("/stop", [&server] (const httplib::Request&, httplib::Response&) { server.stop(); });
}

void RetaServer::addFilePut (httplib::Server& server, const juce::String& workingDirectory, FileSystem* fs)
{
    server.Post ("/file", [fs, workingDirectory] (const httplib::Request& request, httplib::Response& response,
                                                  const httplib::ContentReader& contentReader) {
        std::map<juce::String, juce::String> params {{"key", ""}};
        if ( ! parseParameters (request, params) )
            return respondWithError (response, 400, "Path parameter missing");

        auto key = params["key"];
        auto fileName = workingDirectory + FileSystem::sep() + key + ".zip";
        bool replaced = fs->exists (fileName);
        auto stream = fs->createOutputStream (fileName, true);

        contentReader ([&stream] (const char* data, size_t length) {
            stream->write (data, length);
            return true;
        });

        response.set_content (replaced ? "Replaced" : "OK", "text/plain");
        response.status = replaced ? 201 : 200;
    });
}

bool RetaServer::parseParameters (const httplib::Request& request, std::map<juce::String, juce::String>& parameters)
{
    for ( auto& pair : parameters )
    {
        if ( ! request.has_param (pair.first.getCharPointer()) )
            return false;
        pair.second = request.get_param_value (pair.first.getCharPointer());
    }
    return true;
}

void RetaServer::respondWithError (httplib::Response& response, int code, const juce::String& message)
{
    response.set_content (message.toStdString(), "text/plain");
    response.status = code;
}
