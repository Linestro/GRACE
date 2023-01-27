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
args = parser.parse_args()

tmp = (args.i).split(".")
output_filename = tmp[0] + "_preprocess.csv"
out_file = open(output_filename, "w")

dict = defaultdict(lambda: 0, {})
obj_idx = 0
obj_mapping = {}
usr_idx = 0
usr_mapping = {}

with open(args.i) as fp:
    print("Reading from file", args.i)
    for ln in fp:
        # if (ln.split(',')[0]).isnumeric() == False:
        #     continue
        user = ln.split(' ')[0]
        if user not in usr_mapping:
            usr_mapping[user] = usr_idx
            usr_idx += 1
        user = usr_mapping[user]

        obj = ln.split(' ')[1]
        if obj not in obj_mapping:
            obj_mapping[obj] = obj_idx
            obj_idx += 1
        obj = obj_mapping[obj]

        out_file.write(str(user) + ',' + str(obj) + '\n')

        user = ln.split(' ')[1]
        if user not in usr_mapping:
            usr_mapping[user] = usr_idx
            usr_idx += 1
        user = usr_mapping[user]

        obj = ln.split(' ')[0]
        if obj not in obj_mapping:
            obj_mapping[obj] = obj_idx
            obj_idx += 1
        obj = obj_mapping[obj]

        out_file.write(str(user) + ',' + str(obj) + '\n')