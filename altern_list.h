/** \file
 * \brief Alternative list manipulation for Monte Carlo mode.
 *
 * This file defines the interface for manipulating the list of alternatives
 * for use in Monte Carlo mode. This data comes mostly from the ".compass-alts"
 * file and the alternatives themselves.
 *
 * This implementation uses the tree module defined in tree.h.
 */

#ifndef __ALTERN_LIST_H__
#define __ALTERN_LIST_H__

#include <time.h>


/** Alternative node */
struct altern_tree_node {
    char *name; /** The name of the alternative */
    int flag; /** Whether the alternative should be run */
};

#ifdef __cplusplus
extern "C" {
#endif

extern  int working_directory_modified( time_t *dirtime );
extern  void set_altern_updated_flag( time_t *last_checked );
extern  int altern_files_modified( time_t *last_checked );
extern  char *monte_output_name( char *dir_name );
extern  char *monte_altern_name( const char *dir_name );
extern  int altern_update_directory();
extern  void prune_altern_list();
extern  void reset_altern_list();
extern  char *get_current_list_altern();
extern  int search_altern_list( const char *altern );
extern  int altern_exists( char *altern );
extern  int get_altern_list_size();
extern  int get_current_list_item_number();
extern  void inc_altern_list();
extern  void set_current_altern_list_flag( int flag );
extern  void clear_all_altern_flags();
extern  int get_current_altern_list_flag();
extern  int altern_list_add( const char *name );
extern  int altern_list_remove( const char *name );
extern  void empty_altern_list();

#ifdef __cplusplus
}
#endif
   
#endif /* __ALTERN_LIST_H__ */

