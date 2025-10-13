
extern int test_entry(int a,int b);

int this_should_be_in_bss;
int this_should_be_in_data =0;
int test_entry(int a, int b)
{
	return a+b;
}
