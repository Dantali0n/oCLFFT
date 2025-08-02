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

yhost = []
ywin = []
yreverse = []
yfft = []
ymag = []
ydevice = []

for f in x[0]:
    try:
        data = pd.read_csv("../measurements/flk-ocl/flk-ocl_dvb-t_{0}.csv".format(f))
        yhost.append(sum(data['host'].values) / len(data['host']) / 1000000)
        ywin.append(sum(data['window'].values) / len(data['window']) / 1000000)
        yreverse.append(sum(data['reverse'].values) / len(data['reverse']) / 1000000)
        yfft.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
        ymag.append(sum(data['magnitude'].values) / len(data['magnitude']) / 1000000)
        ydevice.append(sum(data['device'].values) / len(data['device']) / 1000000)
    except:
        print("File bit-ocl_dvb-t_{0}.csv does not exist".format(f))

zwin = []
zfft = []
zmag = []

for f in x[0]:
    try:
        data = pd.read_csv("../measurements/fftw/fftw_dvb-t_{0}.csv".format(f))
    except:
        print("File fftw_dvb-t_{0}.csv does not exist".format(f))
    zwin.append(sum(data['window'].values) / len(data['window']) / 1000000)
    zfft.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
    zmag.append(sum(data['magnitude'].values) / len(data['magnitude']) / 1000000)

fig, ax = plt.subplots()
index = np.arange(9)
num_bars = 9
bar_width = 3.0 / num_bars
opacity = 0.8

colors = cm.rainbow(np.linspace(0, 1, 6))

current_bottom = 0
plt.bar(index, yhost, bar_width,
        alpha=opacity, color=colors[0], label='host-to-device-ocl')
plt.bar(index, ywin, bar_width,
        alpha=opacity, color=colors[1], label='window-ocl', bottom=yhost)
current_bottom = [x + y for x, y in zip(yhost, ywin)]
plt.bar(index, yreverse, bar_width,
        alpha=opacity, color=colors[2], label='reverse-ocl', bottom=current_bottom)
current_bottom = [x + y for x, y in zip(current_bottom, yreverse)]
plt.bar(index, yfft, bar_width,
        alpha=opacity, color=colors[3], label='fft-ocl', bottom=current_bottom)
current_bottom = [x + y for x, y in zip(current_bottom, yfft)]
plt.bar(index, ymag, bar_width,
        alpha=opacity, color=colors[4], label='magnitude-ocl', bottom=current_bottom)
current_bottom = [x + y for x, y in zip(current_bottom, ymag)]
plt.bar(index, ydevice, bar_width,
        alpha=opacity, color=colors[5], label='device-to-host-ocl', bottom=current_bottom)

current_bottom = 0
plt.bar(index + (bar_width * 1.1), zwin, bar_width,
        alpha=opacity, color=adj_lightness(colors[1], 0.6), label='window-fftw')
plt.bar(index + (bar_width * 1.1), zfft, bar_width,
        alpha=opacity, color=adj_lightness(colors[3], 0.6), label='fft-fftw', bottom=zwin)
current_bottom = [x + y for x, y in zip(zfft, zwin)]
plt.bar(index + (bar_width * 1.1), zmag, bar_width,
        alpha=opacity, color=adj_lightness(colors[4], 0.6), label='magnitude-fftw', bottom=current_bottom)

plt.xlabel('Input size')
plt.ylabel('Mean execution time in seconds')
plt.title('Evaluation of entire computation for Fast-Lookup FFT')
plt.axhline(y=0.0166, color='#000', label='16.66ms limit')
plt.xticks(index + ((1.6 / num_bars) * (num_bars / 9)), ("16", "64", "256", "1024", "4096", "16384", "65536", "262144", "1048576"))
plt.legend()

#ax.set_ylim(ymin=0, ymax=1000000)

plt.tight_layout()
plt.show()