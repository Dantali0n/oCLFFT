# -*- coding: utf-8 -*-

# oCLFFT
# Copyright (C) 2025 Corne Lukken

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

file = "flk-ocl-fp32-5700xt-placement"
labels = ["1", "2", "4", "8", "32", "64", "128", "256"]
total = []

breakpoint()
for label in labels:
    for f in x[0]:
        try:
            data = pd.read_csv(f"../measurements//flk-ocl-fp32/{file}-{label}_{f}.csv")
            total.append(sum(data['total'].values) / len(data['total']))
        except:
            print(f"File {file}-{label}_{f}.csv does not exist")


results_1m=[total[0], total[2], total[4], total[6], total[8], total[10], total[12], total[14]]
results_4m=[total[1], total[3], total[5], total[7], total[9], total[11], total[13], total[15]]

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

plt.xlabel('Wavefront size')
plt.ylabel('Execution time')
plt.title('Evaluation of execution time in relation to wavefront size')
plt.xticks(ticks=index + bar_width * 0.55, labels=labels)
plt.legend()

ax.set_yscale('log')
#ax.set_ylim(ymin=0, ymax=1000000)

plt.tight_layout()
plt.show()