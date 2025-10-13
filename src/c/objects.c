extern int test_entry(int,int);
extern void startup(void);

const char* test_string="Hello World";

int main(void)
{
	test_entry((int)(&test_string),10);
	return 0;
}

void startup(void)
{
	main();
}
