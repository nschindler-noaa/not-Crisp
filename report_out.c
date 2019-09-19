/* \file
 *
 *	This module defines the routines which output the data from the
 * report tools to the FILE pointer that allows for display of the
 * information.  It does not include the calling routines for these
 * functions, although perhaps it should.  As a result, the only
 * non-static functions in this file are do_report(), which outputs
 * the value of a line of selected variables, and write_report_header(),
 * which outputs the names of all selected variables. The actual
 * selection process is performed in report_frame.c and will not
 * be discussed here.
 */
#include <stdio.h>

#include "crisp.h"
#include "log.h"
#include "report.h"
#include "alt_report.h"
#include <string.h>

/* SPEC_FLOAT and SPEC_INT are defined in report.h */

#define WRITE_FLOAT(xxx, yyy)	fprintf(xxx, SPEC_FLOAT, yyy)
#define WRITE_INT(xxx, yyy)	fprintf(xxx, SPEC_INT, yyy)

/*
 * int write_period(FILE *fp, PERIOD_LIST *period, length) -- Write a single
 *	period, inserting line breaks as appropriate.  This function is a bit
 *	of a kludge, since the rest of the write functions just write one
 *	entry's worth of data, whereas this one has to figure out the line
 *	length and insert newlines as necessary.
 */
static int write_period(fp, period, length, sub_period)
	FILE *fp;
	PERIOD_LIST *period;
	int length;
	int sub_period;
{
    extern int g_report_line_length;

	int line_length = g_report_line_length;
    char buf[24];

    while (period)
    {
	sprintf(buf, "%d-%d", period->start, period->end);
	if (length + (int)(strlen(buf)) >= line_length) {
	    putc('\n', fp);
	    for (length = 0 ; length < REPORT_PREFIX_WIDTH ; ++length)
		putc(' ', fp);
	}
	length += strlen(buf);
	fprintf(fp, "%s", buf);
	if (period->sub_periods) {
	    sprintf(buf, "%d-%d", period->sub_periods->start,
		    period->sub_periods->end);
	    /* +1 for comma, check now. */
	    if (length+(int)(strlen(buf))+1 > line_length) {
		putc('\n', fp);
		for (length = 0 ; length < REPORT_PREFIX_WIDTH ; ++length)
		    putc(' ', fp);
	    }
	    ++length;
	    putc('(', fp);
	    length = write_period(fp, period->sub_periods, length, TRUE);
	    ++length;
	    putc(')', fp);
	}
	else if (!sub_period) {
	    if (length + 4 >= line_length) {
		putc('\n', fp);
		for (length = 0 ; length < REPORT_PREFIX_WIDTH ; ++length)
		    putc(' ', fp);
	    }
	    length += 4;
	    fprintf(fp, ":%3.2f", period->value);
	}
	if ((period = period->next) != NULL) {
	    putc(',', fp);
	    putc(' ', fp);
	    length += 2;
	}
    }
    return length;
}

/*
 * int report_write_var(FILE *fp, struct report_var *var, char *base,
 *	int arg, int start, int end) -- Write a single variable,
 *	switched off the vartype of the report_var structure.  The
 *	looping occurs because we can not tell from the vartype
 *	whether it represents a single value or an array.
 */
static int report_write_var(fp, var, base, arg, start, end)
    FILE *fp;
    struct report_var *var;
    char *base;
    int arg;
    int start;
    int end;
{
    int i_var;

    switch(var->vartype) {
    case RPT_FLOAT:
	for (i_var = start ; i_var < end ; i_var++)
	{
	    float f_var;

	    bcopy(base + var->offset + i_var * sizeof(float), &f_var,
		  sizeof(float));
	    WRITE_FLOAT(fp, f_var);
	}
	break;
    /* This is a special weird case for float values which are kept in a small
     *  array indexed by reach class. The reason we have to hack it this way
     *  is that the number of reach classes may change at run time.
     */
    case RPT_RC_FLOAT:
	for (i_var = start ; i_var < end ; i_var++)
	{
	    float f_var;

	    bcopy(base + var->offset + i_var * sizeof(float), &f_var,
		  sizeof(float));
	    WRITE_FLOAT(fp, f_var);
	}
	break;
    /* This is a special weird case for float values which are part of equation
     *  structures that are kept in a small array indexed by reach class. 
     *  The reason we have to hack it this way is that the number of reach 
     *  classes may change at run time.
     */
    case RPT_RCEQN_FLOAT:
	for (i_var = start ; i_var < end ; i_var++)
	{
	    float f_var;

	    bcopy(base + var->offset + i_var * sizeof(equation), &f_var,
		  sizeof(float));
	    WRITE_FLOAT(fp, f_var);
	}
	break;
    /* This is a special weird case for int values which are part of equation
     *  structures that are kept in a small array indexed by reach class. 
     *  The reason we have to hack it this way is that the number of reach 
     *  classes may change at run time.
     */
    case RPT_RCEQN_INT:
	for (i_var = start ; i_var < end ; i_var++)
	{
	    int int_var;

	    bcopy(base + var->offset + i_var * sizeof(equation), &int_var,
		  sizeof(int));
	    WRITE_INT(fp, int_var);
	}
	break;
    case RPT_INT:
	for (i_var = start ; i_var < end ; i_var++)
	{
	    int int_var;

	    bcopy(base + var->offset + i_var * sizeof(int), &int_var,
		  sizeof(int));
	    WRITE_INT(fp, int_var);
	}
	break;
    case RPT_DAM_SPEC_FLOAT:
	{
	    float f_var;
	    /* set base to be the start of the species struct array*/
	    struct dam_species *spec_base =
		((struct dam *)base)->species;
	    
	    /* get the actual beginning of that particular record. */
	    spec_base = &spec_base[arg];

	    /* use the offset of the var from the start of that particular */
	    /* record */
	    for (i_var = start ; i_var < end ; ++i_var)
	    {
		bcopy((char *)spec_base + var->offset + i_var * sizeof(float),
		      &f_var, sizeof(float));
		WRITE_FLOAT(fp, f_var);
	    }
	}
	break;
    case RPT_DAM_SPEC_INT:
	{
	    int int_var;
	    /* set base to be the start of the species struct array*/
	    struct dam_species *spec_base =
		((struct dam *)base)->species;
	    
	    /* get the actual beginning of that particular record. */
	    spec_base = &spec_base[arg];

	    /* use the offset of the var from the start of that particular */
	    /* record */
	    for (i_var = start ; i_var < end ; ++i_var)
	    {
		bcopy((char *)spec_base + var->offset + i_var * sizeof(int),
		      &int_var, sizeof(int));
		WRITE_INT(fp, int_var);
	    }
	}
	break;
    case RPT_REACH_SPEC_FLOAT:
	{
	    float f_var;
	    /* set base to be the start of the species struct array*/
	    struct reach_species *spec_base =
		((struct reach *)base)->species;
	    
	    /* get the actual beginning of that particular record. */
	    spec_base = &spec_base[arg];

	    /* use the offset of the var from the start of that particular */
	    /* record */
	    for (i_var = start ; i_var < end ; ++i_var)
	    {
		bcopy((char *)spec_base + var->offset + i_var * sizeof(float),
		      &f_var, sizeof(float));
		WRITE_FLOAT(fp, f_var);
	    }
	}
	break;
    case RPT_PERIOD_LIST:
	(void) write_period(fp, (* (PERIOD_LIST **)((char *)base+var->offset)),
		REPORT_PREFIX_WIDTH, FALSE);
	break;
    case RPT_ALT_FLOAT:
	{
	    struct alt_data *ptr;
	    float f_var;

	    if (!base) return 0;	/* Error creating data pointer! */

	    bcopy((char *) base + arg, &ptr, sizeof(struct alt_data *));
	    if (ptr)
		/* print nothing if no data is available */
	    {
		for (i_var = start ; i_var < end ; ++i_var)
		{
		    bcopy((char *) ptr + var->offset + i_var * sizeof(float),
			  &f_var, sizeof(float));
		    WRITE_FLOAT(fp, f_var);
		}
	    }
	}
	break;
    default:
	return 0;
	break;
    }
    return 1;
}

/*
 * int report_show(FILE *filep, struct report_var *reports, void *item,
 *	int index, char *choices, struct output_array *array_info) - Loops
 *	over all variables that are set in the choice list, using the
 *	array_info information to determine which array indices are to
 *	be displayed for an array, calling report_write_var to do the
 *	actual output.
 */
static int report_show(filep, reports, item, index, choices, array_info)
    FILE *filep;
    struct report_var *reports;
    void *item;
    int index;
    char *choices;
    struct output_array *array_info;
{
    int i;
    int start;
    int end;

    for (i = 0 ; reports[i].name ; ++i)
    {
	if (choices[i])
	{
	    /* Go from array_info->start to array_info->end if i is varnum.
	     * Go from array_info->start2 to array_info->end2 if i is varnum2.
	     * Go from 0 to reports[i].length if it is neither
	     */
	    start = (array_info->varnum == i) ? array_info->start
		: ((array_info->varnum2 == i) ? array_info->start2
		   : 0);
	    end = (array_info->varnum == i) ? array_info->end
		: ((array_info->varnum2 == i) ? array_info->end2
		   : ((reports[i].length) ? reports[i].length : 1));

	    if (reports[i].vartype == RPT_FLOAT || 
            reports[i].vartype == RPT_INT || 
            reports[i].vartype == RPT_RC_FLOAT ||
            reports[i].vartype == RPT_RCEQN_FLOAT ||
            reports[i].vartype == RPT_RCEQN_INT)
		report_write_var(filep, &reports[i], item, 0, start, end);
	    else if (reports[i].vartype == RPT_DAM_SPEC_FLOAT
             || reports[i].vartype == RPT_DAM_SPEC_INT
             || reports[i].vartype == RPT_REACH_SPEC_FLOAT
		     || reports[i].vartype == RPT_PERIOD_LIST
		     || reports[i].vartype == RPT_ALT_FLOAT)
		report_write_var(filep, &reports[i], item, index, start, end);
	    else {
		log_msg(L_WARNING, "Report tool:Invalid vartype\n");
		return 0;
	    }
	}
    }
    putc('\n', filep);
    return 1;
}

/*
 * int write_report_header(FILE *filep, struct report_var *var_list,
 *	char *choices, struct output_array *array_info) -- Loops over all
 * variables set in the choices boolean list, outputing the header info
 * (the names of variables and indices of arrays.)
 */
int write_report_header(filep, var_list, choices, array_info)
    FILE *filep;
    struct report_var *var_list;
    char *choices;
    struct output_array *array_info;
{
    int i, string_index, count, array_index, temp;
    char * cur_str;
    char headers[REPORT_NUM_HEADER_LINES][REPORT_STRING_MAX_LEN];
    char tempstr[REPORT_STRING_MAX_LEN];
    int start;
    int end;

    for (string_index=0;
	 string_index < REPORT_NUM_HEADER_LINES;
	 string_index++)
	*headers[string_index]='\0';

    for (i = 0 ; var_list[i].name ; i++)
    {
	if (choices[i])
	{
	    start = (array_info->varnum == i) ? array_info->start
		: ((array_info->varnum2 == i) ? array_info->start2
		    : 0);
	    end = (array_info->varnum == i) ? array_info->end
		: ((array_info->varnum2 == i) ? array_info->end2
		    : ((var_list[i].length) ? var_list[i].length : 1));

	    /* this array_index hack was inserted a bit late in development to
	       handle array headers correctly.  God it's ugly */
	    for( array_index = start ; array_index < end ; array_index++ )
	    {
		/* Start and end of var_list[i].name and copy each *
		 * underline separated name into the string area,  *
		 * from bottom to top */
		if (var_list[i].length <= 1)
		    strcpy(tempstr, var_list[i].name);
		else
		    sprintf(tempstr, "%s_[%d]", var_list[i].name, array_index);

		cur_str=0;
		for (string_index=REPORT_NUM_HEADER_LINES - 1;
		     string_index >= 0;
		     string_index--)
		{
		    if (cur_str != tempstr)
		    {
			cur_str = strrchr(tempstr, '_');

			if (!cur_str)
			    cur_str=tempstr;
			else
			{
			    *cur_str='\0';
			    cur_str++;
			}

			for (count=temp=strlen(headers[string_index]);
			     count < temp+REPORT_COLUMN_WIDTH-(int)(strlen(cur_str));
			     count++)
			    headers[string_index][count]=' ';
			headers[string_index][count]='\0';
			strcat(headers[string_index], cur_str);
		    }
		    else
		    {
			for (count=temp=strlen(headers[string_index]);
			     count < temp+REPORT_COLUMN_WIDTH;
			     count++)
			    headers[string_index][count]=' ';
			headers[string_index][count]='\0';
		    }
		}
	    }
	}
    }
    for (string_index=0;
	 string_index < REPORT_NUM_HEADER_LINES;
	 string_index++)
    {
	for (count=0; count < REPORT_PREFIX_WIDTH; count++)
	    putc(' ', filep);
	fputs(headers[string_index], filep);
	putc('\n', filep);
    }
    return 1;
}

/*
 * int do_report(FILE *fp, int type, char *choices, void *item, int index,
 *	struct output_array *array_info) -- The wrapper function for the
 * output section of the report tool.  There used to be a reason for the
 * existence of this function, now there is none.  But I like it, so I'm
 * keeping it.
 */
int do_report(fp, var_list, choices, item, index, array_info)
    FILE *fp;
    struct report_var *var_list;
    char *choices;
    void *item;
    int index;
    struct output_array *array_info;
{
    return report_show(fp, var_list, item, index, choices, array_info);
}
