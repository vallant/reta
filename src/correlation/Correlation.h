#pragma once

#include <buffer/Buffer.h>

class Correlation
{
public:
    /*  Cancel two buffers after reading them from the file system and return the result */
    static Buffer cancel (FileSystem* fs, const juce::String& lhs, const juce::String& rhs);
    /*  Cancel the two provided buffers and return the result */
    static Buffer cancel (const Buffer& lhs, const Buffer& rhs);

    /*  Compute the root of mean squared error of the two buffers after reading them from the file system */
    static float rmse (FileSystem* fs, const juce::String& lhs, const juce::String& rhs);

    /*  Compute the root of mean squared error of the two buffers */
    static float rmse (const Buffer& lhs, const Buffer& rhs);

    /*  Check if there is a dimension mismatch between the buffers (number of samples or number of channels) */
    static bool dimensionsMatch (const Buffer& lhs, const Buffer& rhs);
};
