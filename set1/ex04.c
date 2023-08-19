#include "byte_string/byte_string.h"
#include "xor/xor.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

/**
 * detect_single_character_xor
*/

int	main(int argc, char **argv)
{
	if (argc != 3)
		return EXIT_FAILURE;

	int		length = atoi(argv[2]);
	if (length <= 0)
		return EXIT_FAILURE;

	char	*line = malloc(length * sizeof(char) + 1);
	if (!line)
		return EXIT_FAILURE;

	FILE	*infile = fopen(argv[1], "r");
	if (!infile)
	{
		free(line);
		return EXIT_FAILURE;
	}

	{
		t_byte_string	byte_string;
		char			c;
		float			score = INFINITY;

		t_byte_string	winner_byte_string;
		char			winner_c;
		float			winner_score = INFINITY;

		char			*first_newline;
		size_t			line_len = 0;

		while (getline(&line, &line_len, infile) != -1)
		{
			first_newline = strchr(line, '\n');
			if (first_newline)
				*first_newline = '0';
			byte_string = byte_string_decode_hex(line, true);
			score = xor_score(&byte_string, &c);
			if (score < winner_score)
			{
				winner_score = score;
				if (!byte_string_is_empty(winner_byte_string))
					byte_string_destroy(&winner_byte_string);
				winner_c = c;
				byte_string_move(&winner_byte_string, &byte_string);
			}
			else
				byte_string_destroy(&byte_string);
		}
		xor_with_char_print(&winner_byte_string, winner_c);
		printf("score: [%f]\n", winner_score);
		byte_string_destroy(&byte_string);
		byte_string_destroy(&winner_byte_string);
	}
	free(line);
	fclose(infile);
	return EXIT_SUCCESS;
}
