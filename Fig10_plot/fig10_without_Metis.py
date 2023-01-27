import sys
import re
import numpy
import os
import math
import matplotlib
import matplotlib.pyplot as plt 
import glob
import argparse
from pylab import *
from scipy.stats.mstats import gmean

workloads = ["steam","anime","movie","dblp","office","twitch","sports","clothes","M1","M2","M3","M4"]
abbrev = ["stm","ani","mov","dblp","off","twit","spo","clo","M1","M2","M3","M4","GM"]

space_arr = []
merci_arr = []
grace_arr = []

for i in range(0, len(workloads)):
    workload = workloads[i]
    
    space_file_name = "../hbm_only_space_log/" + workload + "_1.0.log"
    f = open(space_file_name, "r")
    for line in f.readlines():
        if "Reduction Factor is:" in line:
            space_arr.append(1. / float(line.split()[-1]))
            
    merci_file_name = "../hbm_only_merci_log/" + workload + "_patoh_hbm_1.0.log"
    f = open(merci_file_name, "r")
    for line in f.readlines():
        if "Reduction Factor is:" in line:
            merci_arr.append(1. / float(line.split()[-1]))
            
    grace_files = glob.glob("../hbm_only_grace_log/" + workload + "*.log")
    assert(len(grace_files) == 1)   # For simplicity of the artifact, we only showcase grace plot with 1 parameter, \
                                    # this is not a hard limit and we encourage users to test more
                                                    
    grace_file_name = grace_files[0]
    f = open(grace_file_name, "r")
    for line in f.readlines():
        if "Reduction Factor is:" in line:
            grace_arr.append(1. / float(line.split()[-1]))

f = open("fig10.txt", "w")
for i in range(0, len(workloads)):
    f.write(abbrev[i] + '\t' + str(1) + '\t' + str(space_arr[i]) \
                 + '\t' + str(merci_arr[i]) + '\t' + str(grace_arr[i]) + '\t' + str(0.50) + '\n')  # Base is 1, oracle-of-2 is 0.50.

f.write('GM' + '\t' + str(1) + '\t' + str(gmean(space_arr)) \
                 + '\t' + str(gmean(merci_arr)) + '\t' + str(gmean(grace_arr)) + '\t' + str(0.50) + '\n')
f.close()
file = "fig10.txt"

WIN1 = []
WIN2 = []
WIN3 = []
WIN4 = []
WIN5 = []

kernels = []

with open(file) as fp:
    # print("Reading from file", file)
    for ln in fp:
        if "\n" in ln:
            ln = ln[:-1]
        ln_split = ln.split("\t")
        print(ln_split)
        kernels.append(ln_split[0])
        WIN1.append(float(ln_split[1]))
        WIN2.append(float(ln_split[2]))
        WIN3.append(float(ln_split[3]))
        WIN4.append(float(ln_split[4]))
        WIN5.append(float(ln_split[5]))

ind = numpy.arange(len(kernels))*1.4
# ind[-2] += 0.4
ind[-1] += 0.4
ax1 = plt.gca()
width = 0.20
p00 = plt.bar(ind, WIN1, width,  align='center',
              color='#fde0dd', edgecolor="black", label="No reduction")
p1 = plt.bar(ind+1*width, WIN2, width, align='center',
             color='#f768a1', edgecolor="black", label="SPACE")
p2 = plt.bar(ind+2*width, WIN3, width, align='center',
             color='#c51b8a', edgecolor="black", label="MERCI")
p3 = plt.bar(ind+3*width, WIN4, width, align='center',
             color='#b2df8a', edgecolor="black", label="GRACE")
p4 = plt.bar(ind+4*width, WIN5, width, align='center',
             color='#1a1313', edgecolor="black", label="Oracle-of-2")
             

graph_font_size = 35
font0 = matplotlib.font_manager.FontProperties()
font0.set_weight('bold')
plt.xticks(ind+2.5*width, kernels, fontsize=graph_font_size-3, rotation=0)

plt.text(ind[-1]-0.45*width, WIN1[-1]+0.025,
         "{:.2f}".format(WIN1[-1]), fontsize=graph_font_size-23,rotation=90)
plt.text(ind[-1]+0.65*width, WIN2[-1]+0.025,
         "{:.2f}".format(WIN2[-1]), fontsize=graph_font_size-23, rotation=90)
plt.text(ind[-1]+1.65*width, WIN3[-1]+0.025,
         "{:.2f}".format(WIN3[-1]), fontsize=graph_font_size-23, rotation=90)
plt.text(ind[-1]+2.65*width, WIN4[-1]+0.025,
         "{:.2f}".format(WIN4[-1]), fontsize=graph_font_size-23, rotation=90)
plt.text(ind[-1]+3.65*width, WIN5[-1]+0.025,
         "{:.2f}".format(WIN5[-1]), fontsize=graph_font_size-23, rotation=90)


data_split_place1 = (len(kernels) - 1) * 1.4 - width/2 + 0.1
plt.axvline(x=data_split_place1, color='black', linestyle='--')

plt.legend(fontsize=graph_font_size-3, ncol=3, loc="lower center", bbox_to_anchor=(0.5, 1.02))

ax1.yaxis.set_ticks(np.arange(0, 100, 2))
for i in range(0, 10):
    if i == 0:
        continue
    ax1.axhline(0+0.5*i, color='grey', linestyle='--')
ax1.set_ylim(0, 1.2)
for axis in [ax1.yaxis]:
    axis.set_major_locator(matplotlib.ticker.MaxNLocator(integer=True))
ax1.set_xlim(-0.5, len(kernels) * 1.4 + 0.4)

font0.set_size(graph_font_size)
plt.xlabel("Datasets", fontsize=graph_font_size, font_properties=font0)
plt.ylabel("Memory Traffic\nReduction (x)", fontsize=graph_font_size,
           font_properties=font0)
plt.yticks(fontsize=graph_font_size)
fig = matplotlib.pyplot.gcf()
fig.set_size_inches(15, 6, forward=True)
plt.tight_layout()
fig.savefig('fig10_without_Metis.png')
# plt.show()
