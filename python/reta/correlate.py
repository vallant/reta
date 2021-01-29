import scipy.signal as scs
import scipy.fftpack
from .wav import Wav
import matplotlib.pyplot as plt
import numpy


def correlate(lhs: Wav, rhs: Wav):
    reference = scs.correlate(lhs.channels[0], lhs.channels[0])
    corr = scs.correlate(lhs.channels[0], rhs.channels[0])

    diff = reference - corr

    plt.plot(reference, label="auto corr")
    plt.plot(corr, label="cross corr")
    plt.plot(diff, label="diff")
    plt.legend()
    plt.show()


def cancelTime(lhs: Wav, rhs: Wav, show: bool = True):
    diff = lhs.channels - rhs.channels

    plt.plot(lhs.channels[0], label="lhs")
    plt.plot(rhs.channels[0], label="rhs")
    plt.plot(diff[0], label="diff")
    plt.legend()
    if show:
        plt.show()


def cancelFrequency(lhs: Wav, rhs: Wav):
    N = 600
    T = 1.0 / 800.0
    lhs_f = scipy.fftpack.fft(lhs.channels[0])
    rhs_f = scipy.fftpack.fft(rhs.channels[0])
    diff_f = lhs_f - rhs_f

    xf = numpy.linspace(0.0, 1.0 / (2.0 * T), N // 2)

    plt.plot(xf, 2.0 / N * numpy.abs(lhs_f[:N // 2]), label="lhs")
    plt.plot(xf, 2.0 / N * numpy.abs(rhs_f[:N // 2]), label="rhs")
    plt.plot(xf, 2.0 / N * numpy.abs(diff_f[:N // 2]), label="diff freq")
    plt.legend()
    plt.show()
