#include "fs/FileSystemImpl.h"

#include "catch2/catch.hpp"
#include "juce_core/juce_core.h"

#include <buffer/Buffer.h>

bool isWin()
{
    return (juce::SystemStats::getOperatingSystemType() & juce::SystemStats::Windows) != 0;
}
bool isMac()
{
    return (juce::SystemStats::getOperatingSystemType() & juce::SystemStats::MacOSX) != 0;
}

TEST_CASE ("FileSystemImpl", "[integration][filesystem]")
{
    std::unique_ptr<FileSystem> fs (new FileSystemImpl());

    auto tmp = juce::File::getSpecialLocation (juce::File::SpecialLocationType::tempDirectory).getFullPathName() + "/"
               + juce::Uuid().toString();
    auto nonexistent = tmp + "/nonexistent";
    auto folder = tmp + "/folder";
    auto file = tmp + "/file";

    juce::File (file).create();
    juce::File (folder).createDirectory();

    try
    {
        fs->remove (nonexistent);
    }
    catch ( const std::exception& /*ignore*/ )
    {
    }

    SECTION ("fs reports present file correctly")
    {
        REQUIRE (fs->exists (file));
    }

    SECTION ("fs reports present folder correctly")
    {
        REQUIRE (fs->exists (folder));
    }

    SECTION ("fs reports non-existing file correctly")
    {
        REQUIRE_FALSE (fs->exists (nonexistent));
    }

    SECTION ("write to existing file without override fails")
    {
        REQUIRE_THROWS (fs->write (file, "foo", false));
    }

    SECTION ("write to non-existing file works")
    {
        REQUIRE_NOTHROW (fs->write (nonexistent, "foo", true));
        REQUIRE (fs->exists (nonexistent));
        REQUIRE (fs->readString (nonexistent) == "foo");
    }

    // todo: file permissions seem to be discarded by git
    // SECTION ("write to existing file without permissions fails") { REQUIRE_THROWS (fs->write (readOnly, "foo",
    // true)); }

    SECTION ("Write of audio buffer works")
    {
        // stereo sine with 450 hz
        Buffer buffer (2, 50000, 44100.0);
        auto cyclesPerSample = 450 / buffer.getSampleRate();

        auto angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;

        auto currentAngle = 0.0;
        for ( auto sample = 0; sample < buffer.getNumSamples(); ++sample )
        {
            auto currentSample = static_cast<float> (std::sin (currentAngle));
            currentAngle += angleDelta;
            buffer.setSample (0, sample, currentSample);
            buffer.setSample (1, sample, currentSample);
        }

        auto stream = fs->createOutputStream (nonexistent, true);
        REQUIRE_NOTHROW (buffer.write (std::move (stream)));
    }

    SECTION ("overriding using stream works")
    {
        {
            auto stream = fs->createOutputStream (nonexistent, true);
            stream->writeText ("foo", false, false, "\n");
        }

        {
            auto stream = fs->createOutputStream (nonexistent, true);
            stream->writeText ("foo", false, false, "\n");
        }

        REQUIRE (fs->readString (nonexistent) == "foo");
    }

    SECTION ("ZIP")
    {
        auto zipFile = FileSystem::getTempFolder() + "foo.zip";
        auto zipOutputFolder = FileSystem::getTempFolder() + "fooOut";
        auto zipInputFolder = FileSystem::getTempFolder() + "fooIn";

        juce::File (zipInputFolder).createDirectory();
        juce::File (zipInputFolder + FileSystem::sep() + "foo.txt").createOutputStream()->writeString ("foo");

        fs->createZip (zipInputFolder, zipFile);

        REQUIRE (juce::File (zipFile).existsAsFile());

        fs->extractZip (zipFile, zipOutputFolder);
        REQUIRE (juce::File (zipOutputFolder).exists());
        REQUIRE (juce::File (zipOutputFolder).isDirectory());

        auto outFile = juce::File (zipOutputFolder + FileSystem::sep() + "foo.txt");

        REQUIRE (outFile.existsAsFile());
        REQUIRE (outFile.createInputStream()->readEntireStreamAsString() == "foo");

        fs->remove (zipFile);
        fs->remove (zipInputFolder);
        fs->remove (zipOutputFolder);
    }

    for ( auto& a : {nonexistent, folder} )
    {
        try
        {
            fs->remove (a);
        }
        catch ( const std::exception& /*ignore*/ )
        {
        }
    }
}
