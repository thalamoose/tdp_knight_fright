##
## You need to have some form of gcc installed, but not the native windows one - that doesn't do ELF format
## output, and can't be switched. Ideally some simple compiler, maybe from android.
##
import argparse
import sys

from elftools.elf.elffile import ELFFile

globalString = ''

verbose = True

DW_AT_NAME = 'DW_AT_name'
DW_AT_BYTE_SIZE = 'DW_AT_byte_size'
DW_AT_TYPE = 'DW_AT_type'
DW_AT_UPPER_BOUND='DW_AT_upper_bound'
DW_AT_COUNT='DW_AT_count'
DW_AT_DECL_FILE='DW_AT_decl_file'
DW_AT_DECL_LINE='DW_AT_decl_line'
DW_AT_DECL_COLUMN='DW_AT_decl_column'
DW_AT_SIBLING = 'DW_AT_sibling'
DW_TAG_BASE_TYPE = 'DW_TAG_base_type'
DW_TAG_TYPEDEF='DW_TAG_typedef'
DW_TAG_STRUCTURE_TYPE='DW_TAG_structure_type'
DW_TAG_ARRAY_TYPE='DW_TAG_array_type'
DW_TAG_POINTER_TYPE='DW_TAG_pointer_type'
DW_TAG_MEMBER='DW_TAG_member'
DW_TAG_SUBPROGRAM='DW_TAG_subprogram'
DW_TAG_FORMAL_PARAMETER = 'DW_TAG_formal_parameter'
DW_TAG_VARIABLE = 'DW_TAG_variable'
DW_TAG_COMPILE_UNIT='DW_TAG_compile_unit'
DW_TAG_UNSPECIFIED_PARAMETERS='DW_TAG_unspecified_parameters'
DW_TAG_LEXICAL_BLOCK='DW_TAG_lexical_block'
DW_TAG_CONST_TYPE='DW_TAG_const_type'

def error(errorText):
	raise(Exception(errorText))
	sys.exit(-1)
	   
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

def getFilename(object):
	return object.attributes[DW_AT_DECL_FILE].value

def getLinenumber(object):
	return object.attributes[DW_AT_DECL_LINE].value

def getType(CU, memberDef):
	if memberDef.tag==None:
		return "void"
	if memberDef.tag==DW_TAG_BASE_TYPE:
		return getBaseType(CU, memberDef)
	if memberDef.tag==DW_TAG_ARRAY_TYPE:
		return parseArray(CU, memberDef)
	if memberDef.tag==DW_TAG_TYPEDEF:
		typeDef = getReference(CU, memberDef)
		if typeDef.tag==DW_TAG_BASE_TYPE:
			return getBaseType(CU, typeDef)
		if typeDef.tag==DW_TAG_STRUCTURE_TYPE:
			return getName(CU, typeDef)
		error('ERROR: unknown type tag {typeDef.tag}')
	if memberDef.tag==DW_TAG_POINTER_TYPE:
		typeDef = getReference(CU, memberDef)
		if typeDef==None:
			return f'WORD ;; Pointer to void'
		typeStr = getType(CU, typeDef)
		return f'WORD ;; Pointer to {typeStr} array'
	if memberDef.tag==DW_TAG_STRUCTURE_TYPE:
		structDef = getSiblingReference(CU, memberDef)
		name=getName(CU, structDef)
		return name
	if memberDef.tag==DW_TAG_CONST_TYPE:
		typeDef = getReference(CU, memberDef)
		if typeDef==None:
			return "void"
		return getType(CU, typeDef)
	if memberDef.tag==DW_TAG_MEMBER:
		raise "This code should't be used"
	error(f'ERROR: Unknown member tag {memberDef.tag}')

def parseStruct(CU, prefix, structDef):
	if structDef==None:
		return "<<<<>>>>"
	if structDef.has_children:
		structFields = []
		for child in structDef.iter_children():
			if child.tag==DW_TAG_MEMBER:
				memberDef = getReference(CU, child)
				if memberDef==None:
					type = 'void'
				else:
					type = getType(CU, memberDef)
					if type==None:
						structFields += [';**WARNING** Unnamed structure. Consider not using\n;anonymous structs. It will display better','']
						if verbose: 
							print(f'WARNING: Unnamed structure')
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
		return '<BASETYPE>'

def getEntryCount(type):
	count = 0
	if type.has_children:
		for child in type.iter_children():
			if DW_AT_UPPER_BOUND in child.attributes:
				count += child.attributes[DW_AT_UPPER_BOUND].value+1
			elif DW_AT_COUNT in child.attributes:
				count += child.attributes[DW_AT_COUNT].value
	else:
		error('ERROR: Unable to determine entry count')
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
	elif typeDef.tag==DW_TAG_CONST_TYPE:
		constTypeDef = getReference(CU, typeDef)
		constTypeName = getType(CU, constTypeDef)
		return f'WORD ;; Pointer to {constTypeName}'

	error( 'ERROR: unknown tag {typeDef.tag}')


def getName(CU, type):
	if DW_AT_NAME in type.attributes:
		return type.attributes[DW_AT_NAME].value.decode('ASCII')
	return None
	
def parseBaseSize(baseType):
	if baseType.tag=='DW_DIE_TAG_structure_type':
		error('ERROR: unknown tag {typeDef.tag}')
	byteSize = baseType.attributes[DW_AT_BYTE_SIZE].value
	if byteSize==1:
		return '1'
	if byteSize==2:
		return '2'
	if byteSize==4:
		return '4'
	return f'BLOCK {byteSize}'

def getBaseType(CU, baseType):
	if baseType.tag==DW_TAG_TYPEDEF:
		typeRef = getReference(CU, baseType)
		typeName = getName(CU, typeRef)
		return f'{typeName}'
		error(f'ERROR: unknown tag {typeDef.tag}')
	if DW_AT_BYTE_SIZE in baseType.attributes:
		byteSize = baseType.attributes[DW_AT_BYTE_SIZE].value
		if byteSize==1:		
			return 'BYTE'
		if byteSize==2:	
			return 'WORD'
		if byteSize==4:
			return 'DWORD'
		return f'BLOCK {byteSize}'
	return '***HERE***void'

existingStructs=[]
def handleStruct(CU, structRef, outFile):
	name = getName(CU, structRef)
	if name!=None:
		if name in existingStructs:
			if verbose: print(f'Skipped {name}...')
			return
		existingStructs.append(name)
		structFields = parseStruct(CU, '', structRef)
		if structFields:
			if verbose:	
				print(f'    STRUCT {name}')
			outFile.write(f'    STRUCT {name}\n')
				 
			for textLine in structFields:
				if len(textLine):
					if verbose:	print(textLine)
					outFile.write(textLine+'\n')
			if verbose:	print(f'    ENDS ;; {name}\n')
			outFile.write(f'    ENDS ;; {name}\n\n')

existingMethods=[]

def handleSubprogram(CU, progRef, outFile):
	progName = getName(CU, progRef)
	if progRef.has_children:
		typeRef = getReference(CU, progRef)
		if (typeRef==None):
			returnType = 'void'
		else:
			returnType = getType(CU, typeRef)
		if verbose: print(f';; Returns {returnType}')
		outFile.write(f';; Returns {returnType}\n')
		if progName in existingMethods:
			return
		existingMethods.append(progName)
		print(existingMethods)
		stackFrameName = f'{progName}_stack_frame'
		paramIndex = 1
		if verbose: 
			print(f'    STRUCT {stackFrameName}')
			print(f'__previous_frame_pointer__          		WORD ;; Previous frame pointer\n')
			print(f'__return_address__							WORD ;; Return address\n')
		outFile.write(f'    STRUCT {stackFrameName}\n')
		outFile.write(f'__previous_frame_pointer__          		WORD ;; Previous frame pointer\n')
		outFile.write(f'__return_address__							WORD ;; Return address\n')
		for child in progRef.iter_children():
			if child.tag==DW_TAG_FORMAL_PARAMETER:
				childName = getName(CU, child)
				if childName==None:
					childName=f'param_{paramIndex}'
					paramIndex+=1
				paramType = getReference(CU, child)
				if paramType:
					typeStr = getType(CU, paramType)
				else:
					typeStr = '<void>'
				if verbose: print(f'{childName:<40}    {typeStr}')
				outFile.write(f'{childName:<40}    {typeStr}\n')
			elif child.tag==DW_TAG_VARIABLE:
				varName = getName(CU, child)
				typeRef = getReference(CU, child)
				if typeRef==None:
					raise "typeRef is None"
					typeStr = '****?????*****'
				else:
					typeStr = getType(CU, typeRef)
				if verbose: print(f'{varName:<40}    {typeStr}')
				outFile.write(f'{varName:<40}    {typeStr}\n')
			elif child.tag==DW_TAG_UNSPECIFIED_PARAMETERS:
				if verbose: print(f'*UNSPECIFIED FORMAL PARAMETERS')
			elif child.tag==DW_TAG_LEXICAL_BLOCK:
				continue
			else: 
				error(f'Unhandled tag {child.tag}')
		outFile.write(f'    ENDS ;; {progName}\n\n')
		if verbose: print(f'    ENDS ;; {progName}\n')
	return


def iterateThroughCompileUnit(CU,outFile):
	ignored_tags=[DW_TAG_FORMAL_PARAMETER, DW_TAG_BASE_TYPE, DW_TAG_VARIABLE]
	ignored_tags+=[DW_TAG_COMPILE_UNIT, DW_TAG_TYPEDEF, DW_TAG_MEMBER]
	ignored_tags+=[None]
	for DIE in CU.iter_DIEs():
		if DIE.tag==DW_TAG_STRUCTURE_TYPE and DIE.has_children:
			handleStruct(CU, DIE, outFile)
		elif DIE.tag==DW_TAG_SUBPROGRAM:
			handleSubprogram(CU, DIE, outFile)
	return

def create_structs_from_dwarf(input_file, outFile):
	global objline
	global globalString
	try:
		with open(input_file,'rb') as inFile:
			elfFile = ELFFile(inFile)
			if not elfFile.has_dwarf_info():
				print('WARNING: No dwarf info')
				return
			dwarfInfo = elfFile.get_dwarf_info()
			for CU in dwarfInfo.iter_CUs():
				iterateThroughCompileUnit(CU, outFile);
	except FileNotFoundError:
		error(f'ERROR: Unable to open input file {input_file}')
	except:
		raise
	return

def main(file_list, outputFile):
	outputFile.write('    SLDOPT COMMENT WPMEM, LOGPOINT, ASSERTION\n')
	outputFile.write('    DEVICE ZXSPECTRUMNEXT\n\n')
	for filename in file_list:
		print(f'Processing file {filename}...')
		create_structs_from_dwarf(filename, outputFile)
	return
		 

try:
	parser = argparse.ArgumentParser()

	parser.add_argument('filenames', nargs='+',type=str,help='List of input files')
	parser.add_argument('-output_file','-o', help='Output filename')
	args=parser.parse_args()
	if args.output_file=='':
		main(args.filenames, sys.stdout)
	else:
		with open(args.output_file,'w') as outputFile:
			main(args.filenames, outputFile)
except FileNotFoundError as ex:
	error(f'ERROR: Unable to open output file {args.output_file}')
except:
	raise
