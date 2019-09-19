/** \file
 * \brief  New variable parameter support
 *
 * NOTE: This variable parameter support was implemented a while ago and is
 * probably going to be replaced soon at which point it might be a good idea
 * to go ahead and remove all of this code assuming that the broken stick
 * distribution and drawing from it will no longer be used. This code is
 * probably too clever to do any good.
 *
 * var.c and var.h define the new interface for supporting parameters that are
 * sampled from a distribution. Rather than storing the mean, low, and high
 * values directly in the data structure (which implies that the programmer
 * must know exactly which parameters the user may need to vary), this code
 * transparently assigns new values to parameters (of type float) before each
 * simulation. This means that any parameter can be varied in this manner.
 *
 * The implementation is very simple and stores a pointer to the variable, along
 * with information about the distribution.
 *
 * At the moment, there is legacy support, which necessitates the need to have
 * a pointer to the variable's distribution (e.g. turbine_mort_dist) in the main
 * data structure. THIS IS FOR LEGACY PURPOSES ONLY! For new code, unless the
 * distribution must actually be accessed directly (e.g. for FGE equations),
 * then the code should not have to store a pointer to the variable_paramter
 * structure.
 */

#ifndef VAR_H
#define	VAR_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
	DIST_BROKEN_STICK = 1,
/* unimplemented */
#if 0
	DIST_NORMAL,
	DIST_UNIFORM,
#endif
	DIST_ENUM_SIZE
};

/** Variable parameter information structure */
typedef struct {
	int type; /* type of distribution */
	float *variable; /* pointer to the variable that will be set */

	/* distribution-specific parameters */
	union {
		/* broken stick distribution */
		struct broken_stick_dist {
			float mean;
			float low;
			float high;
		} broken_stick;
/* unimplemented */
#if 0
		/* normal distribution */
		struct normal_dist {
			float mean;
			float std_dev;
		} normal;

		/* uniform distribution */
		struct uniform_dist {
			/* maximum and minimum values */
			float a;
			float b;
		} uniform;
#endif
	} parameters;
} variable_parameter;

/** set a parameter to be variable, drawn from a broken stick distribution
 * returns a pointer to the object if successful, NULL on error */
extern variable_parameter* add_broken_stick_parameter(float *variable, float mean, float low, float high);

/** add/replace variable parameter */
variable_parameter* add_variable_parameter(variable_parameter *vp);
/** get a parameter for a specific variable */
extern variable_parameter* get_variable_parameter(float *variable);
/** remove variable parameter for a specific variable */
extern void remove_variable_parameter(variable_parameter *vp);

/** free all variable-parameter-related memory */
extern void free_variable_parameters();

/** sample all variable parameters; this should be done before executing a game
 * \return 0 if successful */
extern int sample_variable_parameters();

/** check stochastic parameters for errors (e.g. invalid low, mean, high for
 * broken stick distributions.
 * \return 0 on success or non-zero on error */
extern int check_variable_parameters();

#ifdef __cplusplus
}
#endif


#endif
