/** \file
 * \brief Implementation of species manipulation functions. */

#ifdef WIN32
#include <stdlib.h>
#endif

#include <stdio.h>
#include <string.h>

#include "crisp.h"
#include "species.h"

/**
 *	\brief Uses global info to allocate all the global species struct
 *
 *	\return
 *	 0	: OK
 *	-1	: error
 *
 *	note: since this routine occurs at very early init time,
 *	(before xv_main_loop) error messages are printed to stderr
 */

int allocate_species()
{
    if (!num_specs)
    {
	/* nothing configured, use defaults */
	num_specs = DFLT_NUM_SPECS; 
	spec_names = (char **) dflt_spec_names;
    }
    
    if (!(specs = (struct species *)
	  malloc(num_specs * sizeof(struct species))))
    {
	fprintf(stderr, "Error: no mem for species structs!\n");
	return -1;
    }
    bzero((char *)specs,
	  num_specs * sizeof(struct species));
    return 0;
}

/**
 *	\breif Allocates the dam_species struct for this dam
 *
 *	\return
 *	 0	: OK
 *	-1	: error
 *
 *	note: since this routine occurs at very early init time,
 *	(before xv_main_loop) error messages are printed to stderr
 */

int allocate_dam_species(seg)
    struct river_segment *seg;
{
    /* the general species alloc routine may not have been called yet,
     * so it is necessary to anticipate the default case of none specified.
     */
    int number_specs = num_specs ? num_specs : DFLT_NUM_SPECS;
    
    if (!(seg->current->dam.species = (struct dam_species *)
		  malloc(number_specs * sizeof(struct dam_species))))
    {
	fprintf(stderr, "Error: no mem for dam species structs!\n");
	return -1;
    }
    bzero((char *)seg->current->dam.species,
	  number_specs * sizeof(struct dam_species));
    return 0;
}

/**
 *	\brief allocates the reach_species struct for this reach
 *
 *	\return
 *	 0	: OK
 *	-1	: error
 *
 *	note: since this routine occurs at very early init time,
 *	(before xv_main_loop) error messages are printed to stderr
 */

int allocate_reach_species(seg)
    struct river_segment *seg;
{
    /* the general species alloc routine may not have been called yet,
     * so it is necessary to anticipate the default case of none specified.
     */
    int number_specs = num_specs ? num_specs : DFLT_NUM_SPECS;
    
    if (!(seg->current->reach.species = (struct reach_species *)
		  malloc(number_specs * sizeof(struct reach_species))))
    {
	fprintf(stderr, "Error: no mem for reach species structs!\n");
	return -1;
    }
    bzero((char *)seg->current->reach.species,
	  number_specs * sizeof(struct reach_species));
    return 0;
}

/** routine to return the array index of a species pointer */

int species_index(spec)
    struct species *spec;
{
    int i=0;

    while (i < num_specs && &specs[i] != spec)
	i++;

    return((i == num_specs) ? -1 : i);
}

