from PIL import Image
import numpy as np
from argparse import ArgumentParser
import os

parser = ArgumentParser()
# get inputs
parser.add_argument("-i", type=str)
args = parser.parse_args()
img = np.array(Image.open(args.i))

o_matrix = ""
for y, row in enumerate(img):
    for x,value in enumerate(row):
        if value.mean() > 255./2:
            o_matrix += '.'
        else:
            o_matrix += 'o'
    o_matrix += '\n'
    
print(o_matrix)
with open(args.i.replace(".png",".txt"), 'w') as out_file:
    out_file.write(o_matrix)