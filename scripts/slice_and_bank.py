import os
import sys
import os.path as path
import argparse
from pathlib import Path

blockSize = 8192

def split_file_and_generate_map(inputName,outputName,pageIndex,blockSize):
	inputFile = open(inputName,"rb")
	outputFile = open(outputName,"w")

	noExtension = os.path.splitext(outputName)[0]

	fileSize = os.path.getsize(inputName)

	remainingSize = fileSize
	index = 1
	while remainingSize>0:
		outName=f'{noExtension}.{index}'
		segmentOut = open(outName,'wb')
		writeSize = remainingSize
		if remainingSize>blockSize:
			writeSize = blockSize
		segmentData = inputFile.read(writeSize)
		segmentOut.write(segmentData)
		outputFile.write( f'    SECTION PAGE_{pageIndex}\n')
		outputFile.write( f'    incbin "{outName}"\n')
		remainingSize -= writeSize
		index += 1
		pageIndex += 1
		segmentOut.close()
	inputFile.close()
	outputFile.close()

# Given an input file, slice it in to 16KB sections and generate the required assembly to include it.
parser = argparse.ArgumentParser()

parser.add_argument('input_file', type=str, help='Processed (ELF format file)')
parser.add_argument('output_file',type=str, help='Output filename')
parser.add_argument('start_page',type=int, help='Page number to start at')
args=parser.parse_args()

split_file_and_generate_map(args.input_file,args.output_file,args.start_page,blockSize)


