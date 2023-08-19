#include "byte_string/byte_string.h"
#include "xor/xor.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * repeating_xor
*/

int main(int argc, char **argv)
{
	if (argc != 3)
		return EXIT_FAILURE;
	t_byte_string	target = byte_string_decode_cstring(argv[1], true);
	t_byte_string	repeating_key = byte_string_decode_cstring(argv[2], true);

	if (byte_string_is_empty(target) || byte_string_is_empty(repeating_key))
	{
		byte_string_destroy(&target);
		byte_string_destroy(&repeating_key);
		return EXIT_FAILURE;
	}

	t_byte_string	result_bytestring = xor_repeat(target, repeating_key, true);
	char			*result = byte_string_encode_hex(&result_bytestring);

	printf("result: [%s]\n", result);
	if (!result)
	{
		byte_string_destroy(&target);
		byte_string_destroy(&repeating_key);
		return EXIT_FAILURE;
	}
	byte_string_destroy(&target);
	byte_string_destroy(&repeating_key);
	free(result);
	return EXIT_SUCCESS;
}
