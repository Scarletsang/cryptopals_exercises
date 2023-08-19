#include "byte_string/byte_string.h"
#include "xor/xor.h"

#include <stdlib.h>

/**
 * single_byte_xor_cipher
*/

int main(int argc, char **argv)
{
	if (argc != 2)
		return EXIT_FAILURE;
	t_byte_string	bytestring = byte_string_decode_hex(argv[1], false);
	char			winner;

	xor_score(&bytestring, &winner);
	xor_with_char_print(&bytestring, winner);
	byte_string_destroy(&bytestring);
	return EXIT_SUCCESS;
}
