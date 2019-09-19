/* \file
 * \brief Defines the interface for delayed mortality-related calculations (namely the table) */

#ifndef	__DELAYED_MORT_H
#define	__DELAYED_MORT_H

#include "crisp_defs.h"

struct release;

#ifdef __cplusplus
extern "C" {
#endif

	/** Delayed mortality table which can be used in calculations and output */
	typedef struct {
		int num_collector_dams; /** Number of collector dams */

		/** One delayed mortality table column per collector dam */
		struct delayed_mortality_column {
			/** The dam */
			struct dam *dam;
			/** Probability of being transported in on a given day from a given dam */
			double prob[DAYS_IN_SEASON];
		} *columns;

		/** Indexed by the number of bypasses, this is the probability over the
		 * whole season of inriver fish experiencing this number of
		 * bypasses. */
		double *prob_num_bypasses;

	} delayed_mortality_table;

	/** Constructs the delayed mortality table */
	extern void create_delayed_mortality_table(struct release *release, delayed_mortality_table **d, int game);
	/** Fills in the delayed mortality table */
	extern void delayed_mortality_table_free(delayed_mortality_table *d);

#ifdef __cplusplus
}
#endif

#endif

