#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>

enum return_codes {

	SUCCESS = 1,
	ERROR_NOARGS,
	ERROR_ARGSMALL,
	ERROR_ARGBIG,
	ERROR_ARGINVALID,
	ERROR_ARGUNKNOWN,
	ERROR_ARGRECTBAD,
	ERROR_ARGRECTBADNUM,
	ERROR_NUMCONVERT,

};

/* strtol() returns longs anyway */
struct colour {
	long r, g, b;
};

struct rectangle {
	int w, h;
};

/* A format string won't exceed 36 characters + null*/
typedef char col_fmt_string[37];

const char escape_seq_fmt[] = "\x1b[38;2;%1$ld;%2$ld;%3$ld;48;2;%1$ld;%2$ld;%3$ldm";
const char block_utf8[]     = "\xe2\x96\xa0";
const char reset_seq[]        = "\x1b[0m";

void signal_handler(int signal) {

	printf(reset_seq);
	exit(0);

}

void print_help() {

	puts("help.");

}

int parse_arg(const char *arg, struct colour *col) {

	if (strlen(arg) < 6) {

		fprintf(stderr, "Argument \"%s\" is too small.\n", arg);
		return ERROR_ARGSMALL;

	}

	/* Skip over # and 0x prefixes */
	if (arg[0] == '#')
		arg++;
	else if (arg[0] == '0' && arg[1] == 'x')
		arg += 2;

	if (strlen(arg) != 6) {

		fprintf(stderr, "Argument \"%s\" is too big.\n", arg);
		return ERROR_ARGBIG;

	}

	/* Check string is valid hexadecimal */
	for (int i = 0; i < 6; i++) {

		/* Check ascii numbers */
		if (arg[i] >= '0' && arg[i] <= '9')
			continue;
		/* Check ascii a-f */
		if (arg[i] >= 'a' && arg[i] <= 'f')
			continue;
		/* Check ascii A-F */
		if (arg[i] >= 'A'&& arg[i] <= 'F')
			continue;

		fprintf(stderr, "Invalid character: '%c'\n", arg[i]);
		return ERROR_ARGINVALID;

	}

	/* Split into three */
	typedef char colstr[3];
	struct {
		colstr red, green, blue;
	} col_strs;
	memset(&col_strs, 0, sizeof(col_strs));
	strncpy(col_strs.red, arg, 2);
	strncpy(col_strs.green, arg + 2, 2);
	strncpy(col_strs.blue, arg + 4, 2);

	/* Convert hex strings and fill struct */
	char *endptr = NULL;
	col->r = strtol(col_strs.red, &endptr, 16);
	col->g = strtol(col_strs.green, &endptr, 16);
	col->b = strtol(col_strs.blue, &endptr, 16);
	if (endptr[0] != '\0') {

		/* This shouldn't ever happen */
		fprintf(stderr, "strtol() couldn't parse %s\n", endptr);
		return ERROR_NUMCONVERT;

	}

	return SUCCESS;

}

int parse_rect_arg(struct rectangle *rect, const char *arg) {
	

	char *endptr = NULL;
	char *xpointer;

	if ((xpointer = strstr(arg, "x")) == NULL) {

		rect->w = strtol(optarg, &endptr, 0);
		rect->h = rect->w;

	} else {

		if (*(xpointer + 1) == '\0') {

			fprintf(stderr, "Invalid rectangle specification: %s\n", arg);
			return ERROR_ARGRECTBAD;

		}

		*xpointer = '\0';
		rect->w = strtol(arg, &endptr, 0);
		rect->h = strtol(xpointer + 1, &endptr, 0);

	}
	
	if (endptr[0] != '\0') {

		fprintf(stderr, "Invalid number in rectangle specification: %s\n", arg);
		return ERROR_ARGRECTBADNUM;

	}

	return SUCCESS;

}

void print_rect(const struct colour *col, const struct rectangle *rect) {
	
	col_fmt_string escape;
	snprintf(escape, sizeof(escape), escape_seq_fmt, col->r, col->g, col->b);

	for (int y = 0; y < rect->h; y++) {

		printf(escape);
		for (int x = 0; x < rect->w; x++)
			printf(block_utf8);
		printf("%s\n", reset_seq);

	}

}

int main(int argc, char **argv) {

	if (argc < 2) {

		print_help();
		return ERROR_NOARGS;

	}

	signal(SIGINT, signal_handler);

	struct rectangle rect;
	rect.w = 20;
	rect.h = 10;

	/* Parse arguments */
	int c;
	while (1) {

		static struct option long_options[] = {

			{ "help", no_argument, 0, 'h' },
			{ "rectangle", required_argument, 0, 'r' },
			{ 0, 0, 0, 0 }

		};

		int option_index = 0;
		c = getopt_long(argc, argv, "hr:", long_options, &option_index);

		/* Detect the end of options */
		if (c == -1)
			break;

		switch (c) {

			case 'h':
				print_help();
				return 0;
				break;
				

			case 'r':
				int ret = parse_rect_arg(&rect, optarg);
				if (ret != SUCCESS)
					return ret;
				break;

			case '?':
				return ERROR_ARGUNKNOWN;

			default:
				break;

		}

	}

	if (optind >= argc) {

		print_help();
		return ERROR_NOARGS;

	}

	while (optind < argc) {

		struct colour col;
		int result;
		if ((result = parse_arg(argv[optind++], &col)) != SUCCESS)
			return result;

		print_rect(&col, &rect);

	}

	return 0;

}
