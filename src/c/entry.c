#include "structs.h"

extern int test_entry(int a,int b);

int this_should_be_in_bss;
int this_should_be_in_data =0;

int test_entry(int a, int b)
{
	return a+b;
}
void update_position(player_position *obj)
{
	obj->x = obj->x+obj->vx;
	obj->y = obj->y+obj->vy;
	obj->vy = obj->vy+obj->gravity;
}
