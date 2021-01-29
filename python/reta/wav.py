import scipy.io.wavfile as wavfile
import scipy.signal as sps
import numpy


class Wav:

    def __init__(self, sample_rate, channels):
        self.sample_rate = sample_rate
        self.channels = channels

    @staticmethod
    def from_file(path):
        f = wavfile.read(path)

        channels = numpy.array(f[1], dtype=float).transpose()

        # mono wavfiles don't come in array of array
        if type(channels[0]) == numpy.float64:
            channels = numpy.array([channels])

        channels /= 2 ** 15

        return Wav(f[0], channels)

    def channel_count(self):
        return len(self.channels)

    def num_samples(self):
        if self.channel_count() == 0:
            return 0
        return len(self.channels[0])

    def resample(self, target_sample_rate):
        target_samples = self.num_samples() * target_sample_rate / self.sample_rate
        self.channels = sps.resample(
            self.channels, round(target_samples), axis=1)
        self.sample_rate = target_sample_rate

    def reshape(self, target_channels, target_samples):
        self.channels = numpy.resize(
            self.channels, (target_channels, target_samples))

    def consolidate_inplace(self, other):
        target_sample_rate = min(self.sample_rate, other.sample_rate)
        target_channels = min(self.channel_count(), other.channel_count())
        target_samples = min(self.num_samples(), other.num_samples())

        self.resample(target_sample_rate)
        self.reshape(target_channels, target_samples)

    @staticmethod
    def consolidate(lhs, rhs):

        new_lhs, new_rhs = lhs, rhs
        new_lhs.consolidate_inplace(rhs)
        new_rhs.consolidate_inplace(lhs)

        return new_lhs, new_rhs
