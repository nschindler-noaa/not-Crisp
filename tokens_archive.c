/** \file */
#include "parseutil.h"
#include "tokens_archive.h"

command_struct arc_head_tokens [] =
{   { TK_DAM, TKS_DAM	},
    { TK_REACH, TKS_REACH },
    { TK_GAME, TKS_GAME },
    { TK_YEAR, TKS_YEAR },
    { TK_PERIOD, TKS_PERIOD },
    { TK_BEGIN_DATA, TKS_BEGIN_DATA },
	{ 0, "\0" }
};

