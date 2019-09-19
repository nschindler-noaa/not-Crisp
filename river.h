/** \file
 * \brief Functions for manipulating the river */

extern struct river_segment * new_river_segment(/*int seg_type*/);

extern int destroy_river_segment (/* struct river_segment *seg */);

extern struct river_segment *find_seg_by_name(/* river_segment *river,
						 int seg_type,
					         char *seg_name */);

extern struct river_segment *find_seg_by_abbrev(/* river_segment *river,
					         char *seg_abbr */);

extern struct point *next_upstream_point( /* river_segment*, point* */ );
extern struct point *next_downstream_point( /* river_segment*, point* */ );

