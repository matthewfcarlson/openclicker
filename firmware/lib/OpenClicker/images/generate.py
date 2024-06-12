from io import TextIOWrapper
import sys
import os
import datetime
import enum
import json
import glob
from PIL import Image
from pathlib import Path

def GenerateImageHeaderFile(fp, image_path: Path):
    # write the header file
    fp.write(f"// This file was generated by generate.py on {datetime.datetime.now()}\n")
    fp.write("\n")
    fp.write('#include "graphics.h"\n')
    fp.write("\n")
    fp.write("#pragma once\n")
    fp.write("\n")
    im = Image.open(image_path).convert("RGBA")
    pixels = []
    width, height = im.size
    image_tag = image_path.stem.replace("-", "_")
    struct_name = f"image_{image_tag}_raw"
    # convert the image into 5,6,5 RGB format with 5 bits for red, 6 bits for green, and 5 bits for blue
    for y in range(height):
        for x in range(width):
            r, g, b, a = im.getpixel((x, y))
            if a < 128:
                pixels.append(0x120)
                continue
            if a < 250:
                r = int(r * a / 255)
                g = int(g * a / 255)
                b = int(b * a / 255)
                # print(f"r: {r}, g: {g}, b: {b}, a: {a /255}")
            color = (r >> 3) << 11 | (g >> 2) << 5 | (b >> 3)
            pixels.append(color)
            
            # raise NotImplementedError(f"r: {r}, g: {g}, b: {b}, a: {a}")
            # r = r >> 3
            # g = g >> 2
            # b = b >> 3
            # pixel = (r << 11) | (g << 5) | b
            # fp.write(f"{pixel}, ")
    fp.write(f"const unsigned short {struct_name}[{len(pixels)}] PROGMEM = {{\n")
    for i, pixel in enumerate(pixels):
        if i !=0 and i % 16 == 0:
            fp.write(f"// row {int(i / 16)}, {i} pixels \n")
        fp.write(f"{pixel:#06x}")
        if i != len(pixels) - 1:
            fp.write(",")
    fp.write("\n};\n\n")
    fp.write(f"const RemoteGraphic_t Graphics_{image_tag} = {{\n")
    fp.write(f"    .tag = \"{image_tag[0:16]}\",\n")
    fp.write(f"    .data = (const uint8_t*){struct_name},\n")
    fp.write(f"    .width = {width},\n")
    fp.write(f"    .height = {height},\n")
    fp.write(f"}};\n")


def main():
    # first we need to find the path of the folder this file is in
    # then we need to find the path of the folder above this folder
    root_dir = Path(__file__).resolve().parent.parent
    image_dir = root_dir / "images"
    # get the path to presenter/src/common/Protocol.ts relative to the root dir
    image_header_dir = root_dir / "include" / "images"

    # use glob to scan the root dir for any file that is a png, jpg, webp, or bmp
    image_files = glob.glob(str(image_dir / "*.*"), recursive=True)
    for image_file in image_files:
        if image_file.endswith(".py"):
            continue
        image_file_path = Path(image_file)
        # now create a file that has the same filename ends in .h and is in image_header_path folder
        image_header_path = image_header_dir / image_file_path.with_suffix(".h").name
        image_header_path.unlink(missing_ok=True)
        with open(image_header_path, "w") as fp:
            GenerateImageHeaderFile(fp, image_file_path)

    return 0


if __name__ == "__main__":
    sys.exit(main())