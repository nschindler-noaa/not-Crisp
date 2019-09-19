/** \file
 * \brief Flow calculations header.
 */
#ifndef FLOW_H
#define FLOW_H

#ifdef __cplusplus
extern "C" {
#endif

struct river_segment;
    
/* reach enclosure function codes */

#define REFN_NULL	((int) 0x00)		/* nothing */
#define REFN_COUNT	((int) 0x01)		/* count reaches */
#define REFN_TOTLOSS	((int) 0x02)		/* add total loss in reaches */
#define REFN_TOTREG	((int) 0x04)		/* total regulated flow */
#define REFN_ADJLOSS	((int) 0x08)		/* adjust loss in the reaches */
#define REFN_TOTEXCESS	((int) 0x10)		/* total flow min above min */
#define REFN_COUNT_DAMS	((int) 0x20)		/* count dams */
#define REFN_ADJ_UNREG_LOSS ((int) 0x40)	/* adjust loss from unreg calc*/

/* use the temp field in the dam structure for flow calculation purposes */

#define inactive temp1

/* some macros for volume calculations */
/* keeps drawdown from bottoming-out a reach */
#define DEPTH_PADDING   0.95

/* reach volumes with elevation change and sloped bottom;
 * slit into initial and excess for easier computation
 */
#define V_INITIAL(E, Hu, Hl, W, tan)					      \
	((1 / ((Hl) - (Hu)) ) * ((Hl) - (E))     			       	      \
	* ((Hl) - (E)) * ( (W)/2.0 - (tan) * ( (Hl)/6.0 + (Hu)/2.0 + (E)/3.0 )))

#define V_EXCESS(E, Hu, Hl, W, tan)					      \
	( (E) >= (Hu) ? 0.0 : (1 / ((Hl) - (Hu)) ) * ((Hu) - (E))     	       	      \
	* ((Hu) - (E)) * ( (W)/2.0 - (tan) * ( (Hl)/2.0 + (Hu)/6.0 + (E)/3.0 )))

/* flat bottom reach volume */
#define V_FLAT(E, Hu, Hl, W, tan)					      \
	( ( (W) - (tan)*((Hl) + (E)) ) * ((Hl) - (E)) )

/* total volume for reaches with elevation change */
#define V_TOTAL(E, Hu, Hl, W, tan)					      \
	( (Hu) == (Hl) ? V_FLAT((E), (Hu), (Hl), (W), (tan))		      	      \
	: V_INITIAL((E), (Hu), (Hl), (W), (tan))		      	      \
	- V_EXCESS((E), (Hu), (Hl), (W), (tan)) )

/* full pool volume */
#define V_FULL(Hu, Hl, W, tan)					      	      \
	( (Hu) == (Hl) ? V_FLAT(0, (Hu), (Hl), (W), (tan))		      	      \
	: (W) * ((Hl) + (Hu))/2.0 - (tan)/3.0    		      \
	* ( ((Hl) + (Hu)) * ((Hl) + (Hu))/2.0  + (Hl) * (Hu) ))

/* final reach volume; code should use this macro */
#define REACH_VOLUME(E, Hu, Hl, W, tan)					      \
	( (E) == 0.0 ? V_FULL((Hu), (Hl), (W), (tan))				      \
	: V_TOTAL((E), (Hu), (Hl), (W), (tan)) )

/* some definitions regarding the division of the river into multiple flows */

typedef enum FlowLocation
{ FLOW_RIGHT = 0, FLOW_LEFT, NUM_FLOW_PARTITIONS } FlowLocation;

#define FLOW_OTHER_SIDE(side) ((side) == FLOW_LEFT ? FLOW_RIGHT : FLOW_LEFT)
    
extern int fill_headwaters_from_dams(struct river_segment *start_seg);
extern void monte_prop_upstream(struct river_segment **dam_list, struct river_segment **reach_list);
extern void propogate_elev_change( struct river_segment *seg, double dam_top );
extern void modulate_flows(struct river_segment **dam_list, struct river_segment **reach_list);

extern int compute_all_flow();
extern void mark_only_dams_active();



#ifdef __cplusplus
    }
#endif
#endif /* FLOW_H */
