/** \file
 * \brief Scenario vs. Monte Carlo mode. */
#ifndef	_MODE_H
#define	_MODE_H

/* definitions for modes of operation */

#define SCENARIO_MODE	0
#define MONTE_MODE 	1

/* struct used to oversee a run.  useful for explicit event dispatching */

extern struct monte_carlo_data;
struct run_mode {
    unsigned int running;		/* run in progress? (bool) */
    unsigned int mode;			/* scenario, monte, batch, etc. */
/*    int game;	*/			/* current game under execution */
    struct monte_carlo_data *monte_data; /* used in monte carlo mode */
};

extern struct run_mode run_mode;	/* defined in crisp.c */

#endif

