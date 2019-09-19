/*
 * The preprocessor for flow archives.  See the CRiSP Preprocessor chapter
 * in the User's Manual.
 * 
 * A few formats require some extra contortions which are not obvious or
 * well documented... will remedy that when I have some time.  Of note
 * are calls to divide_sor_spill() and free_water_year().
 *
 * Due to the fact that I've never seen any kind of formal specification
 * for the input formats crisppp attempts to parse, I can't guarrantee
 * that it will be able to figure anything out.  It doesn't help that
 * all this code has been kludged and hacked each time the output format
 * specification has changed, or some new kind of input format turns up,
 * either.
 *
 * If someone wants to contribute something in the way of descriptions
 * of crisppp's inputs (hopefully including what generates them and what
 * they can be counted on to look like), please drop me e-mail.
 *
 * -Tom Bunch	       tom@buck.cqs.washington.edu
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#include "crisppp.h"

static flow_data_t *flow_dat_p;

/* open the appropriate input file. */
FILE *open_input(argc, argv)
	int argc;
	char *argv[];
{
    FILE *input_file;
    
    if ((argc > 1) && (strcmp(argv[1], "-"))) {
	if ((input_file = fopen(argv[1], "r")) == NULL) {
	    perror("Error opening input file:");
	    exit(BADPARAMS);
	}
    }
    else
	input_file = stdin;	

    return input_file;
}

/* Open the appropriate output file */
FILE *open_output(argc, argv)
	int argc;
	char *argv[];
{
    FILE *output_file;

    if ((argc > 2) && (strcmp(argv[2], "-"))) { 
	if ((output_file = fopen(argv[2], "w")) == NULL) {
	    perror("Error opening output file:");
	    exit(BADPARAMS);
	}
    }
    else
	output_file = stdout;

    return output_file;
}
    
/* Parse arguments, prep I/O, and cruise *//*
main(argc, argv)
	int argc;
	char *argv[];

{
    FILE *input_file;
    FILE *output_file;
    int entry_count; 
    
    if (argc > 3) {
	fprintf(stderr, "Usage: dump [inputfile [outputfile]\n");
	exit(BADPARAMS);
    }	

    input_file = open_input(argc, argv);
    output_file = open_output(argc, argv);

    flow_dat_p = (flow_data_t *) malloc(sizeof(flow_data_t));
    bzero(flow_dat_p, sizeof(flow_data_t));

    entry_count = parse_input(input_file);
    close(input_file);

    write_flow_archive(output_file, argv[1], entry_count);
    close(output_file);

    free_flow_struct();
}	
    
*/
/* Master control, here.  Figure out what kind of file based on the
   header, then called appropriate function to parse it. */
int parse_input(input_file)
	FILE *input_file;
{
    char 	input_line[256];
    int  	counter;
    char 	*token;

    if (fgets(input_line, 256, input_file) == NULL) {
	perror("Error reading input file:");
	exit(BADPARAMS);
    }
    
    /* I make the following assumptions about file headers, here:
     *
     *		HYSSRFILE  - column 120, line 1 reads "NPDPE"
     *		HYSSRFILE2 - column 1, line 1 reads "PROJ"
     * 		SAMFILE	   - contains colons in line 1.
     *		HYDSIM	   - We attempt to parse anything else that
     *			isn't one of the above formats but contains
     *			a string lit "SOR" or "SOS" in the first
     *			non blank line as a HYDROSIM file.
     */

    /* See if it's the another format of HYSSR file */
    if (!strncmp(&input_line[119], "NPDPE", 5)) {
	int entry_count;
	flow_dat_p->input_type = HYSSRFILE;
	entry_count = parse_hyssr_data(input_file, input_line);
	divide_sor_spill();
	return entry_count;
    }

    /* See if it's a HYSSR file */
    if (!strncmp(input_line, "PROJ", 4))
    {
	flow_dat_p->input_type = HYSSRFILE2;
	return parse_hyssr_data2(input_file);
    }

    /* See if it's a Sam file */
    if ((token = (char *) rindex(input_line, ':'))) {
	flow_dat_p->input_type = SAMFILE;
	token++;
	/* Input point points at start of game count, 0 games cant happen */
	if (!(flow_dat_p->games = atoi(token))  )
	{
	    fprintf(stderr, "Warning: unable to find game count in header.\n");
	    exit (BADPARAMS);
	}
	/* Get date */
	flow_dat_p->date = (char *)
	    malloc(strlen(fgets(input_line, 256, input_file)) + 1);	
	strcpy(flow_dat_p->date, input_line);
	return parse_sam_data(input_file);
    }
    
    /* I hope this is a reliable way to find SOR files */
    do
    {
	if ((token = strtok(input_line+1, space)) && !strncmp(token, "SO", 2))
	{
	    int entry_count;
	    flow_dat_p->input_type = HYDROSIM;
	    flow_dat_p->years = 0; /* automatically upped later */
	    flow_dat_p->date = "";
	    entry_count =  parse_sor_data(input_file);
	    divide_sor_spill();
	    return entry_count;
	} 
    }
    while (fgets(input_line, 256, input_file) != NULL);

    fprintf(stderr, "ERROR: Unknown flow archive file type.  Aborting.\n");
    exit(BADHEADER);
}

/* Parse a line of data and put it in the structure */
parse_sam_data(input_file)
	FILE *input_file;
{
    char input_line[200], damname[7];
    int  cur_year, entry_count;
    dam_period_t *cur_period;
    
    entry_count = 0;
    while (fgets(input_line, 200, input_file) != NULL) {
	if (!(cur_period = (dam_period_t *) malloc(sizeof(dam_period_t)))) {
	    fprintf(stderr, "ERROR: insufficient memory.\n");
	    exit(1);
	}
	bzero(cur_period, sizeof(dam_period_t));
	if (sscanf(input_line, "%d %*3c%d%*d%4d%*d %6c%*4d%*f%f%f%f%*f\n",
		   &cur_period->period, &cur_period->year, 
		   &cur_year, damname, &cur_period->plan_spill, 
		   &cur_period->over_spill, &cur_period->total	
		   ) == 7) 
	    if (!strcmp(strcpy(cur_period->dam, dam_match(damname)),"")) {
		fprintf(stderr, "Unrecognized dam: %s.\n", damname);
		free(cur_period);
	    }
	    else {
		cur_period->period = (cur_period->period + 9) % 14;
		if (cur_period->period == 10)
		    cur_year--; /* I don't understand it either */
		entry_count++;
		to_power_year(cur_year, cur_period, ALL);
	    }
    }
    return entry_count;
}

parse_sor_data(input_file)
	FILE *input_file;
{
    char input_line[200], damname[7], space[3], damcode[4], *token;
    int entry_count, expect, tmp, first_period, water_year;
    dam_period_t *cur_period;
    void free_water_year();
    
    sprintf(space, " \t");
    entry_count = tmp = 0; 
    expect = DAM;
    first_period = -1;
    cur_period = (dam_period_t *) 0;
    
    while (fgets(input_line, 200, input_file) != NULL) {
	char what[16];
	int i;
	
	if (*input_line != '\n')
	    switch (expect)
	    {
	      case SOR:
		token = strtok(input_line+1, space);
		
		if (!strncmp(token, "SO", 2)) 
		    expect = DAM;
		break;
		
	      case DAM:	
		if (sscanf(input_line+1, " TABLE %6c %s", damname, what) == 2) 
		    /* Have found table data.  Are done with this file. */
		    return entry_count;
		
		if (sscanf(input_line+1, "%*d %6c %s", damname, what) != 2) 
		    fprintf(stderr,"ERROR: unrecognized data header:\n>%s\n",
			    input_line);
		else
		{
		    damname[6] = '\0';
		    strcpy(damcode, dam_match(damname));
		    expect = PERIODS;
		}
		if (!strcmp(damcode, ""))  /* Flush to a new dam */
		    while (fgets(input_line, 200, input_file) != NULL)
		    {
			if ((token = strtok(input_line+1, space)))
			    if (!strncmp(token, "SO", 2))
			    {
				expect = DAM;
				break;
			    }
		    }	
		break;
		
	      case PERIODS:
		if (first_period < 0)
		{
		    token = strtok(input_line+1, space);
		    if (!strcmp(token, "SEP")) 
			first_period = 10;
		    else if (!strcmp(token, "JULY"))
			first_period = 7;
		    else if (!strcmp(token, "AUG"))
		    {
			token = strtok((char *) 0, space);
			if (!strcmp(token, "15"))
			    first_period = 8;
			else
			    first_period = 9;
		    }
		    else {
			fprintf(stderr, "ERROR: Bad data format\n");
			exit (BOGUSDATA);
		    }
		}
		expect = DATA;
		break;
		
	      case DATA: 
		i = 0;
		
		if (!cur_period)
		    if (!(cur_period =
			  (dam_period_t *) malloc(sizeof(dam_period_t))))
		    {
			fprintf(stderr, "ERROR: Insufficient memory.\n");
			exit(1);	
		    }
		bzero(cur_period, sizeof(dam_period_t));
		token = strtok(input_line+1, space);
		do
		{
		    if (!isdigit(*token))
			break;
		    water_year = atoi(token);
		    
		    if (!i)
		    { 
			for (i = first_period; i < 14; i++)  
			    if (!(token = strtok((char *) 0, space)))
				fprintf(stderr,
					"%s: Premature end of data at %s, %d\n",
					damname, cur_period->year);
			i = 0;
		    } else { 
			if (i && (i != first_period))
			{
			    fprintf(stderr,
				    "ERROR: Bad data continuation format.\n");
			    exit(BOGUSDATA);
			}
			water_year--;
		    }
		    
		    while(token = strtok((char *) 0, space)) {
			unsigned short insert_mask;
			if (isdigit(*token)) 
			    switch(*what)
			    {
			      case 'E':
				sscanf(token, "%f", &cur_period->elev);
				insert_mask = ELEV;
				break;
			      case 'O':
				cur_period->period = i;
				strcpy(cur_period->dam, damcode);
				sscanf(token, "%f", &cur_period->total);
				insert_mask = TOTAL;
				break;
			      case 'S': case 'F':
				sscanf(token, "%f", &cur_period->plan_spill);
				insert_mask = PLAN_SPILL;
				break;
			    }
			else
			    fprintf(stderr, 
				    "ERROR: Bad data format.\n>%s\n", token);
			cur_period->period = i;
			cur_period->year = water_year;
			strcpy(cur_period->dam, damcode);
			if (i == 13)
			{
			    i = 0;
			    water_year++;
			} else
			    i++;
			if (to_power_year(0, cur_period, insert_mask))
			    free(cur_period);
			else
			    entry_count++;
			if (!(cur_period = (dam_period_t *)
			      malloc(sizeof(dam_period_t))))
			{
			    fprintf(stderr, "%s: Insufficient memory.\n",
				    "ERROR");
			    exit(1);	
			}
			bzero(cur_period, sizeof(dam_period_t));
		    }
		} while ((fgets(input_line, 200, input_file) != NULL) &&
			 (token = strtok(input_line+1, space)) &&
			 (isdigit(*token)));
		free_water_year(0, water_year);
		while (!token || isdigit(*token)) {
		    if (fgets(input_line, 200, input_file) == NULL) 
			return entry_count;
		    token = strtok(input_line+1, space);
		}
		if (!strncmp(token, "SO", 2)) 
		    expect = DAM;
		else
		    expect = SOR;
		break;
	    }
    }
    return entry_count;
}


divide_sor_spill()
{
    power_year_t *cur_py;
    water_year_t *cur_wy; 
    int i, j;
    for (cur_py = flow_dat_p->annum; cur_py; cur_py = cur_py->next)
	for (cur_wy = cur_py->water_year; cur_wy; cur_wy = cur_wy->next)
	    for (j = 0; j < 14; j++)
		for (i = MAXDAMS-1; i >= 0; i--)
		    if (cur_wy->periods[i][j])
			if (cur_wy->periods[i][j]->total != 0.0)
			    cur_wy->periods[i][j]->plan_spill =
				cur_wy->periods[i][j]->plan_spill /
				cur_wy->periods[i][j]->total;
}


int parse_hyssr_data(input_file, first_line)
	FILE *input_file;
	char *first_line;
{
    char input_line[200];
    char damname[10], dam_code[4], *token;
    int inserting;
    dam_period_t *cur_period;

    strcpy(input_line, first_line);

    while(1)
    {
	inserting = 0;
	
	while (strtok(input_line, space) == NULL)
	    if (fgets(input_line, 200, input_file) == NULL)
		return 0;
	
	if (strncmp(&input_line[119], "NPDPE", 5))
	    return 0;
	
	sscanf(&input_line[58], "%s", damname);
	if (fgets(input_line, 200, input_file) == NULL)
	{
	    fprintf(stderr, "Unexpected end of file\n");
	    exit(BOGUSDATA);
	}
	/* REGULATED FLOW -> total spill 
	 * END-OF-MONTH ELEVATION - FT -> elevation 
	 * SPILL - CFs -> Planned spill
	 */
	
	if (!strncmp(&input_line[58], "REGULATED FLOW", 14))
	{
	    inserting = TOTAL;
	    if (!flow_dat_p->date)
	    {
		flow_dat_p->date = (char *) malloc(9);
		strncpy(flow_dat_p->date, &input_line[119], 8);
		flow_dat_p->date[8] = '\0';
	    }
	}
	else if (!strncmp(&input_line[58], "END-OF-MONTH ELEVATION - FT", 25))
	    inserting = ELEV;
	else if (!strncmp(&input_line[58], "SPILL - CFs", 11))
	    inserting = PLAN_SPILL;
	else
	    inserting = 0;
	
	if (inserting && strcmp(strcpy(dam_code, dam_match(damname)), ""))
	{ 
	    int year, counter;
	    
	    /* Find the first line of data. */
	    do
		if (fgets(input_line, 200, input_file) == NULL)
		{
		    fprintf(stderr, "Unexpected end of file\n");
		    exit(BOGUSDATA);
		}
	    while (!isdigit(input_line[0]));
	    
	    /* Pluck out the year and skip the half year at start of data */
	    sscanf(input_line, "%*d-%d", &year);
	    (void) strtok(input_line, space);
	    for (counter = 0; counter < 7; counter++)
		(void) strtok(NULL, space);
	    
	    for (counter = 0; counter < 15; counter++)
	    {
		if (counter == 7)
		{
		    if (fgets(input_line, 200, input_file) == NULL)
			return 0;
		    
		    if (!isdigit(input_line[0])) /* End of a table */
		    {
			extern void free_water_year();	
			free_water_year(0, year);	/* kind of lame */
			
			do
			    if (fgets(input_line, 200, input_file) == NULL)
				return 0;
			while (input_line[0] != ' ');
			break;
		    }
		    (void) strtok(input_line, space);
		}
		if (counter == 14)
		{
		    counter = 0;
		    year++;
		}
		
		if (!(cur_period =
		      (dam_period_t *) malloc(sizeof(dam_period_t))))
		{
		    fprintf(stderr, "ERROR: Insufficient memory.\n");
		    exit(1);
		}	
		bzero(cur_period, sizeof(dam_period_t));
		strcpy(cur_period->dam, dam_code);
		cur_period->period = counter;
		cur_period->year = year;
		{
		    switch (inserting)
		    {
		      case TOTAL:
			sscanf(token = strtok(NULL, space), "%f",
			       &(cur_period->total));
			break;
		      case ELEV:
			sscanf(token = strtok(NULL, space), "%f",
			       &(cur_period->elev));
			break;
		      case PLAN_SPILL:
			sscanf(token = strtok(NULL, space), "%f",
			       &(cur_period->plan_spill));
			break;
		    }
		    
		}
		to_power_year(0, cur_period, inserting); 
	    }
	}
	else
	{
	    do
		if (fgets(input_line, 200, input_file) == NULL)
		    return 0;
	    while (input_line[0] != ' ');
	}
    }
}

parse_hyssr_data2(input_file)
	FILE *input_file;
{
    char input_line[200], damname[5], dam_code[4], what[5];
    int inserting, year, counter;
    dam_period_t *cur_period;
    float afloat;
    
    while (fgets(input_line, 200, input_file))
    {
	if (isupper(input_line[1]))
	{
	    inserting = 0;
	    sscanf(input_line, "%s %d %s", damname, &year, what);
	    if (*what == 'Q')
		inserting = TOTAL;
	    if (*what == 'E')
		inserting = ELEV;
	    if (*what == 'S')
		inserting = PLAN_SPILL;
	    if (inserting) {
		strtok(input_line, space);
		for (counter=0; counter < 8; counter++)
		    (void) strtok(NULL, space);
		if (strcmp(strcpy(dam_code, dam_match(damname)), "")) {
		    for (counter=0; counter < 4; counter++)
		    {
			if (!(cur_period =
			      (dam_period_t *) malloc(sizeof(dam_period_t))))
			{
			    fprintf(stderr, "ERROR: Insufficient memory.\n");
			    exit(1);
			}
			bzero(cur_period, sizeof(dam_period_t));
			switch(inserting) {
			  case TOTAL:
			    cur_period->total =
				(float) atof(strtok(NULL, space));
			    break;
			  case ELEV:
			    cur_period->elev =
				(float) atof(strtok(NULL, space));
			    break;
			  case PLAN_SPILL:
			    cur_period->plan_spill =
				(float) atof(strtok(NULL, space));
			    break;
			}
			strcpy(cur_period->dam, dam_code);
			cur_period->period = counter;
			cur_period->year = year;
			to_power_year(0, cur_period, inserting);
		    }
		    if (fgets(input_line, 200, input_file) == NULL)
		    {
			fprintf(stderr, "Corrupt file at year: %d, dam: %s\n",
				year, damname);
			exit(BOGUSDATA);
		    }
		    (void) strtok(input_line, space);
		    for (counter=0; counter < 2; counter++)
			(void) strtok(NULL, space);
		    for (counter=4; counter < 14; counter++) {
			if (!(cur_period =
			      (dam_period_t *) malloc(sizeof(dam_period_t))))
			{
			    fprintf(stderr, "ERROR: Insufficient memory.\n");
			    exit(1);
			}
			bzero(cur_period, sizeof(dam_period_t));
			switch(inserting) {
			  case TOTAL:
			    cur_period->total =
				(float) atof(strtok(NULL, space));
			    break;
			  case ELEV:
			    cur_period->elev =
				(float) atof(strtok(NULL, space));
			    break;
			  case PLAN_SPILL:
			    cur_period->plan_spill =
				(float) atof(strtok(NULL, space));
			    break;
			}
			strcpy(cur_period->dam, dam_code);
			cur_period->period = counter;
			cur_period->year = year;
			to_power_year(0, cur_period, inserting);
		    }	
		}
	    }
	}
    }
    return 0;
}	

/* return std abbreviation for damname */
char *dam_match(damname)/*[4]*/
	char damname[7];
{
    int index;

    if (!strcmp(damname, "")) 
	return "";
    for (index=0; index<MAXDAMS; index++) 	
	switch (flow_dat_p->input_type) {
	  case SAMFILE: 
	    if(!strncmp(dam_match_array[index].samname, damname, 6)) {
		if (strcmp(dam_match_array[index].code, ""))
		    flow_dat_p->dams[index] = 1;
		return dam_match_array[index].code;
	    }
	    break;
	  case HYDROSIM:
	    if(!strncmp(dam_match_array[index].sorname, damname, 6)) {
		if (strcmp(dam_match_array[index].code, ""))
		    flow_dat_p->dams[index] = 1;
		return dam_match_array[index].code;
	    }
	    break;
	  case HYSSRFILE:
	    if(!strncmp(dam_match_array[index].hysname, damname, 6)) {
		if (strcmp(dam_match_array[index].code, ""))
		    flow_dat_p->dams[index] = 1;
		return dam_match_array[index].code;
	    }
	    break;
	  case HYSSRFILE2:
	    if(!strncmp(dam_match_array[index].hysname2, damname, 6)) {
		if (strcmp(dam_match_array[index].code, ""))
		    flow_dat_p->dams[index] = 1;
		return dam_match_array[index].code;
	    }
	    break;

	}
    return "";
}

/* Returns an index into the dam_match_arry for this dam. */
int std_dam_index(damcode)
	char damcode[4];
{
   int index;

   for (index = 0; index<MAXDAMS; index++)
       if (!strcmp(damcode, dam_match_array[index].code))
	   return index;
   return -1;
}

/* Data insertion/sorting routines. */

/* Put the current game into the appropriate place in the power year */
to_water_year(cur_period_p, water_year_p, insert_mask)
	dam_period_t *cur_period_p;
	water_year_t **water_year_p;
	unsigned short insert_mask;
{
    water_year_t *tmp, *new_year;
    int cur_year = cur_period_p->year;
    int period = cur_period_p->period;
    int dam = std_dam_index(cur_period_p->dam);

    /* Find year or spot for it */
    while (*water_year_p && ((*water_year_p)->year < cur_year)) 
	water_year_p = &(*water_year_p)->next;

    /* If we didn't find the year, malloc and insert it */
    if ((!*water_year_p) || ((*water_year_p)->year != cur_year)) {
	new_year = (water_year_t *) malloc(sizeof(water_year_t));
	bzero(new_year, sizeof(water_year_t));
	new_year->year = cur_year;
	flow_dat_p->games++;
	tmp = *water_year_p;
	*water_year_p = new_year;
	(*water_year_p)->year = cur_year;
	new_year->next = tmp;
    }
    if (!((*water_year_p)->periods[dam][period]))
	(*water_year_p)->periods[dam][period] = cur_period_p;
    else
    {
	if (insert_mask & TOTAL)
	    (*water_year_p)->periods[dam][period]->total
		= cur_period_p->total;
	if (insert_mask & PLAN_SPILL)
	    (*water_year_p)->periods[dam][period]->plan_spill
		= cur_period_p->plan_spill;
	if (insert_mask & ELEV)
	    (*water_year_p)->periods[dam][period]->elev
		= cur_period_p->elev;
	if (insert_mask & OVER_SPILL)
	    (*water_year_p)->periods[dam][period]->over_spill
		= cur_period_p->over_spill;
	return 1;
    }
    return 0;
}

/* find or malloc and insert the correct power_year for this game.
   Pass year to insert_sort to insert the game itself. */

int to_power_year(cur_year, cur_period, insert_mask)
	int cur_year;
	dam_period_t *cur_period;
	unsigned short insert_mask;
{
    power_year_t **power_year_p, *tmp, *new_year;
    
    power_year_p = &(flow_dat_p->annum);

    /* Find year or spot for it */
    while (*power_year_p && ((*power_year_p)->year < cur_year)) 
	power_year_p = &(*power_year_p)->next;

    /* If we didn't find the year, malloc and insert it */
    if ((!*power_year_p) || ((*power_year_p)->year != cur_year)) {
	new_year = (power_year_t *) malloc(sizeof(power_year_t));
	bzero(new_year, sizeof(power_year_t));
	new_year->year = cur_year;
	flow_dat_p->years++;
	tmp = *power_year_p;
	*power_year_p = new_year;
	new_year->next = tmp;
    }
    return to_water_year(cur_period,&(*power_year_p)->water_year,insert_mask);
}

    
/* Routines to free things.  Crisppp has memory leaks, but has such a
 * short life span it shouldn't matter.
 */
void free_water_year(power_year, water_year)
	int power_year;
	int water_year;
{
    power_year_t *cur_py;
    water_year_t *cur_wy, *last_wy; 
    int i, j;

    last_wy = 0;
    for (cur_py = flow_dat_p->annum; cur_py; cur_py = cur_py->next)
	if (cur_py->year == power_year)
	    for (cur_wy = cur_py->water_year; cur_wy; cur_wy = cur_wy->next)
	    {
		if (cur_wy->year == water_year)
		{
		    for (j = 0; j < 14; j++)
			for (i = MAXDAMS-1; i >= 0; i--)
			    if (cur_wy->periods[i][j])
				free(cur_wy->periods[i][j]);
		    if (last_wy)
			last_wy->next = cur_wy->next;
		    else
			cur_py->water_year = cur_wy->next;
		    free(cur_wy);
		    flow_dat_p->games--;
		    return;
		}
		last_wy = cur_wy;
	    }
}

free_power_years(pyears)
	struct _power_year *pyears;
{
    if (pyears) {
	free_power_years(pyears->next);
	free(pyears);
    }
}
	
free_flow_struct()
{
    free_power_years(flow_dat_p->annum);
    free(flow_dat_p->date);
    free(flow_dat_p);
}

/* Output everything. */
write_flow_archive(output_file, in_filename, entry_count)
	FILE *output_file;
	char *in_filename;
	int entry_count;
{
    int dam_count, period_count, i;
    char *pp;
    char cur_time[26];
    time_t clock;

    if (!(pp = (char *) rindex(in_filename, '/')))
	pp = in_filename;
    else
	pp++;

    time(&clock);
    strcpy(cur_time, ctime(&clock));
    cur_time[24] = '\0';	/* wipe stupid new line character */

    fprintf(output_file, "%s %s %s", VERSION, PROGRAM, cur_time);
    switch (flow_dat_p->input_type) {
	case SAMFILE: fprintf(output_file, " %s", "SAM"); break;
	case HYDROSIM: fprintf(output_file, " %s", "SOR"); break;
	case HYSSRFILE: fprintf(output_file, " %s", "HYSSR"); break;
	case HYSSRFILE2: fprintf(output_file, " %s", "HYSSR2"); break;
	default: fprintf(stderr, "Error #01 -- this can't happen.\n"); break;
      }
    fprintf(output_file, " %s %s\n\n", pp, flow_dat_p->date); /* Null date in Hyssr */

    fprintf(output_file, "games\t%d\t # number of games\n",
	    flow_dat_p->games);
    fprintf(output_file, "years\t%d\t # number of years\n\n",
	    flow_dat_p->years);	

    fprintf(output_file, "%s\n\n", dam_list(flow_dat_p->dams, &dam_count));

    for (i = 0; i < MAXDAMS; ++i)
	if (flow_dat_p->dams[i]) {
	    fprintf(output_file,
		   period_list(flow_dat_p->annum->water_year->periods[i],
			       &period_count)
		    );
	    break;
	}

    /* Check out the sanity of the data. */
/*	I'll have to do a proper job of this soon 
    if ((entry_count % (dam_count * period_count)) != 0) {
	fprintf(stderr, "Warning -- data failed sanity check.\n");
	fprintf(stderr, "dams: %d, periods %d, entries: %d\n", 
		dam_count, period_count, entry_count);
	fprintf(stderr, "Proceeding\n");
    }
*/

    fprintf(output_file, "#period water   power   dam       elev   plan   over      tot\n");
    fprintf(output_file, "#       year    year                    spill  spill     flow\n\n");
    fprintf(output_file, "DATA\n");

    print_flow_data(output_file, flow_dat_p->annum);
}

print_flow_data(output_file, year_data)
	FILE *output_file;
	power_year_t *year_data;
{
    if (year_data) {
	print_year_data(output_file, year_data->water_year, year_data->year);
	print_flow_data(output_file, year_data->next);
    }
}

print_year_data(output_file, water_period, year)
	FILE *output_file;
	water_year_t *water_period;
	unsigned int year;
{
    int i, j;
    dam_period_t *tmp;

    while (water_period) {
	for (j = 0; j < 14; j++)
	    for (i = MAXDAMS-1; i >= 0; i--)
		if (tmp = water_period->periods[i][j]) {
		    fprintf(output_file,
			    "%-8s%-4d    %-4d    %-8s%6.1f %6.4f %6.4f %8.1f\n",
			    period_string(tmp->period), tmp->year,
			    year, tmp->dam, tmp->elev, tmp->plan_spill,
			    tmp->over_spill, tmp->total);
		}
	water_period = water_period->next;
    }
}

char *period_string(period)
	int period;
{
    return (period_names[period].code);
}


char *dam_list(dam_mask, counter)
	unsigned short int dam_mask[MAXDAMS];
	int *counter;
{
    char dams[256];
    static char return_string[256];
    int index;

    *dams = '\0';
    for (index = MAXDAMS-1, *counter = 0; (index >= 0); index--)
	if (dam_mask[index]) {
	    strcat(dams, dam_match_array[index].code);
	    strcat(dams, " ");
	    (*counter)++;
	}
    sprintf(return_string, "dams %d ", *counter);
    strcat(return_string, dams);
    return return_string;
}

char *period_list(periods, count)
	dam_period_t *periods[MAXDAMS][14];
	int *count;
{
    int dam, period;
    static char return_string[256];
    char compose_string[256];
    dam_period_t *cur;
    char *tmp;
    
    for (dam = 0; dam < MAXDAMS; dam++) /* Find a dam this file contains */
	if (periods[dam][0])  /* All dams better have a period 0.  */
	    break;

    strcpy(compose_string, "");

    for (period = 0; (period < 14) && (periods[dam][period]); period++) {
	/* Periods are in the list upside down, remember. */
	if (*compose_string)
	    strcat(compose_string, ", ");
	strcat(compose_string, period_names[period].julian);
    }

    /* Now, if the periods don't begin with julian day 0, fudge it */
    if (atoi(compose_string) != 0)
    {
	tmp = (char *) index(compose_string, ':');
	*tmp = '0';
	while (--tmp >= compose_string)
	    *tmp = ' ';
    }

    /* Now fudge it so the last day of the year is 365, no matter what */
    tmp = (char *) rindex(compose_string, ':');
    strcpy(++tmp, "365");

    *count = period;
    sprintf(return_string, "periods %d %s\n\n", *count, compose_string);
    return return_string;	
}

/* Everything below here is for a format we hope we don't have to support */
#ifdef LAME_FORMAT
/* Unused, but preserved just in case. */
int get_int(in)
	char **in;
{
    int retval;
    
    SKIPWHITE(*in);
    if (**in == '\0') return -1;
    retval = atoi(*in);
    while (isdigit(**in)) (*in)++;
    return retval;
}    

/* Unused, but preserved just in case. */
float get_float(in)
	char **in;
{
    float retval;
    
    retval = -1.1;
    SKIPWHITE(*in);
    if (**in == '\0') return retval;
    else if (**in == '.') retval = 0.0;
    else if (sscanf(*in, "%f", &retval) != 1)
	fprintf(stderr, "Warning: Found %c where float expected.\n", **in);
    while (isdigit(**in) || (**in == '.') || (**in == '-')) (*in)++;
    return retval;
}

/* Adds what to the bottom of where */
/* Unused, but preserved just in case. */
putin(what, where)
	int what;	
	int_list **where;
{
    int_list **tmp_l;
    tmp_l = where;
    while(*tmp_l)
	tmp_l = &((*tmp_l)->next);
    if (!(*tmp_l = (int_list *) malloc(sizeof(int_list)))) {
	fprintf(stderr, "ERROR -- insufficient memory.\n");
	exit(1);
    }
    (*tmp_l)->year = what;
    (*tmp_l)->next = (int_list *) 0;
}

/* Unused */
free_int_list(list)
	int_list **list;
{
    if ((*list) && ((*list)->next))
	free_int_list(&((*list)->next));
    free (*list);
    *list = (int_list *) 0;
}

/* Unused
isin(what, where)
	int what;
	int_list *where;
{
    while (where) {
	if (where->year == what) return 1;
	where = where->next;
    }
    return 0;
}
#endif

#ifdef LAME_FORMAT
parse_hyssr_data(input_file)
	FILE *input_file;
{
    char input_line[200], damname[7];
    int  entry_count, scratch;
    dam_period_t *cur_period;
    char *input_pointer;
    unsigned char hyssrmask, whereami, startwhere;
    int water_year, new_year, period, whenami, startwhen;
    float afloat;
    int_list *years, *ty, *p_years;

    entry_count = 0;
    p_years = years = 0;
    while (fgets(input_line, 200, input_file)) {
	if (input_line[0] == '1') {	/* New dam */
	    water_year = startwhere = hyssrmask = 0;
	    free_int_list(&p_years);
	    free_int_list(&years);
	    if (sscanf(input_line, "%d %6s", &scratch, damname) != 2) 
		fprintf(stderr, "Unfathomable input:\n%s\n", input_line);
	    if (!strncmp("LOWER", damname, 5)) {
		if (sscanf(input_line, "%d %*s %6s", &scratch, damname) != 2) 
		    fprintf(stderr, "Unfathomable input:\n%s\n", input_line);
	    }
	    fgets(input_line, 200, input_file); /* Throw it out */
	} else 
	    if (input_pointer = (char *) index(input_line, '-')) {
		period = 7;
		input_pointer++;
		new_year = get_int(&input_pointer);
		if (!isin(new_year, p_years)) putin(new_year, &p_years);
		SKIPBLACK(input_pointer);SKIPWHITE(input_pointer);
		SKIPBLACK(input_pointer);SKIPWHITE(input_pointer);
		/* Ok, that was hokey, but input_pointer should point at the year now */
		new_year = get_int(&input_pointer);
		SKIPWHITE(input_pointer);
		if (new_year != water_year) putin(new_year, &years);
		if (GROK & hyssrmask) {
		    if (!whereami)
			whereami = QOUT;
		    else {
			LEFTSHIFT(whereami);
			while (!(hyssrmask & whereami) || (whereami == GROK))
			    LEFTSHIFT(whereami);
		    }
		} else {
		    if (!strncmp(input_pointer, "Q", 1)) {
			if (QOUT & hyssrmask) hyssrmask |= GROK;
			hyssrmask |= QOUT;
			whereami = QOUT;
		    }
		    else if (!strncmp(input_pointer, "EN", 2)) {
			if (ENERGY & hyssrmask) hyssrmask |= GROK;
			hyssrmask |= ENERGY;
			whereami = ENERGY;
		    }
		    else if (!strncmp(input_pointer, "EL", 2)) {
			if (ELEV & hyssrmask) hyssrmask |= GROK;
			hyssrmask |= ELEV;
			whereami = ELEV;
		    }
		    else if (!strncmp(input_pointer, "SP", 2)) {
			if (SPILLFLO & hyssrmask) hyssrmask |= GROK;
			hyssrmask |= SPILLFLO;
			whereami = SPILLFLO;
		    }
		    else if (!strncmp(input_pointer, "L", 1))
			{hyssrmask |= LOAD; whereami = LOAD;}
		    else if (!strncmp(input_pointer, "SY", 2))
			{hyssrmask |= SYSGEN; whereami = SYSGEN;}
		    else if (!strncmp(input_pointer, "SU", 2))
			{hyssrmask |= SURPLUS; whereami = SURPLUS;}
		    else
			fprintf(stderr, "I don't grok:\n%s\n", input_pointer);
		}	
		SKIPBLACK(input_pointer);
		if ((whereami & ~GROK) > ELEV) 
		    return entry_count; /* This file has outlived its usefulness. */
		if (!startwhere) startwhere = whereami;
		water_year = new_year;
		if ((whereami == QOUT) || (whereami == SPILLFLO) || (whereami == ELEV)) {
		    while (*input_pointer !=  NULL) {
			afloat = get_float(&input_pointer);
			if (!(cur_period = (dam_period_t *) malloc(sizeof(dam_period_t)))) {
			    fprintf(stderr, "ERROR: Insufficient memory.\n");
			    exit(1);
			}
			bzero(cur_period, sizeof(dam_period_t));
			if (!strcmp(strcpy(cur_period->dam, dam_match(damname)),"")) 
			    free(cur_period);
			else {
			    cur_period->period = period;
			    if (!period) 
				water_year++;
			    cur_period->year = water_year;
			    period = ((period +1) % 14);
			    switch  (whereami) {
			      case QOUT:
				cur_period->total = afloat;
				break;
			      case SPILLFLO:	
				cur_period->plan_spill = afloat; /* We'll divide by total later */
				break;
			      case ELEV:
				cur_period->elev = afloat;
				break;
			    }
			    if (isin(cur_period->year, p_years) && (cur_period->period != 13))
				to_power_year(0, cur_period, whereami);
			    else
				free(cur_period);
			}
			SKIPWHITE(input_pointer);
		    }
		    startwhen = period;
		} 
	    }
	    else {
		input_pointer = input_line;
		SKIPWHITE(input_pointer);
		if (!isdigit(*input_pointer)) {
		    whereami = startwhere;
		    ty = years;
		} else {
		    SKIPBLACK(input_pointer);
		    if (!ty) {
			fprintf(stderr, "ERROR: crisppp is confused.\n");
			exit(BOGUSDATA);
		    }
		    whenami = startwhen;
		    water_year = ty->year;
		    if ((whereami == QOUT) || (whereami == SPILLFLO) || (whereami == ELEV)) {
			while (*input_pointer !=  NULL) {
			    afloat = get_float(&input_pointer);
			    if (!(cur_period = (dam_period_t *) malloc(sizeof(dam_period_t)))) {
				fprintf(stderr, "ERROR: Insufficient memory.\n");
				exit(1);
			    }
			    bzero(cur_period, sizeof(dam_period_t));
			    if (!strcmp(strcpy(cur_period->dam, dam_match(damname)),"")) 
				free(cur_period);
			    else {
				cur_period->period = whenami;
				if (!whenami) water_year++;					   
				cur_period->year = water_year;
				whenami = ((whenami +1) % 14);
				switch (whereami) {
				  case QOUT:
				    cur_period->total = afloat;
				    break;
				  case SPILLFLO:
				    cur_period->plan_spill = afloat; /* We'll divide by total later*/
				    break;
				  case ELEV:
				    cur_period->elev = afloat;
				    break;
				}
				if (isin(cur_period->year, p_years) && (cur_period->period != 13))
				    to_power_year(0, cur_period, whereami);
				else
				    free(cur_period);
			    }
			    SKIPWHITE(input_pointer);
			}
		    }
		    do
			LEFTSHIFT(whereami);
		    while (!(hyssrmask & whereami) || (whereami == GROK));
		    if (whereami == QOUT) ty = ty->next;
		}
	    }
    }
    return entry_count;
}
#endif
