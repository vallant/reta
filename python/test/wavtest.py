import unittest
from ..reta import wav
import pathlib
import numpy


class WavTestCase(unittest.TestCase):

    def setUp(self) -> None:
        stereo_path = pathlib.Path(__file__).parent.joinpath(
            pathlib.Path("assets/48000-Stereo.wav"))
        mono_path = pathlib.Path(__file__).parent.joinpath(
            pathlib.Path("assets/44100-Mono.wav"))
        self.stereo = wav.Wav.from_file(stereo_path)
        self.mono = wav.Wav.from_file(mono_path)

    def test_inexistent_file_raises_exception(self):
        with self.assertRaises(FileNotFoundError):
            wav.Wav.from_file("foo")

    def test_existing_file_has_correct_channel_count_sample_rate(self):
        self.assertEqual(48000, self.stereo.sample_rate)
        self.assertEqual(2, self.stereo.channel_count())

        self.assertEqual(44100, self.mono.sample_rate)
        self.assertEqual(1, self.mono.channel_count())

    def test_num_samples_works(self):
        self.assertEqual(3200, self.stereo.num_samples())
        self.assertEqual(3200, self.mono.num_samples())

    def test_consolidation_works(self):
        lhs, rhs = wav.Wav.consolidate(self.stereo, self.mono)

        self.assertEqual(lhs.sample_rate, 44100)
        self.assertEqual(rhs.sample_rate, 44100)

        self.assertEqual(lhs.channel_count(), 1)
        self.assertEqual(rhs.channel_count(), 1)

    def test_samples_are_normalized(self):
        sample_max = numpy.max(self.mono.channels)
        sample_min = numpy.min(self.mono.channels)

        self.assertLessEqual(sample_max, 1.0)
        self.assertGreaterEqual(sample_min, -1.0)

        sample_max = numpy.max(self.stereo.channels)
        sample_min = numpy.min(self.stereo.channels)

        self.assertLessEqual(sample_max, 1.0)
        self.assertGreaterEqual(sample_min, -1.0)


if __name__ == '__main__':
    unittest.main()
