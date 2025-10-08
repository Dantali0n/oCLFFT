# -*- coding: utf-8 -*-

# oCLFFT
# Copyright (C) 2021 Corne Lukken

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import matplotlib.pyplot as plt
import matplotlib.ticker as mtick
from scipy.interpolate import CubicSpline
import numpy as np

import fileinput

def l2_radix2_bound(N: int):
    eps = np.finfo(np.float32).eps           # 2.220446049250313e-16
    k   = int(np.log2(N))                    # 20 stages
    gamma = (k*eps) / (1 - k*eps)            # ≈ k*eps because k*eps ≈ 4.44e-15 << 1
    c = 1.0                                   # radix‑2 gives c≈1 (Higham)
    return  c * eps * np.log2(N)

reference = []
target = []

mode = 0

for line in fileinput.input():
    line = line.rstrip()
    if "fftw" in line:
        mode = 0
    elif "oclfft" in line:
        mode = 1
    elif "eof" in line:
        break
    elif mode == 0:
        try:
            reference.append(float(line))
        except:
            print(line)
    elif mode == 1:
        target.append(float(line))

x = [x for x in range(len(reference))]

fig, axs = plt.subplots(1, 2)

x = np.array(x, dtype=float)
reference = np.array(reference, dtype=float)
target = np.array(target, dtype=float)


offset = (max(reference) * 0.0)
target = [x + offset for x in target]

p1 = CubicSpline(x, reference)
p2 = CubicSpline(x, target)

# p1 = np.polynomial.chebyshev.chebfit(x, reference, 3)
# p2 = np.polynomial.chebyshev.chebfit(x, target, 3)

axs[0].set_title("Evaluate FFT",fontsize=16)
axs[0].set_xlabel('bin', fontsize=18)
axs[0].set_ylabel('amplitude', fontsize=16)
axs[0].plot(x, reference, 'ro', label="FFTW")
axs[0].plot(x, target, 'bo', label="oCLFFT")
axs[0].plot(x, p2(x), color='cyan') # plot first order polynomial
axs[0].plot(x, p1(x), color='pink') # plot first order polynomial
axs[0].legend(loc='upper left')
# axs[0].set_xscale('log')
#axs[0].set_yscale('log')

difference = [abs(j - target[i]) for i, j in enumerate(reference)]
error = [j - target[i] for i, j in enumerate(reference)]
l2_norm = np.linalg.norm(error) / np.linalg.norm(reference)

p3 = CubicSpline(x, difference)

axs[1].semilogy(x, difference, 'b.-')
axs[1].set_title('Error Spectrum (Difference)')
axs[1].set_xlabel('k')
axs[1].set_ylabel('|ΔX(k)|')
# axs[1].axhline(l2_radix2_bound(len(x)), 0, 1, color='red', linestyle='solid', label="cε log2(N)")
axs[1].text(0.02, 0.98, f"Upper bound cε log2(N) (radix2 fp32): {l2_radix2_bound(len(x))}", horizontalalignment='left', verticalalignment='top', transform=axs[1].transAxes)
axs[1].text(0.02, 0.95, f"Relative L2 error: {l2_norm:.4e}", horizontalalignment='left', verticalalignment='top', transform=axs[1].transAxes)
axs[1].text(0.02, 0.92, f"Max absolute error: {np.max(difference):.4e}", horizontalalignment='left', verticalalignment='top', transform=axs[1].transAxes)
axs[1].grid(True, which='both', ls=':')

plt.show()