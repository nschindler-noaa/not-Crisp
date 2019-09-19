/** \file
 * \brief Period structure for creating lists of periods (e.g. for spill schedules). */

#ifndef PERIOD_H
#define PERIOD_H

#ifdef __cplusplus
extern "C" {
#endif
    
typedef struct period_list_struct
{
    struct period_list_struct *prev;
    struct period_list_struct *next;
    struct period_list_struct *sub_periods;
    int start;
    int end;
    float value;
} PERIOD_LIST;

extern PERIOD_LIST *new_period_struct ( );
extern PERIOD_LIST *make_deep_copy(  PERIOD_LIST *rhs );
extern void destroy_period_struct ( PERIOD_LIST *period );
extern void collapse_period_list( PERIOD_LIST *per );
extern int dam_slice_in_schedule ( PERIOD_LIST *sched, int slice );
extern int check_daily_period_list(PERIOD_LIST *day);

#ifdef __cplusplus
}
#endif
    
#endif
