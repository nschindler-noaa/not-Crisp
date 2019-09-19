/** \file
 * \brief Initialization and checking functions. */

#ifndef __INIT_H__
#define __INIT_H__

#define FLOW_PCT 0.40

#ifdef __cplusplus
extern "C" {
#endif

extern void set_num_reach_classes( int n );
extern int init_compass();
extern void remove_all_data();
extern void init_spill_cap( struct dam *damptr );
extern int simulation_init(int mode);

#ifdef __cplusplus
}
#endif

extern int data_check();

#endif /* __INIT_H__ */
