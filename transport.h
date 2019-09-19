/** \file
 * \brief Functions for transport operations. */

extern float river_distance();
extern int copy_trns(struct transport *old, struct transport *new);
extern int add_transport(struct transport *trns);
extern int remove_transport();
extern int remove_all_transport();
extern void set_tr_modified();
extern void clear_tr_modified();
extern int transport_fish(struct release *release, 
						  struct rls_seg *rls_seg, 
						  struct transport *trns, int game);

