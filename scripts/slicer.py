import sys
import argparse
import PIL
from PIL import Image

def main(args):

	try:
		image = PIL.Image.open(args.input_file)
	except FileNotFoundError:
		print( "ERROR: Unable to open",args.input_file)
		sys.exit(-1)
	except EOFError:
		print( "ERROR: Unable to parse",args.input_file)
		sys.exit(-1)
	except PIL.UnidentifiedImageError:
		print( "ERROR: Cannot identify image type")
		sys.exit(-1)

	palette_file = args.palette
	
	crop_args=[int(n) for n in args.crop.split(',')]
	if len(crop_args)!=4:
		print( "ERROR: Invalid crop arguments, use --crop=x,y,w,h")
		sys.exit(-1)
	crop_x,crop_y,crop_width,crop_height=crop_args
	if crop_x<0: crop_x=0
	if crop_y<0: crop_y=0
	if crop_width<0: crop_width=image.width
	if crop_height<0: crop_height=image.height
	if image.mode not in ["P","PA"]:
		print("Unhandled image mode")
		sys.exit(-1)
	cropped_image=image.crop((crop_x,crop_y,crop_x+crop_width,crop_y+crop_height))

	if args.tiled:
		block_width=8
		block_height=8
		max_colors=16
	elif args.sprites:
		block_width=16
		block_height=16
		max_colors=256
	else:
		block_width=cropped_image.width
		block_height=cropped_image.height
		max_colors=256

	group_args=[int(n) for n in args.size.split(',')]
	if len(group_args)!=2:
		print("ERROR: invalid size for each combined tile/sprite, use --size=w,h")
		sys.exit(-1)
	group_width,group_height = group_args
	if group_width==-1:
		group_width=cropped_image.width
	if group_height==-1:
		group_height=cropped_image.height

	palette=cropped_image.getcolors()
	if len(palette)>max_colors:
		print("ERROR: Palette is too large. Expected",max_colors," got ",len(tile_colors))
		sys.exit(-1)

	try:
		outfile = open(args.output_file, "wb")
	except FileNotFoundError:
		print( "ERROR: Unable to open output file", args.output_file)
		sys.exit(-1)

	for ypos in range(0,cropped_image.height,group_height):
		for xpos in range(0,cropped_image.width,group_width):
			tile_group=cropped_image.crop((xpos,ypos,xpos+group_width,ypos+group_height))
			for yoffset in range(0,group_height,block_height):
				for xoffset in range(0,group_width,block_width):
					tile=tile_group.crop((xoffset,yoffset,xoffset+block_width,yoffset+block_height))
					tile_data = tile.getdata()
					raw_bytes=bytes(tile_data)
					if max_colors>16:
						outfile.write(raw_bytes)
					else:
						dst = bytearray()
						for i in range(0,len(raw_bytes),2):
							outpixel=((raw_bytes[i]&0x0f)<<4)|(raw_bytes[i+1]&0x0f)
							dst.append(outpixel)
						outfile.write(bytes(dst))

	if args.palette!=None:
		palette_file = open(args.palette,"wb")
		tile_palette=cropped_image.getpalette()
		output_palette = bytearray()
		index = 0
		for c in range(0,len(palette)):
			r = tile_palette[index+0]>>5
			g = tile_palette[index+1]>>5
			b = tile_palette[index+2]>>5
			target_color = (r<<6)|(g<<3)|b
			output_palette.append((target_color>>1)&0xff)
			output_palette.append(target_color&1)
			index = index+3;
		for i in range(len(palette),max_colors):
			output_palette.append(0);
			output_palette.append(0);
		palette_file.write(bytes(output_palette))

parser = argparse.ArgumentParser()
parser.add_argument('--crop', dest='crop', type=str, default='-1,-1,-1,-1', help='crop to x,y,width,height')
parser.add_argument('--sprite',dest='sprites',action='store_true',help='Assume 16x16 blocks, 8 bit palette')
parser.add_argument('--tile',dest='tiled',action='store_true',help='Assume 8x8 blocks, 4 bit palette')
parser.add_argument('--size',dest='size', type=str, default='-1,-1', help='Size of a tile/sprite group. For tiles, this defines a block with the same 16 color palette')
parser.add_argument('--palette', dest='palette', type=str, help='name of palette file')
parser.add_argument('input_file', type=str, help='Input filename')
parser.add_argument('output_file',type=str, help='Output filename')
args=parser.parse_args()

main(args)
