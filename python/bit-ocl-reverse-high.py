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

x = [("16384", "65536", "262144", "1048576")]

wreverr = []
wreverse = []
for f in x[0]:
    try:
        data = pd.read_csv("../measurements/ard-seq/ard-seq-rev_dvb-t_{0}.csv".format(f))
        wreverr.append(statistics.stdev(data['reverse'].values) / 1000000)
        wreverse.append(sum(data['reverse'].values) / len(data['reverse']) / 1000000)
    except:
        print("File ard-seq-rev_dvb-t_{0}.csv does not exist".format(f))

yreverr = []
yreverse = []
for f in x[0]:
    try:
        data = pd.read_csv("../measurements/ard-ocl/ard-ocl_dvb-t_{0}.csv".format(f))
        yreverr.append(statistics.stdev(data['reverse'].values) / 1000000)
        yreverse.append(sum(data['reverse'].values) / len(data['reverse']) / 1000000)
    except:
        print("File ard-seq_dvb-t_{0}.csv does not exist".format(f))

zreverr = []
zreverse = []
for f in x[0]:
    try:
        data = pd.read_csv("../measurements/bit-ocl/bit-ocl_dvb-t_{0}.csv".format(f))
        zreverr.append(statistics.stdev(data['reverse'].values) / 1000000)
        zreverse.append(sum(data['reverse'].values) / len(data['reverse']) / 1000000)
    except:
        print("File ard-seq_jorn_{0}.csv does not exist".format(f))

fig, ax = plt.subplots()
index = np.arange(4)
num_bars = 4
bar_width = 1 / num_bars
opacity = 0.8

colors = cm.rainbow(np.linspace(0, 1, 3))

plt.grid(which='both', zorder=1)

#plt.errorbar(index, wreverse, zreverr, bar_width/4, fmt='.', color=adj_lightness(colors[0], 0.8))
plt.bar(index, wreverse, bar_width,
    alpha=opacity, color=colors[0], label='seq-reverse (cpu)', zorder=2)

#plt.errorbar(index + (bar_width * 1.1), yreverse, yreverr, bar_width/4, fmt='.', color=adj_lightness(colors[1], 0.8))
plt.bar(index + (bar_width * 1.1), yreverse, bar_width,
    alpha=opacity, color=colors[1], label='seq-reverse (gpu)', zorder=2)

#plt.errorbar(index + (bar_width * 2.2), zreverse, zreverr, bar_width/4, fmt='.', color=adj_lightness(colors[2], 0.8))
plt.bar(index + (bar_width * 2.2), zreverse, bar_width,
    alpha=opacity, color=colors[2], label='ocl-reverse (gpu)', zorder=2)

plt.xlabel('Input size')
plt.ylabel('Mean execution time in seconds')
plt.title('Performance of 2D bit-reverse')
plt.xticks(index + ((1.1 / num_bars) * (num_bars / 4)), ("16384", "65536", "262144", "1048576"))
plt.legend()

ax.set_yscale('log')
#ax.set_ylim(ymin=0, ymax=0.25)

plt.tight_layout()
plt.show()