#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

//////////////////////////////////////////////
/////////////      byte      /////////////////
//////////////////////////////////////////////

typedef unsigned char	t_byte;

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

typedef struct s_byte_string
{
	t_byte	*bytes;
	size_t	size;
}		t_byte_string;

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

/**
 * @brief Cast a bytestring to a type of arbitary bit size, and then get the value of an item in
 * the casted bytestring.
 * 
 * @details Get the last n bit of the byte. Then get the value of the whole bytes that the cast type
 * covers entirely. Finally get the first n bit of a byte. In this way, it can return the value of
 * a type that is e.g. 6-bits, but stored in an 8-bits array.
*/
unsigned int	byte_string_casted_value(t_byte_string *byte_string, size_t item_bit_size, size_t index)
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

/**
 * @brief Converts a string to a byte.
*/
t_byte	crypto_string_to_byte(const char *string, size_t amount, const char *set, size_t base)
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

/**
 * @brief Converts a hexidecimal string to a byte string.
 * @details hex is 16 based, base64 is 64 based. Which means there
 * is 4 hex digits for each base64 digit.
*/
t_byte_string	crypto_byte_string_from_hex(const char *string)
{
	t_byte_string	hex_string = {NULL, 0};
	size_t			string_len = strlen(string);
	{
		hex_string = (t_byte_string){
			.bytes = malloc((string_len / 2) + ((string_len % 2) != 0) * sizeof(t_byte)),
			.size = string_len / 2 + ((string_len % 2) != 0)
		};
		if (!hex_string.bytes)
			return ((t_byte_string) {NULL, 0});
	}
	for (size_t i = 0; i < string_len; i += 2)
	{
		hex_string.bytes[i / 2] = crypto_string_to_byte(&string[i], 2, "0123456789abcdef", 16);
	}
	return hex_string;
}

char	*crypto_base64_encode(t_byte_string byte_string)
{
	static char 	b64[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	char			*base64_string;

	size_t			remainder = byte_string.size % 3;
	size_t			base64_string_len = ((byte_string.size / 3) + (remainder > 0)) * 4;

	base64_string = malloc(base64_string_len + 1);
	if (!base64_string)
		return (NULL);
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
		index = byte_string_casted_value(&byte_string, 6, i);
		assert(index <= 64);
		base64_string[i] = b64[index];
	}
	return base64_string;
}

int main(int argc, char **argv)
{
	if (argc != 2)
		return EXIT_FAILURE;
	t_byte_string	hex_string = crypto_byte_string_from_hex(argv[1]);

	if (!hex_string.bytes)
		return EXIT_FAILURE;
	// byte_string_print_as_binary(hex_string);
	// byte_string_print_as_string(hex_string);
	// fflush(stdout);
	{
		char			*base64_encoded_string = crypto_base64_encode(hex_string);

		if (!base64_encoded_string)
		{
			free(hex_string.bytes);
			return EXIT_FAILURE;
		}
		printf("%s\n", base64_encoded_string);
		free(base64_encoded_string);
	}
	free(hex_string.bytes);
	return EXIT_SUCCESS;
}
