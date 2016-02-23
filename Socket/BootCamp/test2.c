#include <stdio.h>
#include <unistd.h>

int main()
{
	//execlp("java", "java", "lottery", NULL);
	//system("java lottery");
	char *test = "hello world";
	printf("%s \n", test);
	printf("%c ", (*test)[1]);
	return 0;
}
