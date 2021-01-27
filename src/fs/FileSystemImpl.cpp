#include "FileSystemImpl.h"

#include <plugin/PluginImpl.h>

bool FileSystemImpl::exists (const juce::String& file)
{
    return juce::File (file).exists();
}

void FileSystemImpl::write (const juce::String& file, const juce::String& content, bool override)
{
    auto stream = createOutputStream (file, override);
    if ( ! stream->writeString (content) )
        throw std::runtime_error ("Could not write to file");
}

void FileSystemImpl::remove (const juce::String& file)
{
    auto f = getFileIfExists (file);
    if ( f.existsAsFile() )
    {
        if ( ! f.deleteFile() )
            throw std::runtime_error ("Could not delete file");
    }
    else
    {
        removeFolder (file);
    }
}

void FileSystemImpl::removeFolder (const juce::String& folder)
{
    juce::File f (folder);
    if ( ! f.exists() )
        return;

    if ( f.existsAsFile() )
        throw std::runtime_error ("Could not remove as folder, because it is a file: " + folder.toStdString());

    if ( ! f.deleteRecursively (true) )
        throw std::runtime_error ("Could not delete folder:" + folder.toStdString());
}

void FileSystemImpl::createFolder (const juce::String& folder)
{
    juce::File f (folder);
    if ( f.existsAsFile() )
        throw std::runtime_error ("Path exists as file: " + folder.toStdString());

    if ( f.exists() )
        return;

    if ( ! f.createDirectory() )
        throw std::runtime_error ("Could not remove folder: " + folder.toStdString());
}

std::unique_ptr<juce::OutputStream> FileSystemImpl::createOutputStream (const juce::String& file, bool override)
{
    auto stream = getWritableFile (file, override).createOutputStream();

    if ( stream == nullptr || ! stream->openedOk() )
        throw std::runtime_error ("Stream could not be opened for file: " + file.toStdString());

    if ( exists (file) && override )
    {
        auto success = stream->setPosition (0);
        success &= stream->truncate().wasOk();
        if ( ! success )
            throw std::runtime_error ("Truncation of file didn't work");
    }
    return std::unique_ptr<juce::OutputStream> (stream.release());
}

std::unique_ptr<juce::InputStream> FileSystemImpl::createInputStream (const juce::String& file)
{
    auto stream = getFileIfExists (file).createInputStream();
    if ( ! stream->openedOk() )
        throw std::runtime_error ("Stream could not be opened");
    return std::unique_ptr<juce::InputStream> (stream.release());
}

juce::File FileSystemImpl::getFileIfExists (const juce::String& file)
{
    if ( ! exists (file) )
        throw std::runtime_error (file.toStdString() + ": File doesn't exists");

    return juce::File (file);
}

juce::File FileSystemImpl::getWritableFile (const juce::String& file, bool override)
{
    if ( exists (file) && ! override )
        throw std::runtime_error ("File exists: " + file.toStdString());
    juce::File f (file);

    if ( ! f.hasWriteAccess() )
        throw std::runtime_error ("File is read-only: " + file.toStdString());

    if ( f.isDirectory() )
        throw std::runtime_error ("Path is a folder: " + file.toStdString());

    return f;
}
std::unique_ptr<Plugin> FileSystemImpl::openPlugin (const juce::String& file)
{
    return std::make_unique<PluginImpl> (file);
}

bool FileSystemImpl::isPlugin (const juce::String& file)
{
    juce::AudioPluginFormatManager formatManager;
    formatManager.addDefaultFormats();
    juce::OwnedArray<juce::PluginDescription> descriptions;
    for ( auto& format : formatManager.getFormats() )
        format->findAllTypesForFile (descriptions, file);

    return ! descriptions.isEmpty();
}

void FileSystemImpl::extractZip (const juce::String& input, const juce::String& output)
{
    if ( ! exists (output) )
        createFolder (output);

    if ( ! exists (input) )
        throw std::runtime_error ("input file does not exist");

    auto outputFile = juce::File (output);

    if ( outputFile.existsAsFile() )
        throw std::runtime_error ("decompress target must be a folder");

    auto result = juce::ZipFile (input).uncompressTo (outputFile, true);
    if ( result.failed() )
        throw std::runtime_error ("failed to extract zipfile: " + input.toStdString());
}

void FileSystemImpl::createZip (const juce::String& input, const juce::String& output)
{
    auto folder = getFileIfExists (input);
    auto childFiles = folder.findChildFiles (juce::File::TypesOfFileToFind::findFiles, false);

    juce::ZipFile::Builder builder;
    for ( auto& childFile : childFiles )
        builder.addFile (childFile, 0, childFile.getFileName());

    auto stream = createOutputStream (output, true);
    builder.writeToStream (*stream, nullptr);
}
void FileSystemImpl::fetchFileFromUrl (const juce::String& url, const juce::String& file)
{
    juce::MemoryBlock block;
    auto success = juce::URL (url).readEntireBinaryStream (block);
    if ( ! success )
        throw std::runtime_error ("URL file read failed from URL: " + url.toStdString());
    createOutputStream (file, true)->write (block.getData(), block.getSize());
}

juce::String FileSystemImpl::postFileToUrl (const juce::String& url, const juce::String& file)
{
    auto block = readBinary (file);
    auto request = juce::URL::createWithoutParsing (url).withPOSTData (block);
    return request.readEntireTextStream (true);
}
