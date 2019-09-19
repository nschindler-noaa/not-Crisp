#include <stdio.h>

/* See this header for information on the format of the archive and how it works */
#include "create-flow-archive-daynight.h"

const char *dams[] = {
	"BON",
	"TDA",
	"JDA",
	"MCN",
	"PRD",
	"WAN",
	"RIS",
	"RRH",
	"WEL",
	"CHJ",
	"IHR",
	"LMN",
	"LGS",
	"LWG",
	"DWR"
};

int main(int argc, char **argv) {
	int i;
	int col;
	int number = 10001000; /* Number of the DATA */
	int dam;

	printf("V0.9 CRiSP Flow Archive\n\n");

	printf("games\t1\nyears\t1\n");

	printf("dams 15");
	for (dam=0; dam < 15; dam++) {
		printf(" %s", dams[dam]);
	}
	printf("\n");

	printf("periods\t365\t0:");
	for (i=1, col=1; i <= 365; i++, col++) {
		printf("%d", i);
		if (i < 365) printf(",");

		if (col >= 20) {
			printf("\n");
			col = 0;
		}
	}
	printf(":365\n\nDATA\n");

	for (i=0; i < 365; i++) {
		for (dam=0; dam < 15; dam++) {
			double whole_day_value = SPILL_EXPECTED(i)/WHOLE_DAY_DIVIDER;
			double day_value = SPILL_EXPECTED(i)/DAY_DIVIDER;
			double night_value = SPILL_EXPECTED(i)/NIGHT_DIVIDER;

			printf("%d\t2006\t0\t%s\t50.0\t", ++number, dams[dam]);

			/* Print planned spill (whole day value if necessary) */
			if (specified_table[i%5].whole_day) printf("%.12f", whole_day_value);
			else printf("n/a");

			printf("\t0.0\t100000\t");

			/* Print planned spill (day value if necessary) */
			if (specified_table[i%5].day) printf("%.12f", day_value);
			else printf("n/a");

			printf("\t");

			/* Print planned spill (day value if necessary) */
			if (specified_table[i%5].night) printf("%.12f", night_value);
			else printf("n/a");

			printf("\n");
		}
	}

	return 0;
}


