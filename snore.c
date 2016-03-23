/* See LICENSE for license details. */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TICK		0250000
#define DELTA		((double)TICK / 1000000)
#define SCLEAR		"\r                                            \r"
#define LENGTH(X)	(sizeof X / sizeof X[0])
#define ISCHR(c)	(c >= 'a' && c <= 'z')

typedef struct symbol_t {
	char sym;
	int mult;
	int precision;
} Symbol;

void die(const char *errstr, ...);
double time_to_sec(char *s);
void time_print(double tm);

/* must be in ascending order */
static Symbol symbols[] = {
	/* symbol, multiplier, precision */
	{ 's',     1,          3 }, /* first is default (if no suffix) */
	{ 'm',     60,         0 },
	{ 'h',     3600,       0 },
	{ 'd',     86400,      0 }, /* last is default (if no arguments) */
};

void
die(const char *errstr, ...) {
	va_list ap;

	va_start(ap, errstr);
	vfprintf(stderr, errstr, ap);
	va_end(ap);
	exit(1);
}

double
time_to_sec(char *s) {
	int j;
	double part, calculated;
	char *parse_end, *string_end;

	calculated = 0.0;
	string_end = s + strlen(s);

	while(s < string_end) {
		part = strtod(s, &parse_end);
		if(parse_end == s) {
			/* error parsing float */
			return -1;
		}
		s = parse_end;
		if(s < string_end) {
			if(ISCHR(s[0])) {
				for(j = 0; j < LENGTH(symbols); ++j) {
					if(s[0] == symbols[j].sym) {
						part *= symbols[j].mult;
						s++;
						break;
					}
				}
			}
		}
		calculated += part;
	}

	return calculated;
}

void
time_print(double tm) {
	double piece;
	int i;
	char buf[10], *p;

	for(i = LENGTH(symbols) - 1; i >= 0; --i) {
		piece = tm / symbols[i].mult;
		snprintf(buf, sizeof buf, "%09f", piece);
		p = strrchr(buf, '.');
		if(symbols[i].precision)
			p += 1 + symbols[i].precision; /* truncate */
		*p = '\0';
		printf("%s%c%s", buf, symbols[i].sym, i ? " " : "");
		tm -= (int)piece * symbols[i].mult;
	}
}

int
main(int argc, char *argv[]) {
	double tm, endtm;
	int i;

	if(argc == 2 && !strcmp("-v", argv[1]))
		die("snore-"VERSION", © 2016 Claudio Alessi, see LICENSE for details\n");

	if(argc == 1) {
		endtm = symbols[LENGTH(symbols) - 1].mult;
	}
	else {
		endtm = 0;
		for(i = 1; i < argc; ++i) {
			tm = time_to_sec(argv[i]);
			if(tm < 0)
				die("%s: wrong time\n", argv[i]);
			endtm += time_to_sec(argv[i]);
		}
	}

	for(tm = 0; tm < endtm; tm += DELTA) {
		time_print(tm); /* ascending */
		printf(" | ");
		time_print(endtm - tm); /* descending */
		fflush(stdout);
		usleep(TICK);
		printf("%s", SCLEAR);
	}
	printf("\a%s elapsed\n", argv[1]);
	return 0;
}
