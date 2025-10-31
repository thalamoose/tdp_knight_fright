//#include "include/kftypes.h"
//#include "include/objects.h"

typedef unsigned char u8;

typedef struct s_coord
{
	short x;
	short y;
} t_coord;

typedef struct
{
	short x;
	short y;
} t_coord2;

typedef struct s_obj2
{
	struct {
		u8 active:1;
		u8 pingpong:1;
		u8 is4bit:1;
	} flags;
    char totalFrames;
	char direction;
	t_coord coordinate;	
	t_coord2 coord2;
} obj2;

void CopyPalette(void* palette, u8 paletteType);

#if 0


typedef struct s_object
{
	t_coord position;
} obj1;

typedef struct s_test_object
{
    u8 gravity;
	struct 
	{
		char active:1;
		char pingpong:1;
		char is4bit:1;
	} flags;

    char frameIndex;
	
} object;

short testFunc(short param)
{
	return param*2;
}
typedef short s16;
typedef struct s_test_coord
{
	short x;
	short y;
} test_coord;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct s_simple_struct
{
	u16   arr2[10];
	u16* pointer_to_u16;
	void* some_void_ptr;
} simple_struct;


short testFunction(short param1, short param2, char param3)
{
	short local1=func(param1)+func(param2)+func(param3);
	short local2=10;
	return local1+local2;
}

void voidTestFunction(void);
typedef struct s_struct_with_struct_array
{
	simple_struct pos[10];
} t_struct_with_struct_array;

typedef struct s_array
{
	simple_struct pos[10];
	short arr[10];
	u16   arr2[10];
} array;

typedef struct s_second_coord
{
	u16 array[10];
} t_second_coord;

typedef struct s_coord
{
	u16 x;
	u16 y;
} coord;



typedef struct struct_and_typedef
{
	u16 typedef_thisIsaTestU16;
	short typedef_dummy;
} t_test;

struct just_a_struct_no_typedef
{
	t_test no_typedef_a;
};

typedef struct s_array_test_with_named_struct
{
	t_test named_struct_array[10];
} array_test_with_named_struct;



typedef struct s_third_coord
{
	u16 this_is_u16_type_x;
	short variable_x;
	short variable_y;
} t_third_coord;


typedef struct s_test_2
{
	t_third_coord position;
	u16 short_value;
} t_test_2;

typedef struct s_array_2
{
	u16 arr[10];
	t_third_coord arrayTest[10];
} t_array_2;

typedef struct s_complex_struct
{
	coord position;
	coord* pointer_before_everything;
	u8 byte_value;
	u16 short_value;
	u8* pointer_to_u8;
	u16* pointer_to_u16;
	u8 byte_after_pointers;
	void* this_is_a_void_ptr;
	coord* pointer_after_everything;
} t_complex_struct;
#endif