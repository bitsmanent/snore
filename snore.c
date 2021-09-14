/* See LICENSE for license details. */
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BILLION     1000000000.0
#define TICK        250000
#define TINY_TICK   100
#define XOVRTM      0.250
#define CLEAR	    "\33[2K\r"
#define LENGTH(X)   (sizeof X / sizeof X[0])
#define ISCHR(c)    (c >= 'a' && c <= 'z')

typedef struct symbol_t {
	char sym;
	int mult;
	int precision;
} Symbol;

void die(const char *errstr, ...);
int sleepu(double usec);
double time_to_sec(char *s);
void time_print(double tm);
void zzz_loop(struct timespec start_tsp, double stop_tm, double end_tm, double tick);

/* must be in ascending order */
static Symbol symbols[] = {
	/* symbol  multiplier  precision */
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
sleepu(double usec) {
	struct timespec req, rem;
	int r;

	req.tv_sec = 0;
	req.tv_nsec = usec * 1000;
	while((r = nanosleep(&req, &rem)) == -1 && errno == EINTR)
		req = rem;
	return r;
}

double
time_to_sec(char *s) {
	double calculated = 0.0, part;
	char *parse_end, *string_end;
	int j;

	string_end = s + strlen(s);
	while(s < string_end) {
		part = strtod(s, &parse_end);
		if(parse_end == s) {
			/* error parsing float */
			return -1;
		}
		s = parse_end;
		if(s < string_end && ISCHR(s[0])) {
			for(j = 0; j < LENGTH(symbols); ++j) {
				if(s[0] == symbols[j].sym) {
					part *= symbols[j].mult;
					s++;
					break;
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


void
zzz_loop(struct timespec start_tsp, double stop_tm, double end_tm, double tick){
	struct timespec current_tsp;
	double current_tm;

	clock_gettime(CLOCK_REALTIME, &current_tsp);
	current_tm = (current_tsp.tv_sec - start_tsp.tv_sec) +
			(current_tsp.tv_nsec - start_tsp.tv_nsec) / BILLION;

	for(    ; current_tm < stop_tm;    ) {

		printf("%.3fs elapsed | %.3fs remaining ", current_tm, end_tm - current_tm);
#ifndef DISABLE_BUFFERING
		fflush(stdout);
#endif
		sleepu(tick);
		printf(CLEAR);
		clock_gettime(CLOCK_REALTIME, &current_tsp);
		current_tm = (current_tsp.tv_sec - start_tsp.tv_sec) +
				(current_tsp.tv_nsec - start_tsp.tv_nsec) / BILLION;
	}

} 



int
main(int argc, char *argv[]) {
	struct timespec start,elapsed;
	clock_gettime(CLOCK_REALTIME, &start);
	double endtm = 0, tm, elapsed_tm;
	int i;

	if(argc == 2 && !strcmp("-v", argv[1]))
		die("snore-"VERSION"\n");
	if(argc == 1) {
		endtm = symbols[LENGTH(symbols) - 1].mult;
	}
	else {
		for(i = 1; i < argc; ++i) {
			tm = time_to_sec(argv[i]);
			if(tm < 0)
				die("%s: wrong time\n", argv[i]);
			endtm += tm;
			if(endtm >= UINT_MAX)
				die("%s: time too large\n", argv[0]);
		}
	}

#ifdef DISABLE_BUFFERING
	setvbuf(stdout, NULL, _IONBF, 0);
#endif
	zzz_loop(start, endtm-XOVRTM, endtm,      TICK);
	zzz_loop(start, endtm       , endtm, TINY_TICK);

	clock_gettime(CLOCK_REALTIME, &elapsed);
	elapsed_tm = (elapsed.tv_sec - start.tv_sec) +
			(elapsed.tv_nsec - start.tv_nsec) / BILLION;

	time_print(elapsed_tm);
	printf("\n");

	return 0;
}
