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

x = [("16", "64", "256", "1024", "4096", "16384", "65536", "262144", "1048576")]

def adj_lightness(color, amount=0.5):
    import matplotlib.colors as mc
    import colorsys
    try:
        c = mc.cnames[color]
    except:
        c = color
    c = colorsys.rgb_to_hls(*mc.to_rgb(c))
    return colorsys.hls_to_rgb(c[0], max(0, min(1, amount * c[1])), c[2])

zhost = []
zwin = []
zreverse = []
zfft = []
zmag = []
zdevice = []

for f in x[0]:
    try:
        data = pd.read_csv("../measurements/ard-ocl/ard-ocl_dvb-t_{0}.csv".format(f))
        zhost.append(sum(data['host'].values) / len(data['host']) / 1000000)
        zwin.append(sum(data['window'].values) / len(data['window']) / 1000000)
        zreverse.append(sum(data['reverse'].values) / len(data['reverse']) / 1000000)
        zfft.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
        zmag.append(sum(data['magnitude'].values) / len(data['magnitude']) / 1000000)
        zdevice.append(sum(data['device'].values) / len(data['device']) / 1000000)
    except:
        print("File ard-ocl_dvb-t_{0}.csv does not exist".format(f))

whost = []
wwin = []
wreverse = []
wfft = []
wmag = []
wdevice = []

for f in x[0]:
    try:
        data = pd.read_csv("../measurements/bit-ocl/bit-ocl_dvb-t_{0}.csv".format(f))
        whost.append(sum(data['host'].values) / len(data['host']) / 1000000)
        wwin.append(sum(data['window'].values) / len(data['window']) / 1000000)
        wreverse.append(sum(data['reverse'].values) / len(data['reverse']) / 1000000)
        wfft.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
        wmag.append(sum(data['magnitude'].values) / len(data['magnitude']) / 1000000)
        wdevice.append(sum(data['device'].values) / len(data['device']) / 1000000)
    except:
        print("File bit-ocl_dvb-t_{0}.csv does not exist".format(f))

whost = [x/y for x, y in zip(zhost, whost)]
wwin = [x/y for x, y in zip(zwin, wwin)]
wreverse = [x/y for x, y in zip(zreverse, wreverse)]
wmag = [x/y for x, y in zip(zmag, wmag)]
wfft = [x/y for x, y in zip(zfft, wfft)]
wdevice = [x/y for x, y in zip(zdevice, wdevice)]

rhost = []
rwin = []
rreverse = []
rfft = []
rmag = []
rdevice = []

for f in x[0]:
    try:
        data = pd.read_csv("../measurements/flk-ocl/flk-ocl_dvb-t_{0}.csv".format(f))
        rhost.append(sum(data['host'].values) / len(data['host']) / 1000000)
        rwin.append(sum(data['window'].values) / len(data['window']) / 1000000)
        rreverse.append(sum(data['reverse'].values) / len(data['reverse']) / 1000000)
        rfft.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
        rmag.append(sum(data['magnitude'].values) / len(data['magnitude']) / 1000000)
        rdevice.append(sum(data['device'].values) / len(data['device']) / 1000000)
    except:
        print("File flk-ocl_dvb-t_{0}.csv does not exist".format(f))

rhost = [x/y for x, y in zip(zhost, rhost)]
rwin = [x/y for x, y in zip(zwin, rwin)]
rreverse = [x/y for x, y in zip(zreverse, rreverse)]
rmag = [x/y for x, y in zip(zmag, rmag)]
rfft = [x/y for x, y in zip(zfft, rfft)]
rdevice = [x/y for x, y in zip(zdevice, rdevice)]

fig, ax = plt.subplots()
index = np.arange(9)
num_bars = 4
bar_width = 1.5 / num_bars
opacity = 0.95

colors = cm.rainbow(np.linspace(0, 1, 2))

plt.grid(which='both', zorder=1)

plt.bar(index + (bar_width * 1.1), wreverse, bar_width,
        alpha=opacity, color=adj_lightness(colors[0], 1), label='reverse bit-ocl', zorder=2)
plt.bar(index + (bar_width * 1.1), wfft, bar_width,
        alpha=opacity, color=adj_lightness(colors[1], 1), label='fft bit-ocl', bottom=wreverse, zorder=2)

plt.bar(index + (bar_width * 2.2), rreverse, bar_width,
        alpha=opacity, color=adj_lightness(colors[0], 0.7), label='reverse flk-ocl', zorder=2)
plt.bar(index + (bar_width * 2.2), rfft, bar_width,
        alpha=opacity, color=adj_lightness(colors[1], 0.7), label='fft flk-ocl', bottom=rreverse, zorder=2)

plt.xlabel('Input size')
plt.ylabel('Speedup compared to first OpenCL implementation (ard-ocl)')
plt.title('Evaluation of implementations')
plt.xticks(index + ((2.45 / num_bars) * (num_bars / 4)), ("16", "64", "256", "1024", "4096", "16384", "65536", "262144", "1048576"))
plt.legend()

ax.set_yscale('log')
#ax.set_ylim(ymin=0, ymax=1000000)

plt.tight_layout()
plt.show()