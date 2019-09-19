/** \file

 * \brief includes 
   icludes a structure for the physical TDG 
   parameters as well as the global constant for supersaturation
   equilibrium.
*/
 
#define NE 0.0  			/* Nitrogen Supersaturation Equalibrium */

typedef struct{
        double length;			/* Reach length	*/
        double depth;			/* Average reach depth */
        double l;			/* dissexp = (miles)-1 = K/velocity   */
        double lam;			/* dissexp + (THETA)/(daily velocity) */
}ReachGasSpecs;
 
