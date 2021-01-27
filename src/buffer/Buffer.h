#pragma once
#include <fs/FileSystem.h>
#include <fs/FileSystemImpl.h>
#include <juce_audio_utils/juce_audio_utils.h>

/*  An extension to the juce AudioSampleBuffer to be sample-rate aware. */
class Buffer : public juce::AudioSampleBuffer
{
public:
    /*  Initialize the buffer with zeros. */
    explicit Buffer (int numChannels, int numSamples, double sampleRate);

    /*  Write the complete buffer to the stream. The stream will be deleted. */
    void write (std::unique_ptr<juce::OutputStream> stream) const;

    /*  Read the buffer from the stream. The stream will be deleted. */
    static Buffer read (std::unique_ptr<juce::InputStream> stream);

    /*  Change/retrieve the samplerate */
    double getSampleRate() const;
    void setSampleRate (double newSampleRate);

    /*  Execute a function on each sample. */
    void forEach (const std::function<void (int, int)>& function);

    /*  Set each sample with the provided value */
    void fill (float value);

    /*  Append the second buffer to the current one */
    void operator+= (const Buffer& rhs);

private:
    double sampleRate;
};
