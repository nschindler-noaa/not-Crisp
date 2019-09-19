/** \file */
#include "parseutil.h"
#include "tokens_alt.h"

command_struct single_alt_tokens [] =
{
    {TK_INCLUDE,		TKS_INCLUDE},
    {TK_FLOW_ARCHIVE_NAME,	TKS_FLOW_ARCHIVE_NAME},
    {TK_USE_FLOW_FROM,		TKS_USE_FLOW_FROM},
    {TK_NGAMES,			TKS_NGAMES},
    {TK_USE_SPILL_FROM,		TKS_USE_SPILL_FROM},
    {TK_USE_ELEV_FROM,		TKS_USE_ELEV_FROM},
    {TK_INPUT_FILES,		TKS_INPUT_FILES},
    {TK_MULTI,			TKS_MULTI},
    {TK_END,			TKS_END},
    {0,"\0"}
};

command_struct alt_tokens [] =
{
    {TK_VERSION,		TKS_VERSION},
    {TK_INCLUDE,		TKS_INCLUDE},
    {TK_ALTERNATIVE,		TKS_ALTERNATIVE},
    {0,"\0"}
};
    
