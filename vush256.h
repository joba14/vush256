
/**
 * @file vush256.h
 * 
 * @copyright This file is a part of the "vush256" project and is distributed,
 * and licensed under "MIT" license.
 * 
 * @author joba14
 * 
 * @date 2024-05-19
 */

#ifndef __vush256_h__
#define __vush256_h__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef char char_t;
typedef bool bool_t;

#define vush256_string_length ((uint64_t)64)
#define vush256_blocks_count ((uint64_t)(vush256_string_length / 8))

typedef struct
{
	uint32_t blocks[vush256_blocks_count];
	char_t stringified[vush256_string_length + 1];
	bool_t valid;
} vush256_s;

static void _set_block(const uint64_t index, const uint32_t state, const uint32_t previous, vush256_s* const hash);

vush256_s vush256_make(const char_t* const input, const uint64_t length);

#endif  // __vush256_h__


#define __vush256_c__
#ifdef __vush256_c__

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define vush256_invalid ((vush256_s) { .blocks = {0}, .stringified = {0}, .valid = false, })

#define _vush256_primary1 ((uint64_t)31)
#define _vush256_primary2 ((uint64_t)2747636419)
#define _vush256_primary3 ((uint64_t)2654435769)

// note: these primary values were taken from the internet. I searched what constants are usually
// used in hashing algorithms, and learnt about prime numbers. First one I came up by myself, and
// the last two I borrowed from a forum. And they seem to be working with my algorithm just fine.

static void _set_block(const uint64_t index, const uint32_t state, const uint32_t previous, vush256_s* const hash)
{
	assert(hash != NULL);
	hash->blocks[index] += (state >> index) ^ previous;
	hash->blocks[index] ^= previous + index;
	hash->blocks[index] ^= _vush256_primary2;
	hash->blocks[index] *= _vush256_primary3 + index;
	hash->blocks[index] ^= hash->blocks[index] >> 16;
	hash->blocks[index] *= _vush256_primary3;
	hash->blocks[index] ^= hash->blocks[index] >> 16;
	hash->blocks[index] *= _vush256_primary3;
}

vush256_s vush256_make(const char_t* const input, const uint64_t length)
{
	if (NULL == input)
	{
		return vush256_invalid;
	}

	vush256_s hash = vush256_invalid;
	uint32_t state = 0;

	for (uint64_t index = 0; index < vush256_blocks_count; ++index)
	{
		_set_block(index, state, ((0 == index) ? state : hash.blocks[index - 1]), &hash);
	}

	state = 0;

	for (uint64_t index = 0; index < length; ++index)
	{
		const uint32_t value = *(uint32_t* const)(input + index);
		state = _vush256_primary1 * state - _vush256_primary2 * state + (state ^ _vush256_primary3) + value;

		for (uint64_t block_index = 0; block_index < vush256_blocks_count; ++block_index)
		{
			_set_block(block_index, state, ((0 == block_index) ? state : hash.blocks[block_index - 1]), &hash);
		}
	}

	for (uint64_t index = 0; index < vush256_blocks_count; ++index)
	{
		if (snprintf(hash.stringified + (index * vush256_blocks_count), vush256_blocks_count + 1, "%08x", hash.blocks[index]) <= 0)
		{
			return vush256_invalid;
		}
	}

	hash.stringified[vush256_string_length] = 0;
	hash.valid = true;
	return hash;
}

#endif
