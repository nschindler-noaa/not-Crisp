/** \file 
 * \brief Realtime mode functions. */

#ifdef REALTIME
#ifndef REALTIME_H
#define REALTIME_H

#include "realtime_defs.h"
#include "realtime_types.h"

#ifdef __cplusplus
extern "C" {
#endif
    
extern void rt_set_input_pathname( char *path );
extern RtInfo *construct_rtinfo( char *name );
extern void destroy_rtinfo( RtInfo **RtInfopp );
extern RtInfo *clone_rtinfo( RtInfo *from );
extern int rt_reset_name( RtInfo *rtinfo, char *name );
extern int realtime_output_datafile( struct run *current_run );

#ifdef __cplusplus
}
#endif
    
#endif /* REALTIME_H */
#endif /* REALTIME */
