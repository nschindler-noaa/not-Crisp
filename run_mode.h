/** \file
 * \brief Definitions for run mode (i.e. choosing between scenario, Monte Carlo, etc. */

#ifndef C1_RUN_MODE_H
#define C1_RUN_MODE_H

#ifdef __cplusplus
extern "C" {
#endif

/** definitions for modes of operation */

enum run_modes { MODE_NONE, SCENARIO, MONTE, RT_SCENARIO, RT_MONTE };

/** test if a realtime run configured */
#define RT_MODE_ACTIVE(flag) ((flag)==RT_SCENARIO || (flag)==RT_MONTE)
    
/** struct used to oversee a run.  useful for explicit event dispatching */

struct run_mode {
    unsigned int running;	/** run in progress? (bool) */
    unsigned int batch;		/** batch mode active? (bool) */
    int mode;			/** scenario, monte, etc. */
    int compute_flow_break; /** 0 = OK, 1 -> cancel compute_all_flow() call */
    int summary;         /** in batch monte mode, should we write summary.altn? */
    struct monte_carlo_data *monte_data; /** used in monte carlo mode */
};

extern struct run_mode run_mode;	/* defined in crisp.c */

#ifdef __cplusplus
}
#endif
#endif
