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
import pandas as pd
import numpy as np

x = [("32768", "131072", "524288", "2097152")]

ywin = []
yfft = []
ymag = []

for f in x[0]:
    try:
        data = pd.read_csv("../measurements/ard-seq/ard-seq_dvb-t_{0}.csv".format(f))
    except:
        print("File ard-seq_jorn_{0}.csv does not exist".format(f))
    ywin.append(sum(data['window'].values / len(data['window'])))
    yfft.append(sum(data['fft'].values / len(data['fft'])))
    ymag.append(sum(data['window'].values / len(data['window'])))

zwin = []
zfft = []
zmag = []

for f in x[0]:
    try:
        data = pd.read_csv("../measurements/fftw/fftw_dvb-t_{0}.csv".format(f))
    except:
        print("File ard-seq_jorn_{0}.csv does not exist".format(f))
    zwin.append(sum(data['window'].values / len(data['window'])))
    zfft.append(sum(data['fft'].values / len(data['fft'])))
    zmag.append(sum(data['window'].values / len(data['window'])))

fig, ax = plt.subplots()
index = np.arange(4)
num_bars = 4
bar_width = 0.9 / num_bars
opacity = 0.8

plt.bar(index, ywin, bar_width,
alpha=opacity, color='#cc0000', label='window-seq')
plt.bar(index, yfft, bar_width,
alpha=opacity, color='#00cc00', label='fft-seq', bottom=ywin)
plt.bar(index, ymag, bar_width,
alpha=opacity, color='#0000cc', label='magnitude-seq', bottom=yfft)

plt.bar(index + (bar_width * 1.1), zwin, bar_width,
alpha=opacity, color='#880000', label='window-fftw')
plt.bar(index + (bar_width * 1.1), zfft, bar_width,
alpha=opacity, color='#008800', label='fft-fftw', bottom=zwin)
plt.bar(index + (bar_width * 1.1), zmag, bar_width,
alpha=opacity, color='#000088', label='magnitude-fftw', bottom=zfft)

plt.xlabel('Input size')
plt.ylabel('Mean execution time in microseconds')
plt.title('Performance of basic OpenCL compared to sequential CPU')
plt.xticks(index + ((0.5 / num_bars) * (num_bars / 4)), ("32768", "131072", "524288", "2097152"))
plt.legend()

#ax.set_ylim(ymin=0, ymax=1000000)

plt.tight_layout()
plt.show()