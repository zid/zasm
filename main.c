#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "assemble.h"

int main(int argc, const char *argv[])
{
	int i, disassemble = 0;
	const char *filename = NULL;

	for(i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			if(argv[i][1] == 'd')
				disassemble = 1;
			else {
				fprintf(stderr, "Unknown command line option '%c'\n", argv[i][1]);
				return EXIT_FAILURE;
			}
		} else {
			if(filename)
			{
				fprintf(stderr, "Fatal: Filename given twicei\n");
				return EXIT_FAILURE;
			}
			filename = argv[i];
		}
	}

	if(!filename)
	{
		fprintf(stderr, "Fatal: No filename given\n");
		return EXIT_FAILURE;
	}

	if(disassemble)
	{

	} else {
		if(!assemble(filename))
			return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
