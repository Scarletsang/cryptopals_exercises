#include "xor.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#include "byte_string/byte_string.h"

const float	g_english_frequency[26] = {
    0.08167,
    0.01492,
    0.02782,
    0.04253,
    0.1270,
    0.02228,
    0.02015,
    0.06094,
    0.06966,
    0.00153,
    0.00772,
    0.04025,
    0.02406,
    0.06749,
    0.07507,
    0.01929,
    0.00095,
    0.05987,
    0.06327,
    0.09056,
    0.02758,
    0.00978,
    0.02360,
    0.00150,
    0.01974,
    0.00074
};

t_byte_string	xor_fixed(t_byte_string bstr1, t_byte_string bstr2, bool discard_after_encode)
{
	if (bstr1.size != bstr2.size)
		return byte_string_empty();
	t_byte_string	bstr_after_xor = byte_string_create(bstr1.size, discard_after_encode);

	if (byte_string_is_empty(bstr_after_xor))
		return byte_string_empty();
	for (size_t i = 0; i < bstr_after_xor.size; i++)
	{
		bstr_after_xor.bytes[i] = bstr2.bytes[i] ^ bstr1.bytes[i];
	}
	return bstr_after_xor;
}

t_byte_string	xor_repeat(t_byte_string target, t_byte_string repeating_key, bool discard_after_encode)
{
	t_byte_string	bstr_after_xor = byte_string_create(target.size, discard_after_encode);

	if (byte_string_is_empty(bstr_after_xor))
		return byte_string_empty();
	for (size_t i = 0; i < bstr_after_xor.size; i++)
	{		
		bstr_after_xor.bytes[i] = target.bytes[i] ^ repeating_key.bytes[i % repeating_key.size];
	}
	return bstr_after_xor;
}

float	xor_score(t_byte_string *bytestring, char *winner)
{
	float	score = INFINITY;
	float	final_score = INFINITY;

	for (size_t i = 0; i < 256; i++)
	{
		score = xor_with_char_score(bytestring, i);
		if (score < final_score)
		{
			final_score = score;
			if (winner)
				*winner = i;
		}
	}
	return final_score;
}

void	xor_with_char_print(const t_byte_string *bytestring, char c)
{
	printf("xor against %c(%3u): ", c, (unsigned int) c);
	for (size_t i = 0; i < bytestring->size; i++)
	{
		printf("%c", bytestring->bytes[i] ^ c);
	}
	printf("\n");
}

float	xor_with_char_score(const t_byte_string *bytestring, char c)
{
	unsigned int	value;
	unsigned int	frequencies[28] = {0};

	for (size_t i = 0; i < bytestring->size; i++)
	{
		value = bytestring->bytes[i] ^ c;
		if (value == ' ')
			frequencies[26]++;
		else if (isalpha(value))
		{
			if (isupper(value))
				frequencies[value - 'A']++;
			else
				frequencies[value - 'a']++;
		}
		else
			frequencies[27]++;
	}
	float	score = 0.0f;
	unsigned int	alpha_total = bytestring->size - frequencies[26] - frequencies[27];
	for (size_t i = 0; i < 26; i++)
	{
		score += fabs(g_english_frequency[i] - ((float) frequencies[i] / (float) alpha_total));
	}
	score += fabs(0.2 - ((float) frequencies[26] / (float) bytestring->size));
	return score;
}