# Script to convert a .png to data that can be used
# with drawBitmap

import sys
import numpy as np
import matplotlib.image as mpimg 

if (len(sys.argv) != 2):
  print("Usage:", sys.argv[0], "<image to process>")
  exit()

black = [ 0., 0., 0., 1.]
white = [ 1., 1., 1., 1.]

def IsPixelOn(pixel):
  comparison = (pixel == black)
  return not comparison.all()

def TreatBlock(block):
  assert(block.shape[0] == 8)
  count = 0
  for i in range(block.shape[1]):
    if (count % 8 == 0):
      print("  ", end="")
    value = 0
    for j in range(8):
      set_value = IsPixelOn(block[j][i])
      if (set_value):
        value += 1<<(7-j)
    #print(hex(value)+", ", end="")
    print("0x%02x, " % value, end="")
    count = count + 1
    if (count % 8 == 0):
      print()
  if (count % 8 != 0):
    print() 


input_file = sys.argv[1]
print("Reading", input_file)
image = mpimg.imread(input_file)
v = image.shape[0]
h = image.shape[1]
print("Vertical size:", v)
print("Horizontal size:", h)
pixel_bytes = image.shape[2]
print("Values per pixel:", pixel_bytes)
if (pixel_bytes != 4):
  print("Number of bytes per pixel is not supported (=expected color png):", pixel_bytes)
  exit()

# Pad at top and bottom of image if the height is not a multiple of 8
if (v%8 != 0):
  startpad = (int)((8-v%8)/2)
  endpad = (8-v%8) - startpad
  print("Padding vertically:", startpad, "and", endpad)
  startpadding = np.full((startpad, h, 4), black)
  endpadding = np.full((endpad, h, 4), black)
  image = np.concatenate((startpadding, image, endpadding), axis=0)

# Create values
v = image.shape[0]
h = image.shape[1]
print("Treating", v, "x", h, "pixels")
number_blocks = (int)(v/8)

print();
print("static const PROGMEM uint8_t bitmap_data[] =")
print("{");
for i in range(number_blocks):
  block = image[i*8:(i+1)*8]
  print("  // block", i)
  TreatBlock(block)
print("};")
print("static const uint8_t bitmap_number_blocks = " + str(number_blocks) +";")
print("static const uint8_t bitmap_horizontal_size = " + str(h) + ";")


