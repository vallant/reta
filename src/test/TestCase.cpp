
#include "TestCase.h"

#include <correlation/Correlation.h>
#include <util/Permutation.h>
TestCase::TestCase (std::unique_ptr<Plugin> plugin, juce::String outputPath, std::unique_ptr<FileSystem> fs) :
    plugin (std::move (plugin)), outputPath (std::move (outputPath)), fs (std::move (fs))
{
}

int TestCase::execute (const TestConfigurations& configs, bool verbose)
{
    fs->writeJson (outputPath + FileSystem::sep() + "info.json", plugin->getInfo(), true);
    fs->writeJson (outputPath + FileSystem::sep() + "config.json", configs, true);
    if ( verbose )
        std::cout << "==============" << std::endl;

    for ( size_t i = 0; i < configs.size(); ++i )
    {
        if ( verbose )
            std::cout << "\rRunning test #" << i + 1 << "/" << configs.size();
        const auto& config = configs[i];

        prepare (config);
        processBlocks();

        auto infos = collectInfos (config);
        write (infos, i);
        write (config, i);
    }
    if ( verbose )
        std::cout << std::endl << "==============" << std::endl << "Done." << std::endl;

    return 0;
}

std::vector<TestCase::Info> TestCase::collectInfos (const TestConfiguration& config)
{
    std::vector<TestCase::Info> infos;
    for ( auto& param : config.parameters )
        infos.push_back ({param.name, plugin->getCurrentParameterText (param.name).toStdString(),
                          plugin->getValueFromText (param.name)});
    return infos;
}
std::pair<Buffer, TestConfiguration> TestCase::execute (Plugin* plugin, const TestConfiguration& input)
{
    InputSignals signals;
    auto i = input;
    auto blocks = prepare (plugin, i, signals);

    processBlocks (plugin, blocks);
    return {aggregateSum (blocks, signals), i};
}

std::vector<Buffer> TestCase::prepare (Plugin* plugin, const TestConfiguration& config, const InputSignals& signals)
{
    plugin->reset();
    plugin->prepare (config.sampleRate, config.blockSize, config.layout);
    for ( auto& param : config.parameters )
    {
        if ( param.value >= 0.0f )
            plugin->setParameter (param.name, param.value);
    }

    return signals.get (config.inputSignal, config.blockSize, config.sampleRate, config.layout);
}

void TestCase::processBlocks (Plugin* plugin, std::vector<Buffer>& blocks)
{
    for ( auto& block : blocks )
    {
        plugin->process (block);
    }
}

Buffer TestCase::aggregateSum (const std::vector<Buffer>& blocks, const InputSignals& signals)
{
    Buffer sum (blocks[0].getNumChannels(), static_cast<int> (signals.numSamples()), blocks[0].getSampleRate());
    int processed = 0;
    for ( auto& block : blocks )
    {
        for ( int channel = 0; channel < block.getNumChannels(); ++channel )
        {
            sum.copyFrom (channel, processed, block, channel, 0, block.getNumSamples());
        }
        processed += block.getNumSamples();
    }
    return sum;
}

void TestCase::prepare (const TestConfiguration& config)
{
    blocks = prepare (plugin.get(), config, signals);
}

Buffer TestCase::aggregateSum() const
{
    return aggregateSum (blocks, signals);
}

void TestCase::write (const TestConfiguration& config, size_t i)
{
    auto fileName = outputPath + FileSystem::sep() + "test" + juce::String (i) + ".wav";

    auto stream = fs->createOutputStream (fileName, true);
    aggregateSum().write (std::move (stream));
}

void TestCase::write (const std::vector<Info>& info, size_t i)
{
    auto fileName = outputPath + FileSystem::sep() + "test" + juce::String (i) + ".json";
    fs->writeJson (fileName, info, true);
}

void TestCase::processBlocks()
{
    processBlocks (plugin.get(), blocks);
}

std::vector<TestInfo> TestCase::Info::compareTo (const TestCase::Info& other) const
{
    std::vector<TestInfo> infos;
    infos.push_back (RETA_CREATE_TEST_STR (parameterName));
    infos.push_back (RETA_CREATE_TEST_STR (textForValue));
    infos.push_back (RETA_CREATE_TEST_FLOAT (valueForText));

    for ( auto& test : infos )
        test.name = parameterName + ": " + test.name;
    return infos;
}
