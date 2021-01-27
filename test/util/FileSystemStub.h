#pragma once
#include "fs/FileSystem.h"

struct FileSystemStub : FileSystem
{
    bool exists (const juce::String& /*file*/) override
    {
        return fileExists;
    }

    void write (const juce::String& file, const juce::String& content, bool override) override
    {
        writtenFiles.push_back (file);
        writtenContent.push_back (content);
        wasOverridden = override;
    }
    void remove (const juce::String& file) override
    {
        removedFiles.push_back (file);
    }

    juce::String readString (const juce::String& file) override
    {
        readFiles.push_back (file);
        return stringToRead;
    }

    std::unique_ptr<juce::OutputStream> createOutputStream (const juce::String& file, bool /*override*/) override
    {
        writtenFiles.push_back (file);
        writtenBlocks.push_back (juce::MemoryBlock());
        return std::make_unique<juce::MemoryOutputStream> (writtenBlocks.back(), false);
    }

    std::unique_ptr<juce::InputStream> createInputStream (const juce::String& file) override
    {
        readFiles.push_back (file);
        return std::make_unique<juce::MemoryInputStream> (readBlock, false);
    }

    std::unique_ptr<Plugin> openPlugin (const juce::String& /*file*/) override
    {
        throw std::runtime_error ("not implemented");
    }

    void extractZip (const juce::String& input, const juce::String& output) override {}

    void createZip (const juce::String&, const juce::String&) override {}
    void removeFolder (const juce::String& /*folder*/) override {}
    void createFolder (const juce::String& /*folder*/) override {}
    bool isPlugin (const juce::String& /*file*/) override
    {
        return true;
    }

    void fetchFileFromUrl (const juce::String& /*url*/, const juce::String& /*file*/) override {}
    juce::String postFileToUrl (const juce::String& /*url*/, const juce::String& /*file*/) override
    {
        return "";
    }

    bool fileExists = true;
    std::vector<juce::String> writtenFiles;
    std::vector<juce::String> writtenContent;
    std::vector<juce::MemoryBlock> writtenBlocks;
    std::vector<juce::String> readFiles;
    std::vector<juce::String> removedFiles;
    juce::MemoryBlock readBlock;
    bool wasOverridden;
    juce::String stringToRead;
};