#pragma once
#include <buffer/Buffer.h>

using InputSignal = std::pair<const char*, int>;

/* A class to hold all available input signals */
class InputSignals
{
public:
    /* Initialize the input signals */
    InputSignals();

    /* Return the number of input signals */
    static size_t size();

    /* Get the signal at index */
    Buffer get (size_t index) const;

    /* Return a vector of the original signal partitioned into blocks, and replicated for all input channels */
    std::vector<Buffer> get (size_t index, size_t blockSize, double sampleRate,
                             const juce::AudioProcessor::BusesLayout& layout) const;

    /* Return the number of samples each input signal has */
    size_t numSamples() const;

private:
    static std::vector<InputSignal> signals();

    std::vector<Buffer> inputBuffers;
};
