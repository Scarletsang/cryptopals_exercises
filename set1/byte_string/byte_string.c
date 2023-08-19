#include "byte_string.h"

#include <string.h>
#include <stdio.h>
#include <assert.h>

const char	*g_hex_set = "0123456789abcdef";
const char	*g_base64_set = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//////////////////////////////////////////////
/////////////      byte      /////////////////
//////////////////////////////////////////////

t_byte	byte_cast_from_start(t_byte byte, unsigned int amount)
{
	assert(amount <= 8);
	t_byte	mask = 0;

	for (size_t i = (8 - amount); i < 8; i++)
	{
		mask |= (0b1 << i);
	}
	return ((byte & mask) >> (8 - amount));
}

t_byte	byte_cast_from_last(t_byte byte, unsigned int amount)
{
	assert(amount <= 8);
	t_byte	mask = 0;

	while (amount > 0)
	{
		mask <<= 1;
		mask += 1;
		amount--;
	}
	return (byte & mask);
}

//////////////////////////////////////////////
//////////      byte string     //////////////
//////////////////////////////////////////////

t_byte_string	byte_string_create(size_t size, bool discard_after_encode)
{
	return ((t_byte_string){
		.bytes = malloc(size * sizeof(t_byte)),
		.size = size,
		.discard_after_encode = discard_after_encode
	});
}

void	byte_string_destroy(t_byte_string *byte_string)
{
	if (byte_string->bytes)
		free(byte_string->bytes);
	byte_string->bytes = NULL;
	byte_string->size = 0;
}

t_byte_string	byte_string_empty(void)
{
	return ((t_byte_string){
		.bytes = NULL,
		.size = 0,
		.discard_after_encode = false
	});
}

void	byte_string_move(t_byte_string *dest, t_byte_string *src)
{
	*dest = (t_byte_string) {
		.bytes = src->bytes,
		.size = src->size,
		.discard_after_encode = src->discard_after_encode
	};
	*src = byte_string_empty();
}

bool	byte_string_is_empty(t_byte_string byte_string)
{
	return (byte_string.bytes == NULL);
}

/**
 * @details Get the last n bit of the byte. Then get the value of the whole bytes that the cast type
 * covers entirely. Finally get the first n bit of a byte. In this way, it can return the value of
 * a type that is e.g. 6-bits, but stored in an 8-bits array.
*/
unsigned int	byte_string_casted_value(const t_byte_string *byte_string, size_t item_bit_size, size_t index)
{
	assert(item_bit_size <= 32);
	unsigned int	value = 0;

	size_t			start_bit = item_bit_size * index;
	size_t			end_bit = start_bit + item_bit_size;
	size_t			byte_index = start_bit / 8;
	size_t			remainder_bits = start_bit % 8;

	if (((byte_index < byte_string->size)) && (remainder_bits != 0))
	{
		value = byte_cast_from_last(byte_string->bytes[byte_index], 8 - remainder_bits);
		byte_index++;
	}
	for (int i = item_bit_size / 8; ((byte_index < byte_string->size)) && (i > 0); i--)
	{
		value <<= 8;
		value += byte_string->bytes[byte_index];
		byte_index++;
	}
	remainder_bits = end_bit % 8;
	if ((byte_index < byte_string->size) && (remainder_bits != 0))
	{
		value <<= remainder_bits;
		value += byte_cast_from_start(byte_string->bytes[byte_index], remainder_bits);
	}
	return value;
}

/////////////////////////////////////////////////////
//////////      byte string decoder    //////////////
/////////////////////////////////////////////////////

/**
 * @brief Converts a string to a byte.
*/
static t_byte	string_to_byte(const char *string, size_t amount, \
const char *set, size_t base)
{
	t_byte	byte = 0;
	char	*match;

	for (size_t i = 0; string[i] && (i < amount); i++)
	{
		byte *= base;
		match = strchr(set, string[i]);
		assert(match != NULL);
		byte += (match - set);
	}
	return byte;
}

static void	byte_to_string(t_byte byte, \
const char *set, size_t base, char *string, size_t amount)
{
	for (int i = amount - 1; i >= 0 ; i--)
	{
		string[i] = set[byte % base];
		byte /= base;
	}
}

t_byte_string	byte_string_decode_cstring(const char *string, bool discard_after_encode)
{
	size_t			string_len = strlen(string);
	t_byte_string	hex_string =  byte_string_create(string_len, discard_after_encode);

	if (byte_string_is_empty(hex_string))
		return (byte_string_empty());
	for (size_t i = 0; i < string_len; i ++)
	{
		hex_string.bytes[i] = string[i];
	}
	return hex_string;
}

/**
 * @brief Converts a hexidecimal string to a byte string.
 * @details hex is 16 based, base64 is 64 based. Which means there
 * is 4 hex digits for each base64 digit.
*/
t_byte_string	byte_string_decode_hex(const char *string, bool discard_after_encode)
{
	size_t			string_len = strlen(string);
	t_byte_string	hex_string =  byte_string_create(\
		string_len / 2 + ((string_len % 2) != 0), discard_after_encode);

	if (byte_string_is_empty(hex_string))
		return (byte_string_empty());
	for (size_t i = 0; i < string_len; i += 2)
	{
		hex_string.bytes[i / 2] = \
			string_to_byte(&string[i], 2, g_hex_set, 16);
	}
	return hex_string;
}

/////////////////////////////////////////////////////
//////////      byte string encoder    //////////////
/////////////////////////////////////////////////////

char	*byte_string_encode_hex(t_byte_string *byte_string)
{
	if (byte_string_is_empty(*byte_string))
		return NULL;
	char	*hex_string = malloc(byte_string->size * 2 + 1);

	if (!hex_string)
	{
		if (byte_string->discard_after_encode)
			byte_string_destroy(byte_string);
		return NULL;
	}
	hex_string[byte_string->size * 2] = 0;
	for (size_t i = 0; i < byte_string->size; i++)
	{
		byte_to_string(byte_string->bytes[i], g_hex_set, 16, &hex_string[i * 2], 2);
	}
	if (byte_string->discard_after_encode)
		byte_string_destroy(byte_string);
	return hex_string;
}

char	*byte_string_encode_base64(t_byte_string *byte_string)
{
	if (byte_string_is_empty(*byte_string))
		return NULL;
	char	*base64_string;

	size_t	remainder = byte_string->size % 3;
	size_t	base64_string_len = ((byte_string->size / 3) + (remainder > 0)) * 4;

	base64_string = malloc(base64_string_len + 1);
	if (!base64_string)
	{
		if (byte_string->discard_after_encode)
			byte_string_destroy(byte_string);
		return (NULL);
	}
	base64_string[base64_string_len] = 0;
	if (remainder == 1)
	{
		base64_string[base64_string_len - 1] = '=';
		base64_string[base64_string_len - 2] = '=';
		base64_string_len -= 2;
	}
	else if (remainder == 2)
	{
		base64_string[base64_string_len - 1] = '=';
		base64_string_len--;
	}
	unsigned int	index;
	for (size_t i = 0; i < base64_string_len; i++)
	{
		index = byte_string_casted_value(byte_string, 6, i);
		assert(index <= 64);
		base64_string[i] = g_base64_set[index];
	}
	if (byte_string->discard_after_encode)
		byte_string_destroy(byte_string);
	return base64_string;
}

/////////////////////////////////////////////////////
//////////      byte string printers    /////////////
/////////////////////////////////////////////////////

void	byte_string_print_as_binary(t_byte_string byte_string)
{
	for (size_t i = 0; i < byte_string.size; i++)
	{
		for (int j = 7; j >= 0; j--)
		{
			printf("%d", (byte_string.bytes[i] >> j) & 0x01);
		}
		printf("\n");
	}
}

void	byte_string_print_as_string(t_byte_string byte_string)
{
	for (size_t i = 0; i < byte_string.size; i++)
	{
		printf("%c", byte_string.bytes[i]);
	}
	printf("\n");
}
