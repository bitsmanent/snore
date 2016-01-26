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
int time_to_sec(char *s);
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

int
time_to_sec(char *s) {
	int i, j, len;
	char tmp[16];

	len = strlen(s);
	for(i = 0; i < len; ++i) {
		if(!ISCHR(s[i]))
			continue;
		for(j = 0; j < LENGTH(symbols); ++j) {
			if(s[i] == symbols[j].sym) {
				strncpy(tmp, s, i + 1);
				return atoi(tmp) * symbols[j].mult;
			}
		}

		/* unsupported suffix */
		return -1;
	}

	/* no suffix specified, use the first */
	strncpy(tmp, s, i + 1);
	return atoi(tmp) * symbols[0].mult;
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
