import sys
import argparse
from PIL import Image
import numpy as np

def create_char_map(texture_file, target_file, output_file, tile_size=8):
    """
    Create a character map from target_file using tiles from texture_file.
    Writes the result into output_file in assembler db format.
    """
    # === LOAD IMAGES ===
    texture_img = Image.open(texture_file).convert("RGB")
    target_img  = Image.open(target_file).convert("RGB")

    # === SLICE TEXTURE INTO TILES ===
    tex_w, tex_h = texture_img.size
    tiles = []
    for y in range(0, tex_h, tile_size):
        for x in range(0, tex_w, tile_size):
            tile = texture_img.crop((x, y, x+tile_size, y+tile_size))
            tiles.append(np.array(tile))

    # === FUNCTION TO MATCH A PATCH AGAINST TILES ===
    def match_tile(patch):
        arr = np.array(patch)
        min_diff = float("inf")
        idx = 0
        for i, t in enumerate(tiles):
            diff = np.sum(np.abs(arr.astype(int) - t.astype(int)))
            if diff < min_diff:
                min_diff = diff
                idx = i + 1   # numbering starts at 1, 0 = blank
        return idx

    # === SCAN TARGET IMAGE INTO TILEMAP ===
    tw, th = target_img.size
    cols = tw // tile_size
    rows = th // tile_size

    char_map = np.zeros((rows, cols), dtype=int)

    for gy in range(rows):
        for gx in range(cols):
            patch = target_img.crop((gx*tile_size, gy*tile_size,
                                     (gx+1)*tile_size, (gy+1)*tile_size))
            arr = np.array(patch)
            # If patch is fully black → blank (0)
            if np.all(arr == [0,0,0]):
                char_map[gy, gx] = 0
            else:
                char_map[gy, gx] = match_tile(patch)

    # === OUTPUT TO ASM FILE ===
    with open(output_file, "w") as f:
        f.write(f"    db {cols},{rows}\n")
        for row in char_map:
            row_str = ", ".join(f"{val:2}" for val in row)
            f.write(f"    db {row_str}\n")

    print(f"Character map saved to {output_file}")
    
parser = argparse.ArgumentParser()

parser.add_argument('--crop', dest='crop', type=str, default='-1,-1,-1,-1', help='crop to x,y,width,height')
parser.add_argument('--palette', dest='palette', type=str, help='name of palette file')

parser.add_argument('reference_file', type=str, help='Source tiles')
parser.add_argument('input_file', type=str, help='File to be tiled')
parser.add_argument('output_file',type=str, help='Output filename')
args=parser.parse_args()

create_char_map (args.reference_file, args.input_file,args.output_file)
