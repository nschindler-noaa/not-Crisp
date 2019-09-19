/* \file
 * \brief  Functions provided by the release module.
 *
 * Other modules should use these rather than malloc'ing releases on their own.
 */

#ifdef __cplusplus
extern "C" {
#endif
    
extern struct release *construct_release(); /* allocate and initialize */ 
extern void copy_release( struct release *from, struct release *to ); /* copy data */		
extern void activate_release( struct release *r ); /* insert in global list */	
extern void delete_release( struct release *r ); /* free memory and remove */
extern void remove_all_releases();	    /* trash everything */
extern struct release *find_release();

#ifdef __cplusplus
}
#endif
    
