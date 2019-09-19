/* growth.c
 *  This file contains routines and variables needed to implement
 *  the model of fish growth.
 */

#include <math.h>
#ifdef TEST
#include <stdio.h>
#else
#include "crisp.h"
#endif
#include "growth.h"

/* For length-weight interchangability:
 *  log(W) = b0 + b1 * log(L)
 */

#ifndef TEST
/* input: length in mm.  output: weight in grams. */
double length_to_weight( double length ) {
    return exp( current->length_weight_b0 
                     + current->length_weight_b1 * log( length ) );
}

/* input: weight in grams.  output: length in mm. */
double weight_to_length( double weight ) {
    return exp( (log( weight ) - current->length_weight_b0)
                                     / current->length_weight_b1 );
}
#endif

/* input: A & B parameters and water temperature in degrees C. 
 *  output: "P" value needed by growth_fn() */
double pvalue( double Aparam, double Bparam, double temp ) {
    double s;
    s = sin( Aparam + Bparam * temp );
    return s * s;
}

/* input: weight in grams, water temperature in degrees C, calculated
 *  "p" value.   output: weight gain in grams per day.
 */
double growth_fn( double weight, double temp, double pvalue ) {
    /* result. */
    double Gr;

    /* intermediate quantities. */
    double C, R, D, F, U, V, dr, g1, g2, l1, l2, ka, kb;

    /* known quantities. */
    static double va = 9.7;
    static double rk4 = 0.13;
    static double vb = 0.0405;
    static double alpha = 5764; /* or 7602 */
    static double beta = 0.9862; /* or .5266 */
    /* only needed if we reactivate g1 and g2 equations below.
    static double ct0 = 15;
    static double ctm = 18;
     */
    static double cq = 5;
    static double ck1 = 0.36;
    static double ct1 = 24;
    static double ck4 = 0.01;
    static double ca = 0.303;
    static double cb = -0.275;
    static double ra = 0.00264;
    static double rb = -0.217;
    static double rq = 0.06818;
    static double rt0 = 0.0234;
    static double rtm = 0;
    static double Sda = 0.172;
    static double ua = 0.0314;
    static double ub = 0.58;
    static double ug = -0.299;
    static double fa = 0.212;
    static double fb = -0.222;
    static double fg = 0.631;
    static double conv = 13562.64; /* convert oxygen to calories */

    V =  va * pow( weight, rk4 ) * exp( vb * temp );
    dr = alpha + beta * weight;

    /* So long as g1 and g2 are calculated from known constants, we
     *  can reduce them to known constants and save some time.
    g1 = (1 / (ct0 - cq)) * log(0.98 * (1 - ck1) / (0.02 * ck1));
    g2 = (1 / (ct1 - ctm)) * log(0.98 * (1 - ck4) / (0.02 * ck4));
     */
    g1 = 0.4467184;
    g2 = 1.41449;
    l1 = exp( g1 * (temp - cq ) );
    l2 = exp( g2 * (ct1 - temp ) );
    ka = ck1 * l1 / (1 + ck1 * (l1 - 1));
    kb = ck4 * l2 / (1 + ck4 * (l2 - 1));

    C = ca * pow( weight, cb ) * pvalue * ka * kb
#ifdef TEST
                         * 5400;
#else
                         * current->prey_energy_density;
#endif
    F = fa * pow( temp, fb ) * exp( fg * pvalue ) * C;
    R = ra * pow( weight, rb ) * exp( rq * temp ) 
          * exp( (rt0 - rtm * temp) * V ) * conv;
    D = Sda * (C - F);
    U = ua * pow( temp, ub ) * exp( ug * pvalue ) * (C - F);

#ifdef TEST
    printf( "C: %lf, R: %lf, D: %lf, U: %lf, F: %lf\n", C, R, D, U, F );
#endif

    Gr = (C - (R + D + U + F)) * weight / dr;

    return (Gr > 0.0 ? Gr : 0.0);
}

#ifdef TEST
/* command line is: growthfn <weight> <temp> <pvalue> <# days>
 *  and output is weight thereafter.
 *//*
int main( int argc, char **argv ) {
    double w0, weight, temp, pvalue;
    int i, days;
 
    w0 = weight = atof( argv[1] );
    temp = atof( argv[2] );
    pvalue = atof( argv[3] );
    days = atol( argv[4] );
 
    for (i = 0; i < days; i++)
        weight += growth_fn( weight, temp, pvalue );
 
    printf( "weight: %lf, temp: %lf, pvalue: %lf, days: %d\nweight: %lf\n",
            w0, temp, pvalue, days, weight );
}
 */
#endif

