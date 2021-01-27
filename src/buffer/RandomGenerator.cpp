
#include "RandomGenerator.h"

#include "plugin/BusLayout.h"

RandomGenerator::RandomGenerator (size_t seed) : blockRandom (seed), samplesPerBlock (0), seed (seed) {}

void RandomGenerator::prepare (const BusLayout& newBusLayout, double newSampleRate, size_t newSamplesPerBlock)
{
    busLayout = newBusLayout;
    samplesPerBlock = newSamplesPerBlock;
    sampleRate = newSampleRate;
}

Buffer RandomGenerator::getNextBlock()
{
    Buffer buffer (static_cast<int> (busLayout.getNumChannels()), static_cast<int> (samplesPerBlock), sampleRate);

    for ( int sample = 0; sample < buffer.getNumSamples(); ++sample )
    {
        auto val = blockRandom.next (-1.0f, 1.0f);
        for ( int channel = 0; channel < buffer.getNumChannels(); ++channel )
        {
            buffer.setSample (channel, sample, val);
        }
    }

    return buffer;
}
void RandomGenerator::reset()
{
    blockRandom = Random (seed);
}
