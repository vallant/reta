#include "InputSignals.h"

#include "BinaryData.h"

InputSignals::InputSignals()
{
    for ( const auto& pair : signals() )
    {
        auto stream (std::make_unique<juce::MemoryInputStream> (pair.first, pair.second, false));
        auto buffer = Buffer::read (std::move (stream));
        inputBuffers.push_back (buffer);
    }
}

size_t InputSignals::size()
{
    return signals().size();
}

Buffer InputSignals::get (size_t index) const
{
    return inputBuffers[index];
}

std::vector<Buffer> InputSignals::get (size_t index, size_t blockSize, double sampleRate,
                                       const juce::AudioProcessor::BusesLayout& layout) const
{
    const auto& buffer = inputBuffers[index];
    std::vector<Buffer> blocks;
    int processed = 0;

    while ( processed < buffer.getNumSamples() )
    {
        auto remaining = buffer.getNumSamples() - processed;
        auto currentBlockSize = std::min (remaining, static_cast<int> (blockSize));

        auto inputChannels = 0;
        auto outputChannels = 0;
        for ( int i = 0; i < layout.inputBuses.size(); ++i )
            inputChannels += layout.getNumChannels (true, i);
        for ( int i = 0; i < layout.outputBuses.size(); ++i )
            outputChannels += layout.getNumChannels (false, i);

        Buffer block (std::max (inputChannels, outputChannels), currentBlockSize, buffer.getSampleRate());
        block.setSampleRate (sampleRate);

        for ( int i = 0; i < block.getNumChannels(); ++i )
            juce::FloatVectorOperations::copy (block.getWritePointer (i), buffer.getReadPointer (0) + processed,
                                               currentBlockSize);

        blocks.push_back (block);

        processed += currentBlockSize;
    }

    return blocks;
}
size_t InputSignals::numSamples() const
{
    jassert (size() > 0);
    int numSamples = inputBuffers[0].getNumSamples();
    for ( size_t i = 0; i < size(); ++i )
        jassert (numSamples == inputBuffers[i].getNumSamples());
    return static_cast<size_t> (numSamples);
}
std::vector<InputSignal> InputSignals::signals()
{
    std::vector<InputSignal> binaryInputFiles;
    binaryInputFiles.emplace_back (BinaryData::_1_wav, BinaryData::_1_wavSize);
    binaryInputFiles.emplace_back (BinaryData::_2_wav, BinaryData::_2_wavSize);
    binaryInputFiles.emplace_back (BinaryData::_3_wav, BinaryData::_3_wavSize);
    binaryInputFiles.emplace_back (BinaryData::_4_wav, BinaryData::_4_wavSize);
    return binaryInputFiles;
}
