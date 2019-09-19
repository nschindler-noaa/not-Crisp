/* \file
 *  \brief Routines and variables needed to implement the model of fish growth.
 */

/** input: length in mm.  output: weight in grams. */
extern double length_to_weight( double length );

/** input: weight in grams.  output: length in mm. */
extern double weight_to_length( double weight );

/** input: weight in grams, water temperature in degrees C, calculated
 *  "p" value.   output: weight gain in grams per day.
 */
extern double growth_fn( double weight, double temp, double pvalue );

/** input: A & B parameters and water temperature in degrees C. 
 *  output: "P" value needed by growth_fn() */
extern double pvalue( double Aparam, double Bparam, double temp );

