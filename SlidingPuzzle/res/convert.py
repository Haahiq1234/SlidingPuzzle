from PIL import Image
import os

files = os.listdir("jpgimages")
i = 1
for file in files:
    img = Image.open("jpgimages/"+ file)
    img.save("image_" + str(i) + ".png", "PNG")
    i += 1