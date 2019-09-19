/** \file
 * \brief Tokens for parsing the list of Monte Carlo alternatives. */
#define TKS_ALT     "alternative"
#define TKS_YEARS   "years"
#define TKS_GAMES   "games"

#define TKS_YEAR    "year"
#define TKS_REL        "release"
#define TKS_AGE           "age"
#define TKS_SMOLT_ON      "smolt"
#define TKS_FLOW_SEG      "flow_indicator" 
#define TKS_STATS         "stats"
#define TKS_STATS_INRIVER         "inriver"
#define TKS_STATS_TRANSPORT         "transport"
#define TKS_GAME	  "game"
#define TKS_RIV_SEG       "river_segment"
                       /* "end river_segment" */
#define TKS_END     "end" /* "release" */

#define TK_ALT      1
#define TK_YEARS    2
#define TK_GAMES    3

#define TK_YEAR	    4
#define TK_REL      5
#define TK_AGE      6
#define TK_STATS    7
#define TK_GAME     8
#define TK_RIV_SEG  9
                   
#define TK_END      10
#define TK_FLOW_SEG 11
#define TK_SMOLT_ON 12
extern command_struct at_tokens[];
