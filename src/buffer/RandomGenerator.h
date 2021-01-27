#pragma once

#include "Buffer.h"
#include "BufferGenerator.h"
#include "plugin/BusLayout.h"
#include "util/Random.h"

/*  A random buffer generator */
class RandomGenerator : public BufferGenerator
{
public:
    explicit RandomGenerator (size_t seed);
    void prepare (const BusLayout& newBusLayout, double newSampleRate, size_t newSamplesPerBlock) override;
    Buffer getNextBlock() override;
    void reset() override;

private:
    Random blockRandom;
    size_t samplesPerBlock;
    BusLayout busLayout;
    double sampleRate {};
    size_t seed {};
};