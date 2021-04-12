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

x = [("8", "128", "512",  "2048", "8192", "32768", "131072", "524288", "2097152")]

yhost = []
ywin = []
yreverse = []
yfft = []
ymag = []
ydevice = []

for f in x[0]:
    try:
        data = pd.read_csv("../measurements/ard-ocl/ard-ocl_dvb-t_{0}.csv".format(f))
        yhost.append(sum(data['host'].values) / len(data['host']) / 1000000)
        ywin.append(sum(data['window'].values) / len(data['window']) / 1000000)
        yreverse.append(sum(data['reverse'].values) / len(data['reverse']) / 1000000)
        yfft.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
        ymag.append(sum(data['magnitude'].values) / len(data['magnitude']) / 1000000)
        ydevice.append(sum(data['device'].values) / len(data['device']) / 1000000)
    except:
        print("File ard-seq_jorn_{0}.csv does not exist".format(f))

zwin = []
zfft = []
zmag = []

for f in x[0]:
    try:
        data = pd.read_csv("../measurements/fftw/fftw_dvb-t_{0}.csv".format(f))
    except:
        print("File ard-seq_jorn_{0}.csv does not exist".format(f))
    zwin.append(sum(data['window'].values) / len(data['window']) / 1000000)
    zfft.append(sum(data['fft'].values) / len(data['fft']) / 1000000)
    zmag.append(sum(data['magnitude'].values) / len(data['magnitude']) / 1000000)

fig, ax = plt.subplots()
index = np.arange(9)
num_bars = 9
bar_width = 3.9 / num_bars
opacity = 0.8

current_bottom = 0
plt.bar(index, yhost, bar_width,
        alpha=opacity, color='#cccc00', label='host-to-device-ocl')
plt.bar(index, ydevice, bar_width,
        alpha=opacity, color='#cc00cc', label='device-to-host-ocl', bottom=yhost)

plt.xlabel('Input size')
plt.ylabel('Mean execution time in seconds')
plt.title('Performance analysis of memory copies')
plt.xticks(index + ((0.05 / num_bars) * (num_bars / 9)), ("8", "128", "512",  "2048", "8192", "32768", "131072", "524288", "2097152"))
plt.legend()

#ax.set_ylim(ymin=0, ymax=1100

plt.tight_layout()
plt.show()