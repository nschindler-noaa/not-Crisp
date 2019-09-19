#include <math.h>

/* compute the great-circle distance between two points -
 * we could instead use a latitude-adjusted cartesian distance
 * since the computation is much faster and the error
 * over the distances we're interested in is small.
 */


double gc_dist(y0,x0,y1,x1)
	double x0,y0,x1,y1;
{
#ifdef GREAT_CIRCLE
    extern double sin(), cos(), acos();

    x0 *= (3.14159265 / 180.0);
    y0 *= (3.14159265 / 180.0);
    x1 *= (3.14159265 / 180.0);
    y1 *= (3.14159265 / 180.0);

    return acos(sin(y0) * sin(y1) + cos(y0) * cos(y1) * cos(x1 - x0))
	* (180.0 / 3.14159265) * 69.0468;
#else
    extern double cos(), sqrt();

    double dx = (x0 - x1) * cos(y0 * (3.14159265 / 180.0));
    double dy = (y0 - y1);

    return sqrt(dx*dx + dy*dy) * 69.0468;	/* statute miles */
#endif
}

double gc_dist_comp(y0,x0,y1,x1)
	double x0,y0,x1,y1;
{
#ifdef GREAT_CIRCLE
    extern double sin(), cos(), acos();

    x0 *= (3.14159265 / 180.0);
    y0 *= (3.14159265 / 180.0);
    x1 *= (3.14159265 / 180.0);
    y1 *= (3.14159265 / 180.0);

    return acos(sin(y0) * sin(y1) + cos(y0) * cos(y1) * cos(x1 - x0))
	* (180.0 / 3.14159265) * 69.0468;
#else
    extern double cos(), sqrt();

    double dx = (x0 - x1) * cos(y0 * 0.017453293);
    double dy = (y0 - y1);

    return (dx*dx + dy*dy);	/* statute miles */
#endif
}

