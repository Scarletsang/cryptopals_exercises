#include "byte_string/byte_string.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

/**
 * hex_to_base64
*/

int main(int argc, char **argv)
{
	if (argc != 2)
		return EXIT_FAILURE;
	t_byte_string	hex_string = byte_string_decode_hex(argv[1], true);

	if (!hex_string.bytes)
		return EXIT_FAILURE;
	// byte_string_print_as_binary(hex_string);
	// byte_string_print_as_string(hex_string);
	// fflush(stdout);
	{
		char		*base64_encoded_string = byte_string_encode_base64(&hex_string);

		if (!base64_encoded_string)
			return EXIT_FAILURE;
		printf("%s\n", base64_encoded_string);
		free(base64_encoded_string);
	}
	return EXIT_SUCCESS;
}
