import argparse
import sys
import re
import numpy
import os
import math
import glob
from collections import defaultdict
import pickle
import matplotlib
import matplotlib.pyplot as plt
import tqdm

parser = argparse.ArgumentParser()
parser.add_argument('-i', required=True)
parser.add_argument('-c', required=True)
args = parser.parse_args()

chop = int(args.c)
tmp = (args.i).split(".")
output_filename = tmp[0] + "_chop_" + str(chop) + ".cluster"
out_file = open(output_filename, "w")

size = 0

budget = None
if "anime" in args.i:
    budget = 11200

elif "steam" in args.i:
    budget = 10978

elif "movie" in args.i:
    budget = 26744

elif "flickr" in args.i:
    budget = 103631

elif "twitch" in args.i:
    budget = 739991

elif "alibaba" in args.i:
    budget = 846811

elif "clothes" in args.i:
    budget = 2681297


budget *= (1.1 * chop / 100)

with open(args.i) as fp:
    for ln in fp:
        # if (ln.split(',')[0]).isnumeric() == False:
        #     continue
        this_cluster = ln.split(' ')
        size += (2 ** (len(this_cluster) - 1) - 1)
        out_file.write(ln)
        if(size >= budget):
            assert(size > 0)
            print("Achieved size: ", size)
            break
