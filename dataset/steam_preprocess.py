import json
import argparse
import sys
import re
import numpy
import os
import math
import glob
from collections import defaultdict
import matplotlib
import matplotlib.pyplot as plt
import tqdm
import re

parser = argparse.ArgumentParser()
parser.add_argument('-i', required=True)
args = parser.parse_args()

tmp = (args.i).split(".")
output_filename = tmp[0] + "_preprocess.csv"
out_file = open(output_filename, "w")

obj_idx = 0
obj_mapping = {}
usr_idx = 0
usr_mapping = {}

with open(args.i) as fp:
    print("Reading from file", args.i)
    for ln in fp:
        user = None
        for seg in range(0, len(ln.split(':'))):
            if "steam_id" in ln.split(':')[seg]:
                user = ln.split(':')[seg + 1].split(',')[0]
                # out_file.write(user)
                if user not in usr_mapping:
                    usr_mapping[user] = usr_idx
                    usr_idx += 1
                user = usr_mapping[user]
            
            if "item_id" in ln.split(':')[seg]:
                obj = ln.split(':')[seg + 1].split(',')[0]
                # out_file.write(obj)
                if obj not in obj_mapping:
                    obj_mapping[obj] = obj_idx
                    obj_idx += 1
                obj = obj_mapping[obj]
                assert(user != None)
                out_file.write(str(user) + ',' + str(obj) + '\n')

        # if len(usr_mapping) >= 3:
        #     break

        # s = ln.split("categories",1)[1]
        # if s.find("[") >= 0:
        #     user = (re.sub("[^0-9]", "", ln.split("gPlusUserId",1)[1]))
        #     if user not in usr_mapping:
        #         usr_mapping[user] = usr_idx
        #         usr_idx += 1
        #     user = usr_mapping[user]

        #     start = s.find("[") + len("[")
        #     end = s.find("]")
        #     substring = s[start:end]
        #     locations = substring.split(",")

        # # print(ln.split("categories",1)[1])
        #     for obj in locations:
                # if obj not in obj_mapping:
                #     obj_mapping[obj] = obj_idx
                #     obj_idx += 1
                # obj = obj_mapping[obj]

                # out_file.write(str(user) + ',' + str(obj) + '\n')


# review_data = None

# f = open(args.i)

# review_data = json.load(f)

# print(len(review_data))

# # for row in my_data["rows"]:
# #     do_something(row["text"])

# f.close()
