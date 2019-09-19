/* \file
 *
 *	\brief This is the tool which reports on COMPASS's input data.  This file
 * contains only those functions specific to the input data report tool.
 */

#include <stdlib.h>
#include <string.h>
#include "crisp.h"
#include "log.h"
#include "report.h"
#include "unused.h"

/*
 * void destroy_report_list(struct report_struct *list) - frees a report
 * 	list and all of the memory associated with it.  note that while
 *	the list of items is freed, each "item" itself is not, since these
 *	are always strings used elsewhere in the program (like river segment
 *	names).
 */
void destroy_report_list(list)
    struct report_struct *list;
{
    if (list)
    {
	free(list->set);
	free(list->items);
	free(list);
    }
}

/*
 * struct report_struct *get_river_list(int type1, int type2) --
 */
struct report_struct *get_river_list(type1, type2)
    int type1;
    int type2;
{
    struct report_struct *list;
    int count;
    struct river_segment *current;
    
    /* allocate memory */
    list =(struct report_struct *) malloc(sizeof(struct report_struct));
    bzero(list, sizeof(struct report_struct));
    
    /* yes, it's a waste of time to travers the river twice.  But
       it's easier this way. :) */
    
    /* traverse river and find number of segs of either type */
    for (count = 0,current=river_mouth; current; current=current->next)
	if (current->seg_type == type1 || current->seg_type == type2) count++;
    
    /* allocate memory */
    list->items = (char **) malloc(sizeof(char *) * (count+1));
    list->set = (char *) malloc(sizeof(char) * count);
    bzero(list->set, sizeof(char) * count);
    
    /* traverse river and find all segs of either type */
    for (count = 0,current = river_mouth; current; current=current->next)
	if (current->seg_type == type1 || current->seg_type == type2)
	{
	    /* set struct name to be seg->name */
	    list->items[count] = current->seg_name;
	    count++;
	}
    list->items[count] = NULL;
    list->number = count;

    return list;
}

/*
 * struct report_struct *get_dam_spec_list(void)
 */
struct report_struct *get_dam_spec_list()
{
    static struct report_struct *dam_spec_list=0;

    if (!dam_spec_list)
    {
	dam_spec_list = get_river_list(DAM_SEG, POWERHOUSE_SEG);
	dam_spec_list->title = "Dams";
	dam_spec_list->type = REPORT_DAM_SPEC;
    }

    return(dam_spec_list);
}

struct report_struct *get_dam_list()
{
    static struct report_struct *dam_list=0;

    if (!dam_list)
    {
	dam_list = get_river_list(DAM_SEG, POWERHOUSE_SEG);
	dam_list->title = "Dams";
	dam_list->type = REPORT_DAM;
    }

    return(dam_list);
}

struct report_struct *get_headwater_list()
{
    static struct report_struct *headwater_list=0;
    if (!headwater_list)
    {
	headwater_list = get_river_list(HEADWATER_SEG, 0);
	headwater_list->title = "Headwaters";
	headwater_list->type = REPORT_HEADWATER;
    }
    return (headwater_list);
}

/*
 * struct report_struct *get_reach_spec_list(void)
 */
struct report_struct *get_reach_spec_list()
{
    static struct report_struct *reach_spec_list=0;

    if (!reach_spec_list)
    {
	reach_spec_list = get_river_list(REACH_SEG, NULL);
	reach_spec_list->title = "Reaches";
	reach_spec_list->type = REPORT_REACH_SPEC;
    }

    return(reach_spec_list);
}

struct report_struct *get_reach_list()
{
    static struct report_struct *reach_list=0;
    if (!reach_list)
    {
	reach_list = get_river_list(REACH_SEG, 0);
	reach_list->title = "Reaches";
	reach_list->type = REPORT_REACH;
    }
    return (reach_list);
}

struct report_struct *get_species()
{
    int count;
    struct report_struct *species_list=0;

    species_list =
	(struct report_struct *) malloc(sizeof(struct report_struct));
    bzero(species_list, sizeof(struct report_struct));

    species_list->type = REPORT_SPECIES;
    species_list->items = (char **)malloc(sizeof(char *)
		* (num_specs+1));
    species_list->set = (char *)malloc(sizeof(char) * num_specs);
    bzero(species_list->set, sizeof(char) * num_specs);
    
    for(count=0; count < num_specs; count++)
	species_list->items[count] = specs[count].name;
    species_list->items[count] = NULL;
    species_list->number=count;
    
    return(species_list);
}

struct report_struct *get_stocks()
{
    int count;
    struct report_struct *stock_list=0;

    stock_list =
	(struct report_struct *) malloc(sizeof(struct report_struct));
    bzero(stock_list, sizeof(struct report_struct));

    stock_list->type = REPORT_STOCK;
    stock_list->items = (char **)malloc(sizeof(char *)
		* (stocks.num+1));
    stock_list->set = (char *)malloc(sizeof(char) * stocks.num);
    bzero(stock_list->set, sizeof(char) * stocks.num);
    
    for(count=0; count < stocks.num; count++)
    	stock_list->items[count] = stocks.s[count]->name;
    stock_list->items[count] = NULL;
    stock_list->number=count;
    
    return(stock_list);
}

struct report_struct *get_species_list()
{
    static struct report_struct *species_list=0;

    if (!species_list)
    {
	species_list = get_species();
	species_list->title = "Species";
	species_list->type=REPORT_SPECIES;
    }
    return(species_list);
}

struct report_struct *get_stock_list()
{
    static struct report_struct *stock_list=0;

    if (!stock_list)
    {
	    stock_list = get_stocks();
	    stock_list->title = "Stocks";
	    stock_list->type=REPORT_STOCK;
    }
    return(stock_list);
}

struct report_struct *get_species_index()
{
    static struct report_struct *species_list=0;

    if (!species_list)
    {
	species_list = get_species();
	species_list->title = "Species";
	species_list->type=REPORT_SPECIES;
    }
    return(species_list);
}

struct report_struct *get_stock_index()
{
    static struct report_struct *stock_list=0;

    if (!stock_list)
    {
	    stock_list = get_stocks();
	    stock_list->title = "Stocks";
	    stock_list->type=REPORT_STOCK;
    }
    return(stock_list);
}

struct report_struct *construct_transport_list()
{
    struct report_struct *list;
    int count;
    struct river_segment *seg;
    
    /* allocate memory */
    list =(struct report_struct *) malloc(sizeof(struct report_struct));
    bzero(list, sizeof(struct report_struct));
    
    /* count number of transport segs */
    for (count = 0,seg=river_mouth; seg; seg=seg->next)
	if (seg->seg_type == DAM_SEG && seg->current->dam.transport)
	    count++;
    
    /* allocate memory */
    list->items = (char **) malloc(sizeof(char *) * (count+1));
    list->set = (char *) malloc(sizeof(char) * count);
    bzero(list->set, sizeof(char) * count);
    
    /* set up the lists for the appropriate segs */
    for (count = 0,seg = river_mouth; seg; seg=seg->next)
	if (seg->seg_type == DAM_SEG && seg->current->dam.transport)
	{
	    /* set struct name to be seg->name */
	    list->items[count] = seg->seg_name;
	    count++;
	}
    list->items[count] = NULL;
    list->number = count;
    list->title = "Transport";
    list->type = REPORT_TRANSPORT;

    return list;
}

/*
 * update a transport list set field from a previous one of a possibly
 * different size.  assumes the new one starts blank.
 */

void update_transport_list(new, old)
    struct report_struct *new, *old;
{
    int i, j;

    if (old)
	for (i = 0; i < old->number; i++)
	    if (old->set[i])
	    {
		j = 0;
		while (j < new->number && strcmp(old->items[i], new->items[j]))
		    j++;
		if (j < new->number)
		    new->set[j] = 1;
	    }
}

struct report_struct *get_transport_list()
{
    static struct report_struct *transport_list=0;
    	   struct report_struct *transport_list_new;

    /* transports are strange in that they might dynamically change.  we
     * therefore set up a new list each time, and update the selection
     * from the old one if necessary, deleting the old one afterwards.
     */
    transport_list_new = construct_transport_list();
    if (transport_list)
    {
	update_transport_list(transport_list_new, transport_list);
	destroy_report_list(transport_list);
    }
    return(transport_list = transport_list_new);
}

/*
 * void *get_dam_segment(char *name) -
 */
void *get_dam_segment(name)
	char *name;
{
    extern struct river_segment *find_seg_by_name();

    struct river_segment *seg;

    if (!(seg = find_seg_by_name(river_mouth, DAM_SEG, name)))
	seg = find_seg_by_name(river_mouth, POWERHOUSE_SEG, name);
    if (!seg) {
	log_msg(L_ERROR, "Can't find dam segment %s!\n", name);
	return NULL;
    }
    return (void *) seg->current;
}

/*
 * void *get_reach_segment(char *name) -
 */
void *get_reach_segment(name)
	char *name;
{
    extern struct river_segment *find_seg_by_name();

    struct river_segment *seg;

    seg = find_seg_by_name(river_mouth, REACH_SEG, name);
    if (!seg) {
	log_msg(L_ERROR, "Can't find reach segment %s!\n", name);
	return NULL;
    }
    return (void *) seg->current;
}

/*
 * void *get_headwater_segment(char *name) -
 */
void *get_headwater_segment(name)
	char *name;
{
    extern struct river_segment *find_seg_by_name();

    struct river_segment *seg;

    seg = find_seg_by_name(river_mouth, HEADWATER_SEG, name);
    if (!seg) {
	log_msg(L_ERROR, "Can't find headwater segment %s!\n", name);
	return NULL;
    }
    return (void *) seg->current;
}

/*
 * void *get_run(char *name) -
 */
void *get_run(char *UNUSED(name))
{
    return (void *) current;
}

/*
 * void *get_species_by_name(char *name) -
 */
void *get_species_by_name(name)
	char *name;
{
    extern int num_specs;
    int count;

    for (count = 0 ; count < num_specs ; ++count)
	if (!strcmp(specs[count].name, name))
	    return (void *) &specs[count];
    log_msg(L_ERROR, "Couldn't find species %s\n", name);
    return NULL;
}

/*
 * void *get_stock_by_name(char *name) -
 */
void *get_stock_by_name(name)
	char *name;
{
    int count;

    for (count = 0 ; count < stocks.num ; ++count)
	    if (!strcmp(stocks.s[count]->name, name))
	        return (void *) stocks.s[count];
    log_msg(L_ERROR, "Couldn't find stock %s\n", name);
    return NULL;
}

/* 
 * void *get_transport_segment(char *name) -
 */
void *get_transport_segment(name)
	char *name;
{
    extern struct river_segment *find_seg_by_name();

    struct river_segment *seg;

    seg = find_seg_by_name(river_mouth, DAM_SEG, name);
    if (!seg) {
	log_msg(L_ERROR, "Can't find dam segment %s!\n", name);
	return NULL;
    }
    return (void *) seg->current->dam.transport;
}

