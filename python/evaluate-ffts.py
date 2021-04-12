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
from scipy.interpolate import CubicSpline
import numpy as np

import fileinput

reference = []
target = []

mode = 0

for line in fileinput.input():
    line = line.rstrip()
    if "ref" in line:
        mode = 0
    elif "tar" in line:
        mode = 1
    elif "eof" in line:
        break
    elif mode is 0:
        try:
            reference.append(float(line))
        except:
            print(line)
    elif mode is 1:
        target.append(float(line))

x = [x for x in range(len(reference))]

fig = plt.figure()
ax = fig.add_subplot(1, 1, 1)

x = np.array(x, dtype=float)
reference = np.array(reference, dtype=float)
target = np.array(target, dtype=float)


offset = (max(reference) * 0.0)
target = [x + offset for x in target]

p1 = CubicSpline(x, reference)
p2 = CubicSpline(x, target)

# p1 = np.polynomial.chebyshev.chebfit(x, reference, 3)
# p2 = np.polynomial.chebyshev.chebfit(x, target, 3)

plt.title("Evaluate FFT",fontsize=16)
plt.xlabel('bin', fontsize=18)
plt.ylabel('amplitude', fontsize=16)
ax.plot(x, reference, 'ro',label="reference", color='red')
ax.plot(x, target, 'ro',label="target", color='blue')
ax.plot(x, p2(x), color='cyan') # plot first order polynomial
ax.plot(x, p1(x), color='pink') # plot first order polynomial
ax.legend(loc='upper left')
# ax.set_xscale('log')
#ax.set_yscale('log')
plt.show()