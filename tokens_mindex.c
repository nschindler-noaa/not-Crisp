/* tokens_mindex.c
 *
 *	The global array containing all tokens used in parsing monte
 * index files.
 */

#include "parseutil.h"
#include "tokens_mindex.h"

command_struct index_tokens [] = {
    { TK_MI_YEARS,	TKS_MI_YEARS},
    { TK_MI_NRELEASES,	TKS_MI_NRELEASES},
    { TK_MI_NSEGMENTS,	TKS_MI_NSEGMENTS},
    { TK_MI_ALTERN,	TKS_MI_ALTERN},
    { TK_MI_RELEASE,	TKS_MI_RELEASE},
    { TK_MI_SEGMENT,	TKS_MI_SEGMENT},
    { TK_MI_END,	TKS_MI_END},
    { 0, "\0" },
};
