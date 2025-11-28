# -*- coding: utf-8 -*-

# oCLFFT
# Copyright (C) 2024 Corne Lukken

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

from mpmath import mp
import matplotlib.pyplot as plt
import copy
import numpy as np

mp.dps = 100

import fileinput

bins = []

for line in fileinput.input():
    line = line.rstrip()
    if "eof" in line:
        break
    try:
        bins.append(mp.mpf(line))
    except:
        print(line)

x = [x for x in range(len(bins))]

fig, axs = plt.subplots(2, 3)

x = np.array(x, dtype=float)
# bins = np.array(bins, dtype=mp.mpf)

subplot_num = -1
# axs[0][subplot_num].set_title("Raw values",fontsize=16)
# axs[0][subplot_num].set_xlabel('sample', fontsize=18)
# axs[0][subplot_num].set_ylabel('value', fontsize=16)
# axs[0][subplot_num].plot(x, bins, 'ro', color='red')
# axs[0][subplot_num].axhline(y=0.5, color='r', linestyle='-')
# axs[0][subplot_num].axhline(y=-0.5, color='r', linestyle='-')
# axs[0][subplot_num].legend(loc='upper left')

ranges = [
    {
        "start": -0.5,
        "stop": 0.5,
        "step": 0.005
    },
    {
        "start": -0.5,
        "stop": -0.49,
        "step": 0.00001
    },
    {
        "start": 0.49,
        "stop": 0.50,
        "step": 0.00001
    },
    {
        "start": -0.01,
        "stop": 0.01,
        "step": 0.00001
    },
    {
        "start": -0.26,
        "stop": -0.24,
        "step": 0.00001
    },
    {
        "start": 0.24,
        "stop": 0.26,
        "step": 0.00001
    },
]
for range in ranges:
    temp_bins = copy.deepcopy(bins)
    subplot_num += 1
    num_bins = np.arange(
        range['start'],
        range['stop'] + range['step'],
        step=range['step'], dtype=mp.mpf
    )
    binned = np.zeros(len(num_bins))

    temp_bins = [elem for elem in temp_bins if elem > range['start'] and elem < range['stop']]
    # outliers = []
    # for i, elem in enumerate(temp_bins):
    #     if elem < range['start']:
    #         outliers.append(i)
    #     elif elem > range['stop']:
    #         outliers.append(i)
    #
    # for i in outliers:
    #     del temp_bins[i]

    for i in temp_bins:
        try:
            binned[(int)((i - range['start']) / range['step'])] += 1
        except IndexError:
            print("Range: " + str(range['start']) + " Step: " + str(range['step']) + " I: " + mp.nstr(i, n=25))

    # for x in bins:
    #     if x < num_bins[0]:
    #         binned[0]+=1
    #
    # for i, elem in enumerate(num_bins[1:-1]):
    #     for x in bins:
    #         if x > elem and x < num_bins[i+2]:
    #             binned[i+1]+=1
    #
    # for x in bins:
    #     if x > num_bins[len(num_bins)-1]:
    #         binned[len(num_bins)-1]+=1

    axs[(int)(subplot_num / 3)][subplot_num % 3].set_title("Histogram", fontsize=16)
    axs[(int)(subplot_num / 3)][subplot_num % 3].set_xlabel('bin', fontsize=18)
    axs[(int)(subplot_num / 3)][subplot_num % 3].set_ylabel('count', fontsize=16)
    axs[(int)(subplot_num / 3)][subplot_num % 3].bar(num_bins[:-1], binned[:-1], width=np.diff(num_bins), edgecolor="black", align="edge")
    axs[(int)(subplot_num / 3)][subplot_num % 3].legend(loc='upper left')
    # axs[1].yaxis.set_major_formatter(mtick.FormatStrFormatter('%.2e'))

plt.show()