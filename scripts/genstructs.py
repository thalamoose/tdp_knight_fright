##
## You need to have some form of gcc installed, but not the native windows one - that doesn't do ELF format
## output, and can't be switched. Ideally some simple compiler, maybe from android.
##
import argparse

from elftools.elf.elffile import ELFFile

globalString = ''

verbose = False

DW_AT_NAME = 'DW_AT_name'
DW_AT_BYTE_SIZE = 'DW_AT_byte_size'
DW_AT_TYPE = 'DW_AT_type'
DW_AT_UPPER_BOUND='DW_AT_upper_bound'
DW_AT_SIBLING = 'DW_AT_sibling'
DW_TAG_BASE_TYPE = 'DW_TAG_base_type'
DW_TAG_TYPEDEF='DW_TAG_typedef'
DW_TAG_STRUCTURE_TYPE='DW_TAG_structure_type'
DW_TAG_ARRAY_TYPE='DW_TAG_array_type'
DW_TAG_POINTER_TYPE='DW_TAG_pointer_type'
DW_TAG_MEMBER='DW_TAG_member'


def getReference(CU, object):
	try:
		type = object.attributes[DW_AT_TYPE].value
		return CU.get_DIE_from_refaddr(type)
	except:
		## This means it's a void type, parent needs to handle that
		return None

def getSiblingReference(CU, object):
	sibling = object.attributes[DW_AT_SIBLING].value
	return CU.get_DIE_from_refaddr(sibling)

def parseMember(CU, memberDef):
	if memberDef.tag==DW_TAG_BASE_TYPE:
		return parseBaseType(memberDef)
	if memberDef.tag==DW_TAG_ARRAY_TYPE:
		return parseArray(CU, memberDef)
	if memberDef.tag==DW_TAG_TYPEDEF:
		typeDef = getReference(CU, memberDef)
		if typeDef.tag==DW_TAG_BASE_TYPE:
			return parseBaseType(typeDef)
		if typeDef.tag==DW_TAG_STRUCTURE_TYPE:
			return getName(CU, typeDef)
		raise('ERROR: unknown type tag {typeDef.tag}')
	if memberDef.tag==DW_TAG_POINTER_TYPE:
		typeDef = getReference(CU, memberDef)
		name='void'
		if typeDef!=None:
			name=getName(CU, typeDef)
		return f'WORD ;; Pointer to {name}'
	raise('ERROR: unknown member tag {memberDef.tag}')

def parseStruct(CU, prefix, structDef):
	if structDef.has_children:
		structFields = []
		for child in structDef.iter_children():
			if child.tag==DW_TAG_MEMBER:
				memberDef = getReference(CU, child)
				if memberDef==None:
					type = 'void'
				else:
					type = parseMember(CU, memberDef)
					if type==None:
						structFields += [';**WARNING** Unnamed structure. Consider not using\n;anonymous structs. It will display better','']
						childName = getName(CU, child)
						anonRef = getReference(CU, memberDef)
						structFields += parseStruct(CU, childName+'_', anonRef)
					else:
						name = prefix+getName(CU, child)
						if verbose:	print( f'{name:<40}    {type}')
						structFields.append(f'{name:<40}    {type}')
			elif child.tag==DW_TAG_BASE_TYPE:
				baseTypeDef = getReference(CU, getReference(child))
				base = parseBaseSize(baseTypeDef)
				if verbose:	print( f'base= {base}')
		return structFields
	if structDef.tag==DW_TAG_BASE_TYPE:
		return

def getEntryCount(type):
	count = 0
	if type.has_children:
		for child in type.iter_children():
			count = count+child.attributes[DW_AT_UPPER_BOUND].value+1
	else:
		raise('ERROR: Unable to determine entry count')
	return count

def getByteSize(type):
	return str(type.attributes[DW_AT_BYTE_SIZE].value)
	
def parseArray(CU, arrayDef):
	count = 0
	typeDef = getReference(CU, arrayDef)
	name = getName(CU, typeDef)
	count = getEntryCount(arrayDef)

	if typeDef.tag==DW_TAG_BASE_TYPE:
		objSize = getByteSize(typeDef)
		return f'BLOCK {objSize},{count} ;; Array of {name}[{count}]'
	elif typeDef.tag==DW_TAG_TYPEDEF:
		sibling = getReference(CU, typeDef)
		siblingSize = parseBaseSize(sibling)
		siblingName = getName(CU, sibling)
		if sibling.tag==DW_TAG_BASE_TYPE:
			return f'BLOCK {siblingSize},{count} ;; Array of {name}[{count}]'

		return f'BLOCK {siblingName},{count} ;; Array of {siblingName}[{count}]'
	raise( 'ERROR: unknown tag {typeDef.tag}')


def getName(CU, type):
	if DW_AT_NAME in type.attributes:
		return type.attributes[DW_AT_NAME].value.decode('ASCII')
	return None
	
def parseBaseSize(baseType):
	if baseType.tag=='DW_DIE_TAG_structure_type':
		raise( 'ERROR: unknown tag {typeDef.tag}')
	byteSize = baseType.attributes[DW_AT_BYTE_SIZE].value
	if byteSize==1:
		return '1'
	if byteSize==2:
		return '2'
	if byteSize==4:
		return '4'
	return f'BLOCK {byteSize}'

def parseBaseType(baseType):
	byteSize = baseType.attributes[DW_AT_BYTE_SIZE].value
	if byteSize==1:		
		return 'BYTE'
	if byteSize==2:	
		return 'WORD'
	if byteSize==4:
		return 'DWORD'
	return f'BLOCK {byteSize}'

def iterateThroughCompileUnit(CU,outFile):
	for DIE in CU.iter_DIEs():
		if DIE.tag==DW_TAG_STRUCTURE_TYPE and DIE.has_children:
			name = getName(CU, DIE)
			if name!=None:
				structFields = parseStruct(CU, '', DIE)
				if structFields:
					if verbose:	print(f'\n    STRUCT {name}')
					outFile.write(f'\n    STRUCT {name}\n')
					for textLine in structFields:
						if len(textLine):
							if verbose:	print(textLine)
							outFile.write(textLine+'\n')
					if verbose:	print(f'    ENDS ;; {name}')
					outFile.write(f'    ENDS ;; {name}\n')
	return

def create_structs_from_dwarf(input_file,output_file):
	global objline
	global globalString
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
				iterateThroughCompileUnit(CU, outFile);
	return

parser = argparse.ArgumentParser()

parser.add_argument('input_file', type=str, help='Processed (ELF format file)')
parser.add_argument('output_file',type=str, help='Output filename')
args=parser.parse_args()

create_structs_from_dwarf(args.input_file,args.output_file)


