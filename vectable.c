/** \file
 * \brief code for a generic table of vectors.  the vector table is indexed
 * by two continuous variables (doubles) which are discretized for
 * access.
 */
#include <malloc.h>
#include <math.h>
#include "vectable.h"

/*********************************************************************/
/* INTERNAL MODULE ROUTINES 					     */
/*********************************************************************/

/* copy and return a new null_terminated array of bins.  this is used
 * to copy the initial bin parameters specified by the user in
 * create_vector_table().  it allocates memory which is freed in
 * destroy_vector_table().
 * return: new bin array on success; 0 on error
 */

static struct bin *create_bins(old)
    struct bin *old;
{
    struct bin *new=0;
    struct bin *ptr;

    /* count bins.  use slotsize field to find terminator */
    int nbins=0;
    ptr = old;
    while (ptr && ptr->slotsize) {
	++nbins;
	++ptr;
    }
    /* add one for the terminator */
    ++nbins;
    
    /* create the new one and copy the old into it */
    if (!(new = (struct bin *) malloc(nbins * sizeof(struct bin))))
	return 0;
    ptr = new;
    while (nbins--)
	*(ptr++) = *(old++);
    
    return new;
}

/* init_bins: initialize some fields in an array of bins.
 *   params: a null terminated array (null bin has all zero values) of bins;
 *	     lower bound for first bin.
 *   preconditions:
 * 	     upper bounds and slot sizes exist in all (non-null) bins;
 *	     bin array in ascending order (by upper bound);
 *	     bin array properly terminated (last bin has all zero values);
 *	     slot sizes and u. bounds such that exact coverage with an
 *	        integral number of slots in each bin is provided.
 *   postconditions:
 *	     all remaining fields properly initialized.  each lower bound
 *		will match upper bound from previous bin.  nslots computed
 *		to include a slot for the upper bound, but not the lower.
 *   return: total number of slots on success, 0 on error.
 */

static int init_bins_from_ub(bins, initial_lb)
    struct bin *bins;
    double initial_lb;
{
    double lb = initial_lb;
    double nslots;
    int total_slots = 0;

    /* use upper bound as flag for null bin */
    while (bins->ub) {
	bins->lb = lb;

	/* perform configuration sanity check */
	if (!bins->slotsize || bins->lb >= bins->ub)
	    return 0;
	
	nslots = (bins->ub - bins->lb) / bins->slotsize;
	bins->nslots = (int)nslots;

	/* check for integral number of slots with full coverage */
	if (bins->nslots != nslots)
	    return 0;
	lb = bins->ub;
	total_slots += bins->nslots;
	++bins;
    }
    return total_slots;
}

/* fill the table with vectors using information from the
 * structure.  parameter values will be drawn from slotsize
 * boundaries.  within bins, upper bound values are attributed slots,
 * while lower bound values are not.  
 */

static void fill_table(vtab)
    struct vec_table *vtab;
{
    /* use nslots as the indicator for terminating null bin */
    struct bin *ibin = vtab->ibins;
    int islot = 0;
    while (ibin->nslots) {
	int i;
	for (i=1; i <= ibin->nslots; ++i, ++islot) {
	    double i_val = ibin->lb + (i*ibin->slotsize);
	    struct bin *jbin = vtab->jbins;
	    int jslot = 0;
	    while (jbin->nslots) {
		int j;
		for (j=1; j <= jbin->nslots; ++j, ++jslot) {
		    double j_val = jbin->lb + (j*jbin->slotsize);
		    vtab->table[islot][jslot]
			= (*vtab->create_vec)(i_val, j_val);
		}
		++jbin;
	    }
	}
	++ibin;
    }
}

/* allocate the underlying table, a two dimensional array of void*
 * return: 1 on success; 0 on error
 */

static int alloc_table(vtab)
    struct vec_table *vtab;
{
    int i;
    
    if (!(vtab->table = (void ***) malloc(vtab->islots * sizeof(void**))))
	return 0;

    for (i=0; i < vtab->islots; ++i) 
	if (!(vtab->table[i] = (void **) malloc(vtab->jslots * sizeof(void*))))
	    return 0;
	
    return 1;
}

/* routine to add a vector to the free list.
 * returns 1 on success; 0 on error
 */

static int free_list_insert(vtab, vec)
    struct vec_table *vtab;
    void *vec;
{
    /* make more room if necessary */
    if (!vtab->free_list) {
	vtab->free_list = (void **) malloc(DFLT_FLIST_MAX * sizeof(void *));
	if (!vtab->free_list)
	    return 0;
	vtab->flist_max = DFLT_FLIST_MAX;
	vtab->flist_size = 0;
    }
    else if (vtab->flist_size == vtab->flist_max) {
	void **tmp_list = 0;
	if (vtab->flist_max >= HARD_FLIST_MAX) /* absolute capacity */
	    return 0;
	vtab->flist_max *= 2;
	tmp_list = 
	    (void **) realloc(vtab->free_list, vtab->flist_max*sizeof(void *));
	if (!tmp_list)
	    return 0;
	vtab->free_list = tmp_list;
    }
    /* insert */
    vtab->free_list[vtab->flist_size++] = vec;
    return 1;
}

/* locate the appropriate slot for a continuous parameter, based on
 * the given array of bins.  the bin array must be properly
 * constructed (including null-terminated.)
 *
 * individual slots in the table contain vectors computed for
 * parameter values on "slotsize" boundaries for the appropriate bin,
 * excluding the lower bound and including the upper bound.  e.g. a
 * bin with lb 0.0, ub 1.0, and slotsize 0.25 contains four slots with
 * vectors computed for the values: 0.25, 0.5, 0.75, and 1.0.  a
 * parameter passed to this fn falls in a given slot if its value is
 * in the range slot_value +/- slotsize/2 in general (see next para.).
 *
 * this access fn is a little goofy in the way it handles the boundary
 * conditions.  bins are in order of ascending parameter values.  the
 * bin for a parameter is selected by finding the first bin with upper
 * bound greater than the parameter.  the param is then associated
 * with one of the slots for that bin, unless its value lies beneath
 * the first slot, in which case it is assigned to the last slot in
 * the preceding bin.  a parameter greater than the absolute lower
 * bound but less than the first slot is assigned to the first slot.
 *
 * this access method should be changed to something less braindead
 * later, but is this way now for backwards compatibility.
 *
 * return: slot number if found (0 <= slot < nslots);
 *	   -1 if param beneath absolute lower bound or other error;
 *	   nslots if param above absolute upper bound
 */

static int find_slot(bins, val)
    struct bin *bins;		/* bin array to search */
    double val;			/* parameter value */
{
    int slot=0;
    /* beneath lower bound? */
    if (!bins || !bins->nslots || val < bins->lb)
	return -1;

    while (bins->nslots && bins->ub < val) 
	slot += (bins++)->nslots;

    /* above upper bound? */
    if (!bins->nslots)
	return slot;

    /* find the slot.  first slot in a bin is slot 0.  this
     * computation could reduce the slot to the last value of the
     * previous bin.
     */
    slot += (int) floor((val - bins->lb) / bins->slotsize - 0.5);

    /* beneath lower bound condition already handled */
    return ((slot >= 0) ? slot : 0);
}

/*********************************************************************/
/* EXTERNAL ROUTINES (user callable) 				     */
/*********************************************************************/

/* externally callable routine to create (malloc) a vector table.  the
 * bin parameters should conform to the preconditions outlined in the
 * bin initializing fn above.  this create routine will copy the bin
 * information, create and fully initialize a vector table.  the
 * associated destroy fn should be used to properly destroy the table
 * when it is no longer needed.  "create_vec_fn" must be a pointer to
 * a user fn which creates ("malloc"s) and returns a vector from two
 * continuous parameters (doubles).  the created vectors will be
 * inserted in the table and deleted ("free"d) by the table destroy
 * fn.
 * return: pointer to vtab on success; 0 on error
 */

struct vec_table *create_vector_table(ibins, i_lb, jbins, j_lb, create_vec)
    struct bin *ibins;		/* array of i index bins */
    double i_lb;		/* lower bound of first i bin */
    struct bin *jbins;		/* array of j index bins */
    double j_lb;		/* lower bound of first j bin */
    void * (*create_vec)();	/* user fn to make one vector for the table */
{
    struct vec_table *vtab =
	(struct vec_table *) malloc(sizeof(struct vec_table));
    if (!vtab)
	return 0;

    /* copy and initialize bins and slot totals */
    if (!(vtab->ibins = create_bins(ibins))
	|| !(vtab->jbins = create_bins(jbins))
	|| !(vtab->islots = init_bins_from_ub(vtab->ibins, i_lb))
	|| !(vtab->jslots = init_bins_from_ub(vtab->jbins, j_lb)) )
	return 0;

    if (!alloc_table(vtab))
	return 0;
    vtab->create_vec = create_vec;
    fill_table(vtab);

    vtab->free_list = 0;
    vtab->flist_size = 0;
    vtab->flist_max = 0;
    return vtab;
}

/* internal routing to destroy just the free list */
static void destroy_free_list(vtab)
    struct vec_table *vtab;
{
    if (vtab->free_list) {
	int i;
	for (i=0; i < vtab->flist_size; ++i)
	    free(vtab->free_list[i]);
	free(vtab->free_list);
	vtab->free_list = 0;
    }
    vtab->flist_size = 0;
}

/* user routine to destroy the entire vtab */

void destroy_vector_table(vtab)
    struct vec_table *vtab;
{
    if (vtab) {
	int i, j;
	free(vtab->ibins);
	free(vtab->jbins);
	for (i=0; i < vtab->islots; ++i) {
	    for (j=0; j < vtab->jslots; ++j)
		free(vtab->table[i][j]);
	    free(vtab->table[i]);
	}
	free(vtab->table);
	destroy_free_list(vtab);
	free(vtab);
    }
}


/* access the table using two continuous vars as indices.  returns a
 * vector.  current implementation is: a parameter below the absolute
 * lower bound is an error condition.  a parameter above the greatest
 * upper bound causes computation of a new vector appropriate to that
 * specific parameter pair.  this behavior could be made more flexible
 * in the future.  new vectors created in this manner will be stored
 * in an internal list, and destroyed when the entire vector table is
 * destroyed.
 *
 * returns a vector, or 0 on error.  error code is returned in erc
 * parameter, if supplied by caller.
 */

void *get_vector(vtab, x, y, erc)
    struct vec_table *vtab;
    double x;
    double y;
    int *erc;
{
    void *newvec;
    int tmperc;

    /* If not in tt calib mode, utilize travel table.  Else, fall through
    to code which calculates a travel vector given an incoming mu and lam */
    if (!tt_calib_mode) {
        int i = find_slot(vtab->ibins, x);
        int j = find_slot(vtab->jbins, y);

        if (!erc)
            erc = &tmperc;
        *erc = VECTAB_MEM_ERR;

        if (i < 0 || j < 0) {
            *erc = VECTAB_BOUNDS_ERR;
            return 0;
        }

        if (i < vtab->islots && j < vtab->jslots)
            return vtab->table[i][j];
    }
    
	
	/* compute new vector if either param out of range 
                                           (or if in tt calib mode)*/
    newvec = (*vtab->create_vec)(x, y);
    if (!newvec || !free_list_insert(vtab, newvec)) {
	/* attempt to recover some memory and try again */
	if (newvec)
	    free(newvec);
	destroy_free_list(vtab);
	newvec = (*vtab->create_vec)(x, y);
	if (!newvec || !free_list_insert(vtab, newvec)) 
	    return 0;
    }

    return newvec;
}

/* scale bins - utility routine to scale all values in a bin array
 * by a certain amount.  bin array must be null-terminated.  slotsize
 * is used for the termination test.
 */

void scale_bins(bins, scale)
    struct bin *bins;
    double scale;
{
    while (bins && bins->slotsize) {
	bins->lb *= scale;
	bins->ub *= scale;
	bins->slotsize *= scale;
	++bins;
    }
}
