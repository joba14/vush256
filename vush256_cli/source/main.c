
/**
 * @file main.c
 * 
 * @copyright This file is a part of the "vush256" project and is distributed,
 * and licensed under "MIT" license.
 * 
 * @author joba14
 * 
 * @date 2024-05-19
 */

#define __vush256_c__
#include "vush256.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void usage(FILE* const stream, const char_t* const arg0);

static const char_t* shift(int32_t* const argc, const char_t*** const argv);

static bool_t hash_and_display(const char_t* const string, const uint64_t length, const char_t* const arg0);

int main(int32_t argc, const char_t** argv);

static void usage(FILE* const stream, const char_t* const arg0)
{
	assert(stream != NULL);
	assert(arg0 != NULL);

	(void)fprintf(
		stream,
		"usage: %s <command> <args>\n"
		"\n"
		"commands:\n"
		"    [ --text | -t ] <string>           provide a string to be hashed.\n"
		"    [ --file | -f ] <file-path>        provide file to be hashed.\n",
		arg0
	);
}

static const char_t* shift(int32_t* const argc, const char_t*** const argv)
{
	assert(argc != NULL);
	assert(argv != NULL);

	const char_t* current = NULL;

	if ((*argc > 0) && (*argv != NULL))
	{
		current = **argv;
		*argc -= 1;
		*argv += 1;
	}

	return current;
}

static bool_t hash_and_display(const char_t* const string, const uint64_t length, const char_t* const arg0)
{
	const vush256_s hash = vush256_make(string, length);

	if (!hash.valid)
	{
		(void)fprintf(stderr, "error: failed to make a valid hash!\n");
		usage(stderr, arg0);
		return false;
	}

	(void)fprintf(stdout, "%s\n", hash.stringified);
	return true;
}

int main(int32_t argc, const char_t** argv)
{
	const char_t* const arg0 = shift(&argc, &argv);

	if (argc <= 0)
	{
		(void)fprintf(stderr, "error: no command-line arguments were provided!\n");
		usage(stderr, arg0);
		return 1;
	}

	const char_t* text = NULL;
	const char_t* file = NULL;

	while (argc > 0)
	{
		const char_t* flag = shift(&argc, &argv);

		if (NULL == flag)
		{
			(void)fprintf(stderr, "error: encountered undefined flag!\n");
			usage(stderr, arg0);
			return 1;
		}

		if ((strcmp(flag, "--help") == 0) || (strcmp(flag, "-h") == 0))
		{
			usage(stdout, arg0);
			return 0;
		}
		else if ((strcmp(flag, "--text") == 0) || (strcmp(flag, "-t") == 0))
		{
			if (argc <= 0)
			{
				(void)fprintf(stderr, "error: failed to find value for flag: %s!\n", flag);
				usage(stderr, arg0);
				return 1;
			}

			flag = shift(&argc, &argv);

			if (NULL == flag)
			{
				(void)fprintf(stderr, "error: encountered undefined flag!\n");
				usage(stderr, arg0);
				return 1;
			}

			text = flag;
			break;
		}
		else if ((strcmp(flag, "--file") == 0) || (strcmp(flag, "-f") == 0))
		{
			if (argc <= 0)
			{
				(void)fprintf(stderr, "error: failed to find value for flag: %s!\n", flag);
				usage(stderr, arg0);
				return 1;
			}

			flag = shift(&argc, &argv);

			if (NULL == flag)
			{
				(void)fprintf(stderr, "error: encountered undefined flag!\n");
				usage(stderr, arg0);
				return 1;
			}

			file = flag;
			break;
		}
		else
		{
			(void)fprintf(stderr, "error: unknown command was provided: %s!\n", flag);
			usage(stderr, arg0);
			return 1;
		}
	}

	if ((NULL == text) && (NULL == file))
	{
		(void)fprintf(stderr, "error: neither text nor file was provided!\n");
		usage(stderr, arg0);
		return 1;
	}

	if (text != NULL)
	{
		const bool_t result = hash_and_display(text, strlen(text), arg0);
		return (int32_t)(!(result));
	}
	else if (file != NULL)
	{
		FILE* const stream = fopen(file, "r");

		if (NULL == stream)
		{
			(void)fprintf(stderr, "error: could not open the file %s!\n", file);
			usage(stderr, arg0);
			return 1;
		}

		(void)fseek(stream, 0, SEEK_END);
		const uint64_t fsize = (uint64_t)ftell(stream);
		(void)fseek(stream, 0, SEEK_SET);

		char_t* const content = malloc(fsize + 1);
		assert(content != NULL);

		(void)fread(content, fsize, 1, stream);
		(void)fclose(stream);

		content[fsize] = 0;
		const bool_t result = hash_and_display(content, fsize, arg0);
		free(content);
		return result;
	}

	return 0;
}
