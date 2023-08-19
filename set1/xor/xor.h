#ifndef CRYPTALPAL_XOR_H
# define CRYPTALPAL_XOR_H

# include <stdbool.h>
# include "byte_string/byte_string.h"

extern const float	g_english_frequency[26];

t_byte_string	xor_fixed(t_byte_string bstr1, t_byte_string bstr2, bool discard_after_encode);

t_byte_string	xor_repeat(t_byte_string target, t_byte_string repeating_key, bool discard_after_encode);

float			xor_score(t_byte_string *bytestring, char *winner);

void			xor_with_char_print(const t_byte_string *bytestring, char c);

float			xor_with_char_score(const t_byte_string *bytestring, char c);

#endif