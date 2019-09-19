/** \file 
 * \brief External declarations for multiple powerhouse module.
 *
 * Multiple powerhouses are supported for dams like Bonneville dam that have
 * multiple powerhouses that can be started and stopped independently. */


extern void clear_powerhouse_fractions( /* struct river_segment *dam */ );
extern struct river_segment *find_priority_phouse( /* struct river_segment *dam,
						      int i */ );
extern PERIOD_LIST *make_default_phouse_schedule();
