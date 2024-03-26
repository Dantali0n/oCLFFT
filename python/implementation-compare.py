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

x = [("1048576", "4194304",),]

def adj_lightness(color, amount=0.5):
    import matplotlib.colors as mc
    import colorsys
    try:
        c = mc.cnames[color]
    except:
        c = color
    c = colorsys.rgb_to_hls(*mc.to_rgb(c))
    return colorsys.hls_to_rgb(c[0], max(0, min(1, amount * c[1])), c[2])


file = "ard-seq-5900x"
ytotal = []
for f in x[0]:
    try:
        data = pd.read_csv(f"../measurements/ard-seq/{file}_{f}.csv")
        ytotal.append(sum(data['total'].values) / len(data['total']))
    except:
        print(f"File {file}_{f}.csv does not exist")


file = "fftw-5900x-1"
ftotal = []
for f in x[0]:
    try:
        data = pd.read_csv(f"../measurements/fftw/{file}_{f}.csv")
        ftotal.append(sum(data['total'].values) / len(data['total']))
    except:
        print(f"File {file}_{f}.csv does not exist")
ftotal = [x/y for x, y in zip(ytotal, ftotal)]

file = "fftw-5900x-24"
fftotal = []
for f in x[0]:
    try:
        data = pd.read_csv(f"../measurements/fftw/{file}_{f}.csv")
        fftotal.append(sum(data['total'].values) / len(data['total']))
    except:
        print(f"File {file}_{f}.csv does not exist")
fftotal = [x/y for x, y in zip(ytotal, fftotal)]


file = "ard-ocl-5700xt"
ztotal = []
for f in x[0]:
    try:
        data = pd.read_csv(f"../measurements/ard-ocl/{file}_{f}.csv")
        ztotal.append(sum(data['total'].values) / len(data['total']))
    except:
        print(f"File {file}_{f}.csv does not exist")
ztotal = [x/y for x, y in zip(ytotal, ztotal)]


file = "bit-ocl-5700xt"
wtotal = []
for f in x[0]:
    try:
        data = pd.read_csv(f"../measurements/bit-ocl/{file}_{f}.csv")
        wtotal.append(sum(data['total'].values) / len(data['total']))
    except:
        print(f"File {file}_{f}.csv does not exist")
wtotal = [x/y for x, y in zip(ytotal, wtotal)]


file = "flk-ocl-5700xt-256"
rtotal = []
for f in x[0]:
    try:
        data = pd.read_csv(f"../measurements/flk-ocl/{file}_{f}.csv")
        rtotal.append(sum(data['total'].values) / len(data['total']))
    except:
        print(f"File {file}_{f}.csv does not exist")
rtotal = [x/y for x, y in zip(ytotal, rtotal)]


file = "flk-ocl-fp32-5700xt-placement-256"
ttotal = []
for f in x[0]:
    try:
        data = pd.read_csv(f"../measurements//flk-ocl-fp32/{file}_{f}.csv")
        ttotal.append(sum(data['total'].values) / len(data['total']))
    except:
        print(f"File {file}_{f}.csv does not exist")
breakpoint()
ttotal = [x/y for x, y in zip(ytotal, ttotal)]

ytotal = [1 for x in ytotal]

labels = ["ard-seq", "fftw 1 core (measure)", "fftw 24 core (measure)", "ard-ocl", "bit-ocl", "flk-ocl", "flk-ocl-fp32-placement"]
results_1m=[ytotal[0], ftotal[0], fftotal[0], ztotal[0], wtotal[0], rtotal[0], ttotal[0]]
results_4m=[ytotal[1], ftotal[1], fftotal[1], ztotal[1], wtotal[1], rtotal[1], ttotal[1]]

fig, ax = plt.subplots()
index = np.arange(len(labels))
bar_width = .25
opacity = 0.95

colors = cm.rainbow(np.linspace(0, 1, 2))

plt.grid(which='both', zorder=1)
plt.bar(index, results_1m, bar_width,
        alpha=opacity, color=adj_lightness(colors[0], 1), label='1048576', zorder=2)

plt.bar(index + bar_width * 1.1, results_4m, bar_width,
        alpha=opacity, color=adj_lightness(colors[1], 1), label='4194304', zorder=2)

# plt.bar(index + (bar_width * 1.1), ztotal, bar_width,
#         alpha=opacity, color=adj_lightness(colors[1], 1), label='ard-ocl', zorder=2)

plt.xlabel('Implementation')
plt.ylabel('Speedup compared to first implementation (ard-seq)')
plt.title('Evaluation of implementations (AMD Ryzen 5900x + Radeon 5700XT)')
plt.xticks(ticks=index + bar_width * 0.55, labels=labels)
plt.legend()

ax.set_yscale('log')
#ax.set_ylim(ymin=0, ymax=1000000)

plt.tight_layout()
plt.show()