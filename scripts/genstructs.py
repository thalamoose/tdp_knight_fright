##
## You need to have some form of gcc installed, but not the native windows one - that doesn't do ELF format
## output, and can't be switched. Ideally some simple compiler, maybe from android.
##
## TODO: void*, or likely any pointer type doesn't show up
import sys
import argparse
from pathlib import Path

from elftools.elf.elffile import ELFFile

def parseMember(CU,memberDef,prefix):
	memberLine=''
	tag = memberDef.tag
	memberName = memberDef.attributes['DW_AT_name'].value.decode('ASCII')
	memberType = memberDef.attributes['DW_AT_type'].value
	memberDef = CU.get_DIE_from_refaddr(memberType)
	memberLine += parseBaseType(CU,memberDef,memberName,prefix)
	return memberLine

def parseStruct(CU,structDef,name,indent):
	objline = ''
	if structDef.has_children:
		for child in structDef.iter_children():
			objline += parseBaseType(CU,child,'',name)
	return objline

	
def parseTypedefStart(CU,typeDef,indent):
	name = typeDef.attributes['DW_AT_name'].value.decode('ASCII')
	tag = typeDef.tag
	objline=''
	type = typeDef.attributes['DW_AT_type'].value

	structDef = CU.get_DIE_from_refaddr(type)
	if structDef.tag=='DW_TAG_structure_type':
		objline += f'{indent}STRUCT {name}\n'
		objline += parseBaseType(CU,structDef,'','')
		objline += f'{indent}ENDS\n'
		
	return objline

def parseTypedef(CU,typeDef,indent):
	name = typeDef.attributes['DW_AT_name'].value.decode('ASCII')
	tag = typeDef.tag
	objline=''
	type = typeDef.attributes['DW_AT_type'].value

	structDef = CU.get_DIE_from_refaddr(type)
	objline += parseBaseType(CU,structDef,indent,'')
		
	return objline

def parseBaseType(CU,baseType,baseName,indent):
	try:
		name = baseType.attributes['DW_AT_name'].value.decode('ASCII')
	except:
		name = '<anonymous>'
	tag = baseType.tag
	typeName = ''
	if tag=='DW_TAG_base_type':
		byteSize = baseType.attributes['DW_AT_byte_size'].value
		formatted = f'{indent}{baseName}'
		typeName += f'{formatted:40}'
		if byteSize==1:
			typeName += 'BYTE\n'
		elif byteSize==2:
			typeName += 'WORD\n'
		elif byteSize==4:
			typeName += 'DWORD\n'
		else:
			typeName += f'BLOCK {byteSize}\n'
	elif tag=='DW_TAG_typedef':
		typeName += parseTypedef(CU,baseType,indent+baseName)
	elif tag=='DW_TAG_structure_type':
		typeName += parseStruct(CU,baseType,baseName,'.--->NOT HERE')
	elif tag=='DW_TAG_member':
		if indent!='':
			indent +='_'
		typeName += parseMember(CU,baseType,indent)
	return typeName

def create_structs_from_dwarf(input_file,output_file):
	global objline
	with open(output_file,'w') as outFile:
		outFile.write('    SLDOPT COMMENT WPMEM, LOGPOINT, ASSERTION\n')
		outFile.write('    DEVICE ZXSPECTRUMNEXT\n')

		with open(input_file,'rb') as inFile:
			elfFile = ELFFile(inFile)
			if not elfFile.has_dwarf_info():
				print('Error: No dwarf info')
				return
			
			dwarfInfo = elfFile.get_dwarf_info()
			for CU in dwarfInfo.iter_CUs():
				for DIE in CU.iter_DIEs():
					if DIE.tag=='DW_TAG_typedef':
						objline = parseTypedefStart(CU,DIE,'    ')
						outFile.write(objline)

parser = argparse.ArgumentParser()

parser.add_argument('input_file', type=str, help='Processed (ELF format file)')
parser.add_argument('output_file',type=str, help='Output filename')
args=parser.parse_args()

create_structs_from_dwarf(args.input_file,args.output_file)


