#pragma once

#include "FileSystem.h"
#include "juce_core/juce_core.h"

class FileSystemImpl : public FileSystem
{
public:
    bool exists (const juce::String& file) override;
    void write (const juce::String& file, const juce::String& content, bool override) override;
    void remove (const juce::String& file) override;
    void removeFolder (const juce::String& folder) override;
    void createFolder (const juce::String& folder) override;

    std::unique_ptr<juce::OutputStream> createOutputStream (const juce::String& file, bool override) override;
    std::unique_ptr<juce::InputStream> createInputStream (const juce::String& file) override;

    std::unique_ptr<Plugin> openPlugin (const juce::String& file) override;

    bool isPlugin (const juce::String& file) override;

    void extractZip (const juce::String& input, const juce::String& output) override;
    void createZip (const juce::String& input, const juce::String& output) override;

    void fetchFileFromUrl (const juce::String& url, const juce::String& file) override;
    juce::String postFileToUrl (const juce::String& url, const juce::String& file) override;

private:
    juce::File getFileIfExists (const juce::String& file);

    juce::File getWritableFile (const juce::String& file, bool override);
};