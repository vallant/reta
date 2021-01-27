//
// Created by Stephan on 15.01.21.
//

#include "RetaClient.h"
void RetaClient::fetchReference (const juce::String& url, const juce::String& key, const juce::String& folder,
                                 FileSystem* fs)
{
    auto referenceFile = folder + ".zip";
    fs->fetchFileFromUrl (url + "/file?key=" + key, referenceFile);
    fs->extractZip (referenceFile, folder);
}
void RetaClient::postReference (const juce::String& url, const juce::String& key, const juce::String& folder,
                                FileSystem* fs)
{
    auto zipPath = FileSystem::getTempFolder() + key + ".zip";
    fs->createZip (folder, zipPath);
    auto response = fs->postFileToUrl (url + "/file?key=" + key, zipPath);
    if ( response != "OK" && response != "Replaced" )
        throw std::runtime_error (
            ("Failed to upload folder: " + folder + " with key: " + key + " to server: " + url).toStdString());
}
