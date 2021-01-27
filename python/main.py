import matplotlib.pyplot as plt
from scipy.signal import correlate
import argparse
import reta.wav as wav

parser = argparse.ArgumentParser(description="reta")
parser.add_argument("lhs", help="Path to the first .wav")
parser.add_argument("rhs", help="Path to the second .wav")
args = parser.parse_args()


lhs = wav.Wav.from_file(args.lhs)
rhs = wav.Wav.from_file(args.rhs)

lhs.consolidate_inplace(rhs)
rhs.consolidate_inplace(lhs)

corr = correlate(lhs.channels, rhs.channels).flatten()
diff = (lhs.channels - rhs.channels).flatten()

plt.plot(corr)
plt.show()

plt.plot(diff)
plt.show()
