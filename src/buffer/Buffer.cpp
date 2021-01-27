#include "Buffer.h"

Buffer::Buffer (int numChannels, int numSamples, double sampleRate) :
    juce::AudioBuffer<float> (numChannels, numSamples), sampleRate (sampleRate)
{
    clear();
}

void Buffer::write (std::unique_ptr<juce::OutputStream> stream) const
{
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatWriter> writer;
    writer.reset (
        format.createWriterFor (stream.release(), sampleRate, static_cast<unsigned int> (getNumChannels()), 16, {}, 0));

    auto success = writer->writeFromAudioSampleBuffer (*this, 0, getNumSamples());

    if ( ! success )
        throw std::runtime_error ("Could not write buffer to stream");
}
void Buffer::operator+= (const Buffer& rhs)
{
    auto oldSamples = getNumSamples();
    auto old = *this;

    setSize (getNumChannels(), oldSamples + rhs.getNumSamples());

    for ( int i = 0; i < getNumChannels(); ++i )
    {
        copyFrom (i, 0, old, i, 0, old.getNumSamples());
        copyFrom (i, oldSamples, rhs, i, 0, rhs.getNumSamples());
    }
}
Buffer Buffer::read (std::unique_ptr<juce::InputStream> stream)
{
    juce::WavAudioFormat format;
    std::unique_ptr<juce::AudioFormatReader> reader;
    reader.reset (format.createReaderFor (stream.release(), false));

    Buffer buffer (static_cast<int> (reader->numChannels), static_cast<int> (reader->lengthInSamples),
                   reader->sampleRate);
    reader->read (&buffer, 0, buffer.getNumSamples(), 0, true, true);
    return buffer;
}
void Buffer::forEach (const std::function<void (int, int)>& function)
{
    for ( int channel = 0; channel < getNumChannels(); ++channel )
    {
        for ( int sample = 0; sample < getNumSamples(); ++sample )
        {
            function (channel, sample);
        }
    }
}
void Buffer::fill (float value)
{
    forEach ([&] (int channel, int sample) { setSample (channel, sample, value); });
}
double Buffer::getSampleRate() const
{
    return sampleRate;
}
void Buffer::setSampleRate (double newSampleRate)
{
    sampleRate = newSampleRate;
}
