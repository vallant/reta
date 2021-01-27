#include "server/RetaServer.h"

#include "../util/FileSystemStub.h"

#include <catch2/catch.hpp>
#include <future>

TEST_CASE ("RetaServer", "[integration][server]")
{
    FileSystemStub fs;
    juce::File (__FILE__).loadFileAsData (fs.readBlock);
    auto server = std::async ([&fs] { RetaServer::serve (8080, FileSystem::sep() + "bar", &fs); });

    SECTION ("hi works")
    {
        REQUIRE (juce::URL ("http://localhost:8080/hi").readEntireTextStream() == "Hello World!");
    }

    SECTION ("echo works")
    {
        REQUIRE (juce::URL ("http://localhost:8080/echo?text=hello").readEntireTextStream() == "hello");
    }

    SECTION ("get works")
    {
        juce::MemoryBlock dest;
        juce::URL ("http://localhost:8080/file?key=foo").readEntireBinaryStream (dest);
        REQUIRE (dest == fs.readBlock);
        REQUIRE (fs.readFiles.size() == 1);
        REQUIRE (fs.readFiles[0] == FileSystem::sep() + "bar" + FileSystem::sep() + "foo.zip");
    }

    SECTION ("post works")
    {
        fs.fileExists = false;
        auto response = juce::URL::createWithoutParsing ("http://localhost:8080/file?key=foo")
                            .withPOSTData (fs.readBlock)
                            .readEntireTextStream (true);
        REQUIRE (response == "OK");
        REQUIRE (fs.writtenFiles.size() == 1);
        REQUIRE (fs.writtenFiles[0] == FileSystem::sep() + "bar" + FileSystem::sep() + "foo.zip");
        REQUIRE (fs.readBlock == fs.writtenBlocks[0]);
    }

    SECTION ("post works replacing")
    {
        auto response = juce::URL::createWithoutParsing ("http://localhost:8080/file?key=foo")
                            .withPOSTData (fs.readBlock)
                            .readEntireTextStream (true);
        REQUIRE (response == "Replaced");
        REQUIRE (fs.writtenFiles.size() == 1);
        REQUIRE (fs.writtenFiles[0] == FileSystem::sep() + "bar" + FileSystem::sep() + "foo.zip");
        REQUIRE (fs.readBlock == fs.writtenBlocks[0]);
    }

    juce::URL ("http://localhost:8080/stop").readEntireTextStream();
    server.wait();
}