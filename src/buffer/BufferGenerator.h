#pragma once
#include "Buffer.h"
#include "plugin/BusLayout.h"

/*  A base class for strategies that generate buffer blocks */
class BufferGenerator
{
public:
    virtual ~BufferGenerator() = default;

    /*  Called before @see getNextBlock */
    virtual void prepare (const BusLayout& layout, double sampleRate, size_t samplesPerBlock) = 0;

    /*  Should return a block with @see samlesPerBlock values */
    virtual Buffer getNextBlock() = 0;

    /*  Reset the buffer generator to the initial state. */
    virtual void reset() = 0;
};