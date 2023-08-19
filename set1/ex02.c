#include "byte_string/byte_string.h"
#include "xor/xor.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * fixed_xor
*/

int	main(int argc, char **argv)
{
	if (argc != 3)
		return EXIT_FAILURE;
	t_byte_string	bstr1 = byte_string_decode_hex(argv[1], false);
	t_byte_string	bstr2 = byte_string_decode_hex(argv[2], false);

	if (byte_string_is_empty(bstr1) || byte_string_is_empty(bstr2))
	{
		byte_string_destroy(&bstr1);
		byte_string_destroy(&bstr2);
		return EXIT_FAILURE;
	}

	t_byte_string	bstr_after_xor = xor_fixed(bstr1, bstr2, true);
	byte_string_print_as_string(bstr_after_xor);
	char	*hex_encoded_after_xor = byte_string_encode_hex(&bstr_after_xor);

	if (!hex_encoded_after_xor)
	{
		byte_string_destroy(&bstr1);
		byte_string_destroy(&bstr2);
		return EXIT_FAILURE;
	}

	printf("after xor: %s\n", hex_encoded_after_xor);

	byte_string_destroy(&bstr1);
	byte_string_destroy(&bstr2);
	free(hex_encoded_after_xor);
	return EXIT_SUCCESS;
}
