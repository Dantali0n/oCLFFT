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
import matplotlib.cm as cm
import pandas as pd
import numpy as np
import statistics

def adj_lightness(color, amount=0.5):
    import matplotlib.colors as mc
    import colorsys
    try:
        c = mc.cnames[color]
    except:
        c = color
    c = colorsys.rgb_to_hls(*mc.to_rgb(c))
    return colorsys.hls_to_rgb(c[0], max(0, min(1, amount * c[1])), c[2])

x = [("16", "64", "256", "1024", "4096", "16384", "65536", "262144", "1048576")]

wreverr = []
wreverse = []
for f in x[0]:
    try:
        data = pd.read_csv("../measurements/fftw/fftw_dvb-t_{0}.csv".format(f))
        wreverr.append(statistics.stdev(data['fft'].values) / 1000000)
        wreverse.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
    except:
        print("File fftw_dvb-t_{0}.csv does not exist".format(f))

yreverr = []
yreverse = []
for f in x[0]:
    try:
        data = pd.read_csv("../measurements/bit-ocl/bit-ocl_dvb-t_{0}.csv".format(f))
        yreverr.append(statistics.stdev(data['fft'].values) / 1000000)
        yreverse.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
    except:
        print("File bit-ocl_dvb-t_{0}.csv does not exist".format(f))

zreverr = []
zreverse = []
for f in x[0]:
    try:
        data = pd.read_csv("../measurements/flk-ocl/flk-ocl_dvb-t_{0}.csv".format(f))
        zreverr.append(statistics.stdev(data['fft'].values) / 1000000)
        zreverse.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
    except:
        print("File flk-ocl_dvb-t_{0}.csv does not exist".format(f))

fig, ax = plt.subplots()
index = np.arange(9)
num_bars = 9
bar_width = 2 / num_bars
opacity = 0.95

colors = cm.rainbow(np.linspace(0, 1, 3))

err_size = bar_width/3

plt.grid(which='both', zorder=1)

_, caps, _ = plt.errorbar(index, wreverse, zreverr, err_size, fmt='.', color=adj_lightness(colors[0], 0.6),
     uplims=True, lolims=True, zorder=3)
for cap in caps:
    cap.set_marker('_')
plt.bar(index, wreverse, bar_width,
    alpha=opacity, color=colors[0], label='fftw fft (cpu)', zorder=2)

_, caps, _ = plt.errorbar(index + (bar_width * 1.1), yreverse, yreverr, err_size, fmt='.', color=adj_lightness(colors[1], 0.6),
     uplims=True, lolims=True, zorder=3)
for cap in caps:
    cap.set_marker('_')
plt.bar(index + (bar_width * 1.1), yreverse, bar_width,
    alpha=opacity, color=colors[1], label='bit-ocl fft (gpu)', zorder=2)

_, caps, _ = plt.errorbar(index + (bar_width * 2.2), zreverse, zreverr, err_size, fmt='.', color=adj_lightness(colors[2], 0.6),
     uplims=True, lolims=True, zorder=3)
for cap in caps:
    cap.set_marker('_')
plt.bar(index + (bar_width * 2.2), zreverse, bar_width,
    alpha=opacity, color=colors[2], label='flk-ocl fft (gpu)', zorder=2)

plt.xlabel('Input size')
plt.ylabel('Mean execution time in seconds')
plt.title('Performance of FFT without bit-reversal')
plt.xticks(index + ((1.1 / num_bars) * (num_bars / 5)), ("16", "64", "256", "1024", "4096", "16384", "65536", "262144", "1048576"))
plt.legend()

#ax.set_yscale('log')

plt.tight_layout()
plt.show()
