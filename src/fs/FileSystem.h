#pragma once

#include <juce_core/juce_core.h>
#include <nlohmann/json.hpp>
#include <utility>

// Forward declaration of the plugin class
class Plugin;

/*  Abstract base class for file system access */
struct FileSystem
{
    virtual ~FileSystem() = default;

    /*  Check if the file or folder exists */
    virtual bool exists (const juce::String& file) = 0;

    /*  Write the provided text to the file. This method might throw an exception! */
    virtual void write (const juce::String& file, const juce::String& content, bool override) = 0;

    /*  Remove the referenced file. */
    virtual void remove (const juce::String& file) = 0;

    /*  Remove the referenced folder. This method might throw an exception! */
    virtual void removeFolder (const juce::String& folder) = 0;

    /*  Create the referenced folder. This method might throw an exception! */
    virtual void createFolder (const juce::String& folder) = 0;

    /* Create an output-stream to the referenced file. Might throw! */
    virtual std::unique_ptr<juce::OutputStream> createOutputStream (const juce::String& file, bool override) = 0;

    /* Create an input-stream to the referenced file. Might throw! */
    virtual std::unique_ptr<juce::InputStream> createInputStream (const juce::String& file) = 0;

    /*  Tries to open an audio plugin at the provided location. Might throw! */
    virtual std::unique_ptr<Plugin> openPlugin (const juce::String& file) = 0;

    /*  Returns if the provided location can be opened as an audio plugin */
    virtual bool isPlugin (const juce::String& file) = 0;

    /*  Create a zip-file out of the provided input folder */
    virtual void createZip (const juce::String& inputFolder, const juce::String& outputFile) = 0;

    /*  Extract the zip-file to the provided output-Folder*/
    virtual void extractZip (const juce::String& inputFile, const juce::String& outputFolder) = 0;

    /*  Fetch a file from a URL and save it at the provided location */
    virtual void fetchFileFromUrl (const juce::String& url, const juce::String& file) = 0;

    /*  Post a file to the URL */
    virtual juce::String postFileToUrl (const juce::String& url, const juce::String& file) = 0;

    /*  Helper method to read a file into a binary block */
    juce::MemoryBlock readBinary (const juce::String& file)
    {
        juce::MemoryBlock block;
        createInputStream (file)->readIntoMemoryBlock (block);
        return block;
    }

    /*  Helper method for reading a stream into a string */
    virtual juce::String readString (const juce::String& file)
    {
        return createInputStream (file)->readEntireStreamAsString();
    }

    /*  Return the path to the system-depended temp folder */
    static juce::String getTempFolder()
    {
        return juce::File::getSpecialLocation (juce::File::SpecialLocationType::tempDirectory).getFullPathName()
               + juce::File::getSeparatorString();
    }

    /* Create a temporary folder using a UUID and return its path */
    juce::String createTempFolder()
    {
        auto folder = getTempFolder() + juce::Uuid().toString();
        createFolder (folder);
        return folder;
    }

    /*  Return the system-dependent path-separator-string */
    static juce::String sep()
    {
        return juce::File::getSeparatorString();
    }

    /*  Read a file into a JSON-enabled class. Might throw! */
    template <class T>
    T readJson (const juce::String& file)
    {
        return nlohmann::json::parse (readString (file).toStdString()).get<T>();
    }

    /*  Write a JSON-enabled class to a file */
    template <class T>
    void writeJson (const juce::String& file, const T& object, bool override)
    {
        write (file, nlohmann::json (object).dump (2), override);
    };

    /*  A temporary folder that is deleted from the file system after this class goes out of scope*/
    class ScopedFolder
    {
    public:
        /*  Create a temporary folder in the system tmp directory */
        explicit ScopedFolder (FileSystem* fs) :
            ScopedFolder (fs, FileSystem::getTempFolder() + juce::Uuid().toString())
        {
        }
        /*  Create a temp folder at the provided location */
        ScopedFolder (FileSystem* fs, const juce::String& folder) : fs (fs), folder (folder)
        {
            fs->createFolder (folder);
        }
        ~ScopedFolder()
        {
            fs->removeFolder (folder);
        }
        /*  Return the path of the temp folder */
        juce::String getFolder() const
        {
            return folder;
        }

    private:
        FileSystem* fs;
        juce::String folder;
    };
};