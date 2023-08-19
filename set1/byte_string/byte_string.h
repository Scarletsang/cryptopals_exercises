#ifndef CRYPTALPAL_BYTE_STRING_H
# define CRYPTALPAL_BYTE_STRING_H

# include <stdlib.h>
# include <stdbool.h>

extern const char	*g_hex_set;
extern const char	*g_base64_set;

//////////////////////////////////////////////
/////////////      byte      /////////////////
//////////////////////////////////////////////

typedef unsigned char	t_byte;

t_byte	byte_cast_from_start(t_byte byte, unsigned int amount);

t_byte	byte_cast_from_last(t_byte byte, unsigned int amount);

//////////////////////////////////////////////
//////////      byte string     //////////////
//////////////////////////////////////////////

typedef struct s_byte_string
{
	t_byte	*bytes;
	size_t	size;
	bool	discard_after_encode;
}		t_byte_string;

t_byte_string	byte_string_create(size_t size, bool discard_after_encode);

void  			byte_string_destroy(t_byte_string *byte_string);

t_byte_string	byte_string_empty(void);

bool			byte_string_is_empty(t_byte_string byte_string);

void			byte_string_move(t_byte_string *dest, t_byte_string *src);

/**
 * @brief Cast a bytestring to a type of arbitary bit size,
 * and then get the value of an item in the casted bytestring.
*/
unsigned int	byte_string_casted_value(const t_byte_string *byte_string, \
size_t item_bit_size, size_t index);

//////////////////////////////////////////////////////
//////////      byte string decoder     //////////////
//////////////////////////////////////////////////////

t_byte_string	byte_string_decode_cstring(const char *string, bool discard_after_encode);

t_byte_string	byte_string_decode_hex(const char *string, bool discard_after_encode);

//////////////////////////////////////////////////////
//////////      byte string encoder     //////////////
//////////////////////////////////////////////////////

char			*byte_string_encode_hex(t_byte_string *byte_string);

char			*byte_string_encode_base64(t_byte_string *byte_string);

///////////////////////////////////////////////////////
//////////      byte string printers     //////////////
///////////////////////////////////////////////////////

void			byte_string_print_as_binary(t_byte_string byte_string);

void			byte_string_print_as_string(t_byte_string byte_string);

#endif