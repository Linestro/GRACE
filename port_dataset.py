
from itertools import islice
import sys

# workload="dblp"
# workload="amazon_Office_Products"
# workload="amazon_Clothing_Shoes_and_Jewelry"
# workload="amazon_Sports_and_Outdoors"
workload=sys.argv[1]

# print(workload)

out_file = open(workload+"_postprocess.csv", "w")
user = 0

with open("./MERCI/data/4_filtered/"+workload+"/"+workload+"_train_filtered.txt") as fp:
    for ln in islice(fp,1,None):
        for i in range(0, int(ln.split(' ')[0])):
            for elt in ln.strip('\n').split(' ')[1:]:
                # print(elt)
                if elt.isnumeric():
                    out_file.write(str(user)+','+str(int(elt) - 1)+'\n')
        # exit()
        user += 1
    
with open("./MERCI/data/4_filtered/"+workload+"/"+workload+"_test_filtered.txt") as fp:
    for ln in islice(fp,1,None):
        for i in range(0, int(ln.split(' ')[0])):
            for elt in ln.strip('\n').split(' ')[1:]:
                if elt.isnumeric():
                    out_file.write(str(user)+','+str(int(elt) - 1)+'\n')
        user += 1