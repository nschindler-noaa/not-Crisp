/** \file
 * \brief Code for dealing with storage basins at dams.
 *
 * The interdependence
 *	between basin volume and basin outflow is handled as follows:
 * [Fill this in. :-)]
 *
 * Non-static Functions:
 *	add_basin - for adding a basin to a dam segment
 *	remove_basin - for removing a basin from a dam segment
 *	make_storage_basin_menu - For creating a menu_item with an
 * associated pullright menu containing all of the active storage basins.
 *	fill_basin - To initialize a storage basin to a certain volume.
 *	set_basin_outflow - to set an outflow value for a particular
 * storage basin (modifies volume to match outflow)
 *	set_basin_volume - to set a volume value for a particular storage
 * basin (modifies outflow to match volume)
 */

#include <stdlib.h>
#include <stdio.h>
#include "crisp.h"
#include "log.h"
#include "basin.h"

/** Scaling factor for kcfs (flow) to give
	kiloacre-feet (volume).  If you change it here, change in flow.c. */
#define SCALE_FACTOR	1.98347 

/*	Macros to represent the basin values for a given dam. */
#define INFLOW(xxx, yyy)       ((xxx)->up->current->headwater.flow[yyy])
#define OUTFLOW(xxx, yyy)      ((xxx)->current->dam.flow[yyy])
#define B_VOLUME(xxx, yyy)     ((xxx)->current->dam.storage_basin->volume[yyy])

/**
 * \brief Adds a storage basin to a
 *      given dam segment.
 *
 * \return -1 if the segment is not a dam or if the malloc doesn't work
 *      correctly.
 * Returns 0 if all goes as planned.
 */
int add_basin(struct river_segment *dam, float min, float max)
{
    if (dam->seg_type != DAM_SEG) {
        fprintf(stderr,"Tried to add a reservoir to non-dam segment %s!\n",
                dam->seg_name);
        return -1;
    }
    if (!(dam->current->dam.storage_basin = (struct basin *)
        malloc(sizeof(struct basin)))) {
        fprintf(stderr, "Out of memory in add_basin!\n");
        return -1;
    }
    bzero(dam->current->dam.storage_basin, sizeof(struct basin));
    dam->current->dam.storage_basin->min_volume = min;
    dam->current->dam.storage_basin->max_volume = max;
    return 0;
}

/**
 * \brief  Removes all traces of
 *      a storage basin from a dam.
 */

void remove_basin(dam)
        struct river_segment *dam;
{
    if (dam->seg_type != DAM_SEG) {
        log_msg(L_ERROR,
                "Trying to remove reservoir from non-dam segment!\n");
        return;
    }
    if (dam->current->dam.storage_basin) {
        free(dam->current->dam.storage_basin);
        dam->current->dam.storage_basin = 0;
    }
}


/**
 * \brief Initializes the storage basin for the dam parameter to
 *      volume for all DAYS_IN_SEASON, and initializes the min and max
 *      values from the parameters.
 *
 * \return -1 if dam is not a DAM_SEG or if dam does not have a storage basin.
 * Returns 0 on normal completion.
 */
int fill_basin(dam, volume)
        struct river_segment *dam;
        float volume;
{
    register int i;

    if (dam->seg_type != DAM_SEG) {
        log_msg(L_ERROR, "Tried to fill reservoir at non-dam segment %s.",
                dam->seg_name);
        return -1;
    }
    if (!dam->current->dam.storage_basin) {
        log_msg(L_ERROR, "Tried to fill a nonexistent reservoir at %s.\n",
                dam->seg_name);
        return -1;
    }
    if (dam->up->seg_type != HEADWATER_SEG) {
        log_msg(L_ERROR, "Tried to fill a reservoir for a dam %s %s!\n",
                dam->seg_name, "not preceded by a headwater");
        free(dam->current->dam.storage_basin);
        dam->current->dam.storage_basin = (struct basin *) 0;
        return -1;
    }
    for (i = 0 ; i < DAYS_IN_SEASON ; i++) {
        B_VOLUME(dam, i) = volume;
    }
    return 0;
}

/**
 * \brief Calculates
 *	outflow based on inflow and basin volume.
 *
 * \return 1 if volume
 *	requires changing, and 0 if it doesn't.
 */
int calculate_flow(dam, day)
	struct river_segment *dam;
	int day;
{
    int return_code = 0;
    float volume;
    float flow;

    /* Delta volume (for positive outflow) */
    volume = B_VOLUME(dam, day) - B_VOLUME(dam, day+1);

    /* Outflow required to satisfy delta volume */
    flow = volume/SCALE_FACTOR + INFLOW(dam, day);

    if (flow < 0.0) {
	flow = 0.0;
 	B_VOLUME(dam, day+1) = B_VOLUME(dam, day) + OUTFLOW(dam, day)
	    * SCALE_FACTOR;
	return_code = 1;
    }
    OUTFLOW(dam, day) = flow;
    return return_code;
}

