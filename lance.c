/* Lambda calculus implementation in plain C90*/

#include <stdio.h>
#include <stdlib.h>

#define fatal(...)                                                                                                     \
	do                                                                                                                 \
	{                                                                                                                  \
		fprintf(stderr, "ERROR! %s\n", __VA_ARGS__);                                                                   \
		exit(EXIT_FAILURE);                                                                                            \
	} while (0);

int main(int argc, char **argv)
{
	if (2 != argc)
	{
		fatal("No input file");
	}
	puts("Welcome to lance, a lambda calculus interpreter");
	printf("Now running %s...\n", argv[1]);

	return 0;
}
