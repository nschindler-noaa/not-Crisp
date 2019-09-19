#include "parseutil.h"
#include "tokens_at.h"

command_struct at_tokens [] = {
    { TK_ALT, 		TKS_ALT},
    { TK_YEARS,		TKS_YEARS},
    { TK_GAMES,		TKS_GAMES},
    { TK_YEAR,		TKS_YEAR},
    { TK_REL,		TKS_REL},
    { TK_AGE, 		TKS_AGE},
    { TK_SMOLT_ON,	TKS_SMOLT_ON},
    { TK_STATS, 	TKS_STATS},
    { TK_RIV_SEG,	TKS_RIV_SEG},
    { TK_FLOW_SEG,	TKS_FLOW_SEG},
    { TK_GAME, 		TKS_GAME},
    { TK_END, 		TKS_END},
    { 0, "\0" }
};

   
