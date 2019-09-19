/** \file
 * \brief Output flags for output settings in summary.out output.
 *
 * \section output_settings Output Settings in COMPASS
 * The output_flags.h file defines the output flags that are use to select certain pieces of
 * data for writing out summary.out files. The code for writing out data is
 * storted in summary.c but the flags are actually checked in a few other
 * places (e.g. when performing calculations to decide which data should
 * be recorded and which should be discarded).

 * These constants define the values of the output_flags bit field in river_segs.
 * Note that for reaches, the passage and transport flags are meaningful,
 * while at dams, the passage and routing flags are meaningful (transport
 * is part of routing).  These flags are used both in scenario and monte
 * carlo modes of operation.
 */

/** Passage output */
#define PASSAGE_FLAG		0x01
/** Transport information */
#define TRANSPORT_FLAG		0x02
/** Routing information for dams */
#define ROUTING_FLAG		0x04
/** Dam/reach/headwater flow information */
#define FLOW_FLAG			0x08
/** Dam/reach/headwater temperature information */
#define WATERTEMP_FLAG		0x10
/** Dam/reach nitrogen supersaturation information */
#define NSAT_FLAG			0x20
/** Reach velocity information */
#define VELOCITY_FLAG		0x40
/** Reach loss information */
#define LOSS_FLAG			0x80
/** Dam spill information */
#define SPILL_FLAG			0x100
/** Reach elevation information */
#define ELEVATION_FLAG		0x200
/** Fish length information */
#define LENGTH_FLAG         0x400 
/** Reach turbidity information */
#define TURBIDITY_FLAG      0x800 
/** Reach spill experience (from upstream dam) information */
#define SPILL_EXPERIENCED_FLAG      0x1000
/** Dam day spill information */
#define DAY_SPILL_FLAG      0x2000
/** Dam night spill information */
#define NIGHT_SPILL_FLAG      0x4000

/* these are lookups into the field, for convenience - many un-used*/

/** Whether passage is tracked at a segment */
#define TRACK_PASSAGE(seg) ((seg)->output_flags & PASSAGE_FLAG)
/** Whether transport is tracked at a segment */
#define TRACK_TRANSPORT(seg) ((seg)->output_flags & TRANSPORT_FLAG)
/** Whether dam routing is tracked at a segment */
#define TRACK_ROUTING(seg) ((seg)->output_flags & ROUTING_FLAG)
/** Whether flow information is tracked at a segment */
#define TRACK_FLOW(seg) ((seg)->output_flags & FLOW_FLAG)
/** Whether water temperature information is tracked at a segment */
#define TRACK_WATERTEMP(seg) ((seg)->output_flags & WATERTEMP_FLAG)
/** Whether nitrogen supersaturation information is tracked at a segment */
#define TRACK_NSAT(seg) ((seg)->output_flags & NSAT_FLAG)
/** Whether water velocity information is tracked at a segment */
#define TRACK_VELOCITY(seg) ((seg)->output_flags & VELOCITY_FLAG)
/** Whether reach loss information is tracked at a segment */
#define TRACK_LOSS(seg) ((seg)->output_flags & LOSS_FLAG)
/** Whether dam spill information is tracked at a segment */
#define TRACK_SPILL(seg) ((seg)->output_flags & SPILL_FLAG)
/** Whether elevation information is tracked at a segment */
#define TRACK_ELEVATION(seg) ((seg)->output_flags & ELEVATION_FLAG)

