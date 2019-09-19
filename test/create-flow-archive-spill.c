#include <stdio.h>

/* Program for generating a simple flow.archive file */

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
			printf("%d\t2006\t0\t%s\t50.0\t%.12f\t0.0\t100000\n",
				/* DATA (number) */
				++number,

				/* Project Name */
				dams[dam],

				/* Planned Spill (fraction) */
				((double)i)/365);
		}
	}

	return 0;
}

