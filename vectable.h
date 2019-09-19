/** \file
 * \brief declarations for the vector table module */
#ifndef VECTABLE_H
#define VECTABLE_H


/* code for a generic table of vectors.  the vector table is indexed
 * by two continuous variables (doubles) which are discretized for
 * access.
 */

/* bins are used to compute table indices from the continous index
 * variable.  a bin handles a group of slots, each of which
 * corresponds to one table index.  each slot references a particular
 * discrete range of the continuous index variable.  a bin "includes"
 * a slot for the upper bound, but not the lower.  since this structure
 * contains redundant information, it should be initialized using the
 * provided routine to insure consistency.
 */

struct bin {
    double lb;			/* lower bound */
    double ub;			/* upper bound */
    double slotsize;		/* discretizes the index */
    int nslots;			/* number of slots in this bin */
};

/* the vector table is a two dimensional array of vectors of arbitrary
 * type (void *).  the user must cast a retreived vector to the proper
 * type.  the point of this structure is really to handle the use of
 * a continuous variable as index.
 */

struct vec_table {
    /* the bin arrays must contain a terminating bin with all 0 values */
    struct bin *ibins;		/* array of i index bins */
    struct bin *jbins;		/* array of j index bins */
    int islots;		/* total number of i slots in table */
    int jslots;		/* total number of j slots in table */
    void * (*create_vec)();	/* user fn to make one vector for the table
				   void * (*create_vec)(double,double)
				 */
    void ***table;		/* actual table, size islots x jslots,
				   containing (void*)
				 */
    void **free_list;		/* list of addl vectors to be freed on exit */
    int flist_size;		/* size of free_list */
    int flist_max;		/* current flist max size */
};

#define DFLT_FLIST_MAX (16)	/* initial max size for free list */
#define HARD_FLIST_MAX (1000)  /* absolute maximum for free list */

enum vectab_errs { VECTAB_MEM_ERR, VECTAB_BOUNDS_ERR };

/* externally callable routines; see .c file for descriptions */

extern struct vec_table *create_vector_table();
extern void destroy_vector_table();
extern void *get_vector(struct vec_table *, double x, double y, int* erc);
extern void scale_bins();

/* This is a command line flag.  If set, this flag forces the travel vector to
 * be calculated rather than read from a travel table.  This is necessary for
 * calculation of derivitives when using the calibration tool. */
extern int tt_calib_mode;

#endif
