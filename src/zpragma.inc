
#pragma output REGISTER_SP = 0xbe00


//My zpragma.inc file contains this:


//// Real IM2
#pragma output REGISTER_SP = 0xbe00
#pragma output CRT_ORG_CODE = 0x8000

//// Disable interrupts at start and at exit
#pragma output CRT_ENABLE_EIDI = 0x11

//// On exit, HALT (no more play nice with basic)
////#pragma output CRT_ON_EXIT = 0x10001
//#pragma output CRT_ON_EXIT = 0

//// do not create a heap (no malloc)
#pragma output CLIB_MALLOC_HEAP_SIZE = 0

//// BAlloc tables
#pragma output CLIB_BALLOC_TABLE_SIZE = 0		// INCREASE if MEMPOOL increases!!!

//// do not create a heap for stdio (cannot open files)
#pragma output CLIB_STDIO_HEAP_SIZE = 0

# Append custom banks (defined in mmap.inc)
#pragma output CRT_APPEND_MMAP = 1

#pragma output CRT_ORG_PAGE_16 = 0x0100
#pragma output CRT_ORG_PAGE_18 = 0x4000


