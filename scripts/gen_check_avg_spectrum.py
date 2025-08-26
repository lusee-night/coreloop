import numpy as np
import matplotlib.pyplot as plt
from icecream import ic

# Constants
NUM_GROUPS = 16
NUM_SPECTRA = 16
SPECTRUM_LEN = 2048
INT32_MAX = np.iinfo(np.int32).max

# Data container
data = np.zeros((NUM_GROUPS, NUM_SPECTRA, SPECTRUM_LEN), dtype=np.int32)

rng = np.random.default_rng(seed=42)  # reproducible

# Spectrum 1-4: only positive numbers
for s in range(4):
    data[:, s, :] = rng.integers(1, 1000000, size=(NUM_GROUPS, SPECTRUM_LEN), dtype=np.int64)

# Spectrum 5: special pattern
# for g in range(NUM_GROUPS):
    # indices 0-99: high positive
data[:, 4, 0:10] = INT32_MAX - rng.integers(0, 1000, size=10)
    # indices 100-199: high negative
data[:, 4, 10:20] = -INT32_MAX + rng.integers(0, 1000, size=10)
    # indices 200-299: alternate sign
# sign = 1 if g % 2 == 0 else -1
a1 = rng.integers(0, 1000, size=10)
a2 = rng.integers(0, 1000, size=10)

data[::2, 4, 20:30] = a1
data[1::2, 4, 20:30] = -a2
    # rest random
data[:, 4, 30:] = rng.integers(-1000000, 1000000, size=SPECTRUM_LEN - 30)

a = 31 * np.iinfo(np.int32).max // 32
vals = np.array([ a + i * np.iinfo(np.int32).max // 512 for i in range(NUM_GROUPS) ], dtype=np.int32)
data[:, 4, 0] = vals

a = -31 * np.iinfo(np.int32).max // 32
vals = np.array([ a + i * np.iinfo(np.int32).max // 256 for i in range(NUM_GROUPS) ], dtype=np.int32)
data[:, 4, 1] = vals

a = np.iinfo(np.int32).max // 32
vals = np.array([ a + i * np.iinfo(np.int32).max // 512 for i in range(NUM_GROUPS) ], dtype=np.int32)
data[:, 4, 2] = vals

a = -np.iinfo(np.int32).max // 32
vals = np.array([ a - i * np.iinfo(np.int32).max // 512 for i in range(NUM_GROUPS) ], dtype=np.int32)
data[:, 4, 3] = vals

a = 62 * np.iinfo(np.int32).max // 64
vals = np.array([ a + i * np.iinfo(np.int32).max // 512 for i in range(NUM_GROUPS) ], dtype=np.int32)
data[:, 4, 4] = vals

a = -61 * np.iinfo(np.int32).max // 64
vals = np.array([ a + i * np.iinfo(np.int32).max // 256 for i in range(NUM_GROUPS) ], dtype=np.int32)
data[:, 4, 5] = vals

a = np.iinfo(np.int32).max // 64
vals = np.array([ a + i * np.iinfo(np.int32).max // 2048 for i in range(NUM_GROUPS) ], dtype=np.int32)
data[:, 4, 6] = vals

a = -np.iinfo(np.int32).max // 64
vals = np.array([ a - i * np.iinfo(np.int32).max // 2048 for i in range(NUM_GROUPS) ], dtype=np.int32)
data[:, 4, 7] = vals


# Spectrum 6: small values
# for g in range(NUM_GROUPS):
    # 0–99: small, sum positive over groups
vals_pos = rng.integers(-5000, 5000, size=10)
# if g == 0:
#     vals_pos += 1000  # bias first group positive
data[:, 5, 0:10] = vals_pos + 1000

# 100–199: small, sum negative over groups
vals_neg = rng.integers(-5000, 5000, size=10)
# if g == 0:
#     vals_neg -= 1000  # bias first group negative
data[:, 5, 10:20] = vals_neg - 1000

# rest: small values
data[:, 5, 20:] = rng.integers(-100, 100, size=SPECTRUM_LEN - 20)

# Spectrum 7: one large negative spike in one group
# for g in range(NUM_GROUPS):
data[:, 6, :] = rng.integers(0, 10000, size=SPECTRUM_LEN)
data[0, 6, 500] = -INT32_MAX

# Spectrum 8: one large positive spike
# for g in range(NUM_GROUPS):
data[:, 7, :] = rng.integers(-10000, 0, size=SPECTRUM_LEN)
data[1, 7, 1500] = INT32_MAX

# Spectrum 9–16: mostly random patterns
for s in range(8, NUM_SPECTRA):
    data[:, s, :] = rng.integers(-500000, 500000, size=(NUM_GROUPS, SPECTRUM_LEN))



# Save to file
with open("check_averaging_spectrum.dat", "w") as f:
    for g in range(NUM_GROUPS):
        for s in range(NUM_SPECTRA):
            f.write(" ".join(map(str, data[g, s])) + " ")
    # remove trailing space if needed

# Plot averages for visual check
# fig, axes = plt.subplots(4, 4, figsize=(15, 10))
# axes = axes.flatten()
# for s in range(NUM_SPECTRA):
#     avg_spectrum = data[:, s, :20].mean(axis=0)
#     axes[s].plot(avg_spectrum)
#     axes[s].set_title(f"Spectrum {s+1}")
# plt.tight_layout()
# plt.savefig("averages_plot.png")
# plt.show()


for s in [4]:
    avg_spectrum_grob = np.sum((data[:, s, :8] // NUM_GROUPS).astype(np.int32), axis=0).astype(np.int32)
    avg_spectrum_int32 = data[:, s, :8].mean(axis=0).astype(np.int32)
    avg_spectrum_int64 = data[:, s, :8].astype(np.int64).mean(axis=0).astype(np.int32)
    avg_spectrum_float = data[:, s, :8].astype(np.float32).mean(axis=0).astype(np.int32)
    ic(avg_spectrum_grob)
    ic(avg_spectrum_int32)
    ic(avg_spectrum_int64)
    ic(avg_spectrum_float)
    # ic(data[:, s, :8])

