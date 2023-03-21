#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum VARIANT
{
	IGNORE,
	HANDLER,
	MASK,
	PENDING,
	INVALID
} Variant;

Variant choose_variant(const char *variant);

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Too few arguments\n");
		return -1;
	}

	Variant variant = choose_variant(argv[1]);

	if (variant == INVALID)
	{
		printf("Invalid argument\n");
		return -1;
	}

	return 0;
}

Variant choose_variant(const char *variant)
{
	if (strcmp(variant, "ignore") == 0)
	{
		return IGNORE;
	}

	if (strcmp(variant, "handler") == 0)
	{
		return HANDLER;
	}

	if (strcmp(variant, "mask") == 0)
	{
		return MASK;
	}

	if (strcmp(variant, "pending") == 0)
	{
		return PENDING;
	}

	return INVALID;
}