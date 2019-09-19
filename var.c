/** \file
 * \brief Implementation of variable parameter support. */
#include "crisp.h"
#include "sample.h"
#include "var.h"
#include "log.h"
#include "report.h"
#include <stdlib.h>

/* array of points to variable parameters */
variable_parameter **vparams = NULL;
int vparam_count = 0;

/* add variable parameter returns the object on success, NULL on error */
variable_parameter* add_variable_parameter(variable_parameter *vp) {
	int i;

	if (!vp) return NULL;

	/* replace if necessary */
	{
		variable_parameter *old = get_variable_parameter(vp->variable);
		if (old) remove_variable_parameter(old);
	}

	/* add */
	{
		int vp_count = vparam_count + 1;
		variable_parameter **vps = (variable_parameter**) malloc(sizeof(variable_parameter*) * vp_count);

		if (!vps) return NULL;

		/* copy pointers to new array */
		for (i=0; i < vparam_count; i++) vps[i] = vparams[i];
		if (vparams) free(vparams);
		vparams = vps;

		/* insert new variable parameter */
		vparams[vparam_count] = vp;
		vparam_count = vp_count;
	}

	return vp;
}

/* add variable that is to be drawn from a broken stick distribution */
variable_parameter* add_broken_stick_parameter(float *variable, float mean, float low, float high) {
	variable_parameter *vp = (variable_parameter*) malloc(sizeof(variable_parameter));

	if (!vp) return NULL;

	vp->variable = variable;

	vp->type = DIST_BROKEN_STICK;
	vp->parameters.broken_stick.mean = mean;
	vp->parameters.broken_stick.low = low;
	vp->parameters.broken_stick.high = high;

	return add_variable_parameter(vp);
}

/* TODO: this should be faster than a linear search, probably.
 * On the other hand, linear searches are used in the parser all over the place,
 * so I'm not going to worry about this for the time being. */
variable_parameter* get_variable_parameter(float *variable) {
	int i;

	for (i=0; i < vparam_count; i++) {
		if (vparams[i] && vparams[i]->variable == variable) return vparams[i];
	}
	return NULL;
}

/* this function doesn't bother to allocate a new array, since it isn't necessary */
void remove_variable_parameter(variable_parameter *vp) {
	int i;

	for (i=0; i < vparam_count; i++) {
		if (vparams[i] == vp) {
			free(vp);
			break;
		}
	}

	vparam_count--;

	/* shift down remaining variable parameters */
	for (; i < vparam_count; i++) {
		vparams[i] = vparams[i+1];
	}
}


/* free all memory for variable parameters */
void free_variable_parameters() {
	int i;
	for (i=0; i < vparam_count; i++) free(vparams[i]);
	free(vparams);
	vparams = NULL;
	vparam_count = 0;
}

/* sample paramters */
int sample_variable_parameters() {
	int i;

	for (i=0; i < vparam_count; i++) {
		if (!vparams[i] || !vparams[i]->variable) return 1;

		switch (vparams[i]->type) {
		case DIST_BROKEN_STICK:
			{
				const struct broken_stick_dist *p = &vparams[i]->parameters.broken_stick;
				*vparams[i]->variable = (float) broken_stick(p->mean, p->low, p->high);
			}
		break;
		default:
			return 1;
		}
	}

	return 0;
}

/** Handy macro to see if point is in [start, start + length) */
#define IN_BOUNDS(point, start, length)	((long)(point) >= (long)(start) && (long)(point) < (((long)(start)) + ((long)(length))))
/** this utility function checks a list of report_var structs (from report_init.c)
 * to find the name for a variable.
 *
 * msg should be a format string with two %s tokens, the first for msg_param, the
 * second for the name of the parameter
 *
 * error_msg should only have one %s for the msg_param */
void named_parameter_error(void *addr, void *structure, struct report_var *vars, const char *msg, const char *msg_param, const char *error_msg) {
	int i;
	struct report_var *var;

	for (i=0, var = &vars[0]; var->name != NULL; var = &vars[++i]) {
		const long addr_int = (long) addr;
		const long structure_int = (long) structure;
		const long offset_int = (long) var->offset;

		/* special case for reach class-specific variables */
		if (var->vartype == RPT_RC_FLOAT && IN_BOUNDS(addr, structure_int + offset_int, 10*sizeof(float))) {
			log_msg(L_ERROR, msg, msg_param, var->name);
			log_msg(L_ERROR, "error in reach class %d\n", (addr_int - structure_int - offset_int)/sizeof(float));
			return;
		}
		else if (addr_int == structure_int + offset_int) {
			log_msg(L_ERROR, msg, msg_param, var->name);
			return;
		}
	}
	log_msg(L_ERROR, error_msg, msg_param);
}

/** This utility function is designed to figure out where the parameter
 * belongs so that a helpful error message can be printed out.
 *
 * It does this by just exploring all of the data structures and using
 * the report_var structures and arrays from report.h and report_init.c
 * to get the name from the memory address (structure + offset)
 *
 *  It doesn't search all data structures at the moment since stochastic
 * parameters are only found in a couple of them. See the comment at the
 * end of this function body for more information. */
void variable_parameter_error(variable_parameter* vp) {
	int i;
	struct river_segment *seg;
	struct species *spec;
	StockT *stock;
	void *addr = (void*) vp->variable;

	/* check river segments */
	for (seg = river_mouth; seg; seg = seg->next) {
		if (IN_BOUNDS(addr, seg, sizeof(struct river_segment))) {
			switch (seg->seg_type) {
			case REACH_SEG:
				named_parameter_error(addr,
					(void*) &seg->current->reach,
					reach_report_vars,
					"bad stochastic param in reach %s, input %s\n",
					seg->seg_name,
					"dist param error for unknown input in reach %s\n");
			break;
			case DAM_SEG:
			case POWERHOUSE_SEG:
				named_parameter_error(addr,
					(void*) &seg->current->dam,
					dam_report_vars,
					"bad stochastic param in dam/powerhouse %s, input %s\n",
					seg->seg_name,
					"dist param error for unknown input in dam/powerhouse %s\n");
			break;
			case HEADWATER_SEG:
				named_parameter_error(addr,
					(void*) &seg->current->headwater,
					headwater_report_vars,
					"bad stochastic param in headwater %s, input %s\n",
					seg->seg_name,
					"dist param error for unknown input in headwater %s\n");
			break;
			default:
				log_msg(L_ERROR, "dist param error in river segment of unknown type @%d\n", seg);
			break;
			}
			return;
		}

		/* check dam species */
		else if (seg->seg_type == DAM_SEG || seg->seg_type == POWERHOUSE_SEG) {
			int j;
			struct dam_species *dspec;

			for (j=0, dspec = &seg->current->dam.species[j]; j < num_specs; dspec = &seg->current->dam.species[++j]) {
				if (IN_BOUNDS(addr, dspec, sizeof(struct dam_species))) {
					log_msg(L_ERROR, "bad stochastic param in dam %s\n", seg->seg_name);
					named_parameter_error(addr,
						(void*) dspec,
						dam_spec_report_vars,
						"bad stochastic param in dam species %s, input %s\n",
						specs[j].name,
						"dist param error for unknown input in dam species %s\n");
					return;
				}
			}
		}
	}

	/* check species */
	for (i=0, spec = &specs[0]; i < num_specs; spec = &specs[++i]) {
		if (IN_BOUNDS(addr, spec, sizeof(struct species))) {
			/* check generic stock */
			if (IN_BOUNDS(addr, &spec->stock, sizeof(StockT))) {
				named_parameter_error(addr,
					(void*) &spec->stock,
					stock_report_vars,
					"bad stochastic param in species %s, input %s\n",
					spec_names[i],
					"dist param error for unknown input in generic stock for species %s\n");
				return;
			}

			/* check species parameters */
			named_parameter_error(addr,
				(void*) spec,
				species_report_vars,
				"bad stochastic param in species %s, input %s\n",
				spec_names[i],
				"dist param error for unknown input in species %s\n");
			return;
		}
	}

	/* check stocks */
	for (i=0, stock = stocks.s[0]; i < stocks.num; stock = stocks.s[++i]) {
		if (IN_BOUNDS(addr, stock, sizeof(StockT))) {
			named_parameter_error(addr,
				(void*) stock,
				stock_report_vars,
				"bad stochastic param in stock %s, input %s\n",
				stock->name,
				"dist param error for unknown input in stock %s\n");
			return;
		}
	}

	/* TODO: releases or others should be checked if variable parameter
	 * support is added for things other than river segments, species,
	 * and stocks */

	/* couldn't figure the parameter's name out... */
	switch (vp->type) {
	case DIST_BROKEN_STICK:
		log_msg(L_ERROR, "bad broken stick params on unknown input: (%f, %f, %f)\n",
			vp->parameters.broken_stick.low,
			vp->parameters.broken_stick.mean,
			vp->parameters.broken_stick.high);
	break;
	default:
		log_msg(L_ERROR, "bad stochastic param error on unknown input of unknown distribution type\n");
	}
}

/** check paramters by looping through them all and running param_error
 * on the broken stick ones. */
int check_variable_parameters() {
	static char msg[256];
	int i;

	for (i=0; i < vparam_count; i++) {
		if (!vparams[i] || !vparams[i]->variable) {
			log_msg(L_ERROR, "distribution for undefined parameter found\n");
			return 1;
		}

		switch (vparams[i]->type) {
		case DIST_BROKEN_STICK:
			{
				if (param_error(vparams[i]->parameters.broken_stick.low,
						vparams[i]->parameters.broken_stick.mean,
						vparams[i]->parameters.broken_stick.high,
						&msg[0])) {

					variable_parameter_error(vparams[i]); /* print error */
					if (msg[0] != '\0') log_msg(L_ERROR, "%s\n", (char*) msg);

					return 1;
				}
			}
		break;
		default:
			log_msg(L_ERROR, "unknown stochastic parameter type found\n");
			return 1;
		}
	}

	return 0;
}

