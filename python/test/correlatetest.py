import unittest

import numpy

from ..reta.wav import Wav
from ..reta import correlate
import pathlib


class CorrelateTestCase(unittest.TestCase):

    def setUp(self) -> None:
        stereo_path = pathlib.Path(__file__).parent.joinpath(
            pathlib.Path("assets/48000-Stereo.wav"))
        mono_path = pathlib.Path(__file__).parent.joinpath(
            pathlib.Path("assets/44100-Mono.wav"))
        self.stereo = Wav.from_file(stereo_path)
        self.mono = Wav.from_file(mono_path)

        self.mono.consolidate_inplace(self.stereo)
        self.stereo.consolidate_inplace(self.mono)

    def test_something(self):

      #  lhs = Wav(44100, numpy.ones(1024))
      #  rhs = Wav(44100, numpy.zeros(1024))
        lhs = self.mono
        rhs = self.stereo

        #correlate.correlate(lhs, rhs)
        correlate.cancelTime(lhs, rhs, False)
        #correlate.cancelFrequency(lhs, rhs)
        self.assertTrue(True)


if __name__ == '__main__':
    unittest.main()
