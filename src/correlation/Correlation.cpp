#include "Correlation.h"
Buffer Correlation::cancel (const Buffer& lhs, const Buffer& rhs)
{
    if ( ! dimensionsMatch (lhs, rhs) )
        throw std::runtime_error ("Cancellation: Dimensions don't match.");

    Buffer result (lhs.getNumChannels(), lhs.getNumSamples(), lhs.getSampleRate());
    result.forEach ([&lhs, &rhs, &result] (int channel, int sample) {
        auto diff = lhs.getSample (channel, sample) - rhs.getSample (channel, sample);
        result.setSample (channel, sample, diff);
    });

    return result;
}
bool Correlation::dimensionsMatch (const Buffer& lhs, const Buffer& rhs)
{
    bool valid = true;
    valid &= juce::approximatelyEqual (lhs.getSampleRate(), rhs.getSampleRate());
    valid &= lhs.getNumSamples() == rhs.getNumSamples();
    valid &= lhs.getNumChannels() == rhs.getNumChannels();
    return valid;
}

float Correlation::rmse (const Buffer& lhs, const Buffer& rhs)
{
    if ( ! dimensionsMatch (lhs, rhs) )
        throw std::runtime_error ("Cancellation: Dimensions don't match.");

    Buffer sum (1, lhs.getNumSamples(), lhs.getSampleRate());
    float diff = 0.0f;

    for ( int i = 0; i < lhs.getNumChannels(); ++i )
    {
        juce::FloatVectorOperations::subtract (sum.getWritePointer (0), lhs.getReadPointer (i), rhs.getReadPointer (i),
                                               lhs.getNumSamples());
        juce::FloatVectorOperations::multiply (sum.getWritePointer (0), sum.getReadPointer (0), sum.getReadPointer (0),
                                               sum.getNumSamples());
    }
    for ( int sample = 0; sample < sum.getNumSamples(); ++sample )
        diff += sum.getSample (0, sample);

    return std::sqrt (diff / static_cast<float> (lhs.getNumSamples() * lhs.getNumChannels()));
}

float Correlation::rmse (FileSystem* fs, const juce::String& lhs, const juce::String& rhs)
{
    Buffer lhsBuffer = Buffer::read (fs->createInputStream (lhs));
    Buffer rhsBuffer = Buffer::read (fs->createInputStream (rhs));
    return rmse (lhsBuffer, rhsBuffer);
}

Buffer Correlation::cancel (FileSystem* fs, const juce::String& lhs, const juce::String& rhs)
{
    Buffer lhsBuffer = Buffer::read (fs->createInputStream (lhs));
    Buffer rhsBuffer = Buffer::read (fs->createInputStream (rhs));
    return cancel (lhsBuffer, rhsBuffer);
}
