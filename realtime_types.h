/** \file
 * \brief Realtime information structure.  */

#ifdef REALTIME
#ifndef REALTIME_TYPES_H
#define REALTIME_TYPES_H

/** Structure for holding realtime information. */
typedef struct rt_info {
    char *name;
    int data_date;
} RtInfo;


#endif /* REALTIME_TYPES_H */
#endif /* REALTIME */
