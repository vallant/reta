#pragma once

#include "TestConfiguration.h"

#include <fs/FileSystemImpl.h>
#include <plugin/Plugin.h>
#include <test/InputSignals.h>

/* A class to perform the audio processing */
class TestCase
{
public:
    /* Initialize the test case */
    TestCase (std::unique_ptr<Plugin> plugin, juce::String outputPath,
              std::unique_ptr<FileSystem> fs = std::make_unique<FileSystemImpl>());

    /* Execute the test case with the specified configurations */
    int execute (const TestConfigurations& configurations, bool verbose = false);

    /* A struct to hold the float and string representations of parameters */
    struct Info
    {
        std::string parameterName;
        std::string textForValue;
        float valueForText;

        std::vector<TestInfo> compareTo (const Info& other) const;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE (Info, parameterName, textForValue, valueForText)
    };

    /* Execute the test case with the specified configuration */
    static std::pair<Buffer, TestConfiguration> execute (Plugin* plugin, const TestConfiguration& input);

    /* Prepare the plugin for a test configuration, and return the partitioned buffer blocks */
    static std::vector<Buffer> prepare (Plugin* plugin, const TestConfiguration& config, const InputSignals& signals);

    /* Process the buffer blocks */
    static void processBlocks (Plugin* plugin, std::vector<Buffer>& blocks);

    /* Aggregate the buffer blocks into one buffer again */
    static Buffer aggregateSum (const std::vector<Buffer>& blocks, const InputSignals& signals);

private:
    void prepare (const TestConfiguration& config);
    Buffer aggregateSum() const;
    void write (const TestConfiguration& config, size_t i);
    void write (const std::vector<Info>& info, size_t i);
    void processBlocks();
    std::vector<Info> collectInfos (const TestConfiguration& config);

    std::unique_ptr<Plugin> plugin;
    juce::String outputPath;
    std::unique_ptr<FileSystem> fs;

    std::vector<Buffer> blocks;
    InputSignals signals;
};
