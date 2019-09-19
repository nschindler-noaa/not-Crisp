#include <string.h>
#include <stdlib.h>
#ifdef WIN32
#include <stdio.h>
#include <malloc.h>
#include <io.h>
#else
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <malloc.h>
#endif

#include "crisp.h"
#include "river.h"
#include "token.h"
#include "c1tokens.h"
#include "init.h"
#include "parseutil.h"
#include "batch.h"
#include "gc_dist.h"
#include "file.h"
#include "init_river.h"

#ifndef R_OK
/* TODO PC should not need this. */
#define R_OK   4
#endif

void		init_release_menus();

extern double	gc_dist();
extern int add_basin(struct river_segment *dam, float min, float max);
extern int allocate_species();

static char	keyword[MAX_TOKEN_LEN+1];

static char *make_string(str1, str2)
	char *str1;
	char *str2;
{
	char *dst;
	int i, str1len, str2len;

	/* strip trailing blanks */
	for (i = strlen(str1); i > 0 && str1[i-1] == ' '; i--)
	    ;
	str1len = i;
	for (i = strlen(str2); i > 0 && str2[i-1] == ' '; i--)
	    ;
	str2len = i;

	dst = (char *) malloc(str1len + str2len + 1);

	strncpy(dst, str1, str1len);
	dst[str1len] = 0;
	strncat(dst, str2, str2len);
	dst[str1len+str2len] = 0;

	return dst;
}


/* reads name, including spaces, from file, */
/* from current mark to end of line, up to MAX_TOKEN_LEN max */
static int read_name(input, name)
    FILE	*input;
    char	*name;
{
    char	temp[MAX_TOKEN_LEN+1];
    int		line;

    line = buf_line(input);
    if (pop_token(input, name, "") == EOF)
	return EOF;
	if (pop_token(input, temp, "") == EOF)
	    return EOF;
    while (buf_line(input) == line) {
	strncat(name, " ", MAX_TOKEN_LEN - strlen(name));
	strncat(name, temp, MAX_TOKEN_LEN - strlen(name));
	if (pop_token(input, temp, "") == EOF)
	    return EOF;
    }
    push_last_token(input);
    return 0;
}


/* parse a latitude/longitude.  return:
 *	0	: error
 *	other	: pointer to the point
 */

static struct point * 
parse_latlon(input)
	FILE	       *input;
{
	struct point   *point;
	float		lat, lon;
	char		cp[32];

	/* latitude */
	read_float(input, &lat, "");
	if (pop_token(input, cp, "") == EOF)
	    return (struct point *)0;
	if (is_int(cp))
	{
	    lat += (float)atoi(cp) / 60.0;
	    if (pop_token(input, cp, "") == EOF)
		return (struct point *)0;
	    if (is_int(cp))
	    {
		lat += (float)atoi(cp) / 3600.0;
		if (pop_token(input, cp, "") == EOF)
		    return (struct point *)0;
	    }
	}

	if (cp[0] == 'S' || cp[0] == 's')
	    lat = -lat;
	else if (cp[0] != 'N' && cp[0] != 'n')
	    push_last_token(input);

	/* longitude */
	read_float(input, &lon, "");
	if (pop_token(input, cp, "") == EOF)
	    return (struct point *)0;
	if (is_int(cp))
	{
	    lon += (float)atoi(cp) / 60.0;
	    if (pop_token(input, cp, "") == EOF)
		return (struct point *)0;
	    if (is_int(cp))
	    {
		lon += (float)atoi(cp) / 3600.0;
		if (pop_token(input, cp, "") == EOF)
		    return (struct point *)0;
	    }
	}

	if (cp[0] == 'W' || cp[0] == 'w')
	    lon = -lon;
	else if (!strcmp(cp, "end") || (cp[0] != 'E' && cp[0] != 'e'))
	    push_last_token(input);

	if ( ! (point = (struct point *)malloc(sizeof(struct point))))
		return (struct point *)0;

	point->next = (struct point *)0;
	point->prev = (struct point *)0;

	point->lat = lat;
	point->lon = lon;

	if (lat_min > lat)
		lat_min = lat;
	if (lat_max < lat)
		lat_max = lat;
	if (lon_min > lon)
		lon_min = lon;
	if (lon_max < lon)
		lon_max = lon;

	return point;
}


/* parse a release site.  return codes:
 *	 0	: OK
 *	-1	: error
 */

static int parse_release_site(input)
	FILE	       *input;
{
	struct release_site *site;
	static struct release_site *last;
	char name[MAX_TOKEN_LEN+1];

	if ( ! (site = (struct release_site *)malloc(sizeof(struct release_site))))
		return -1;

	bzero((char *)site, sizeof(struct release_site));
	if (last)
		last->next = site;
	else
		release_site = site;
	last = site;



	if (read_name(input, name) == EOF)
		return -1;
	site->name = make_string(name, ""); 

	while (pop_token(input, keyword, "") != EOF ) {
	    if (strcmp(keyword, "latlon") == 0) {
		struct point   *point;

		if ( ! (point = parse_latlon(input)))
		    return -1;

		site->latlon = point;

	    } else if (strcmp(keyword, "river_mile") == 0) {
		float	river_mile;

		if (!read_float(input, &river_mile, ""))
		    return -1;

		site->seg_mile = river_mile;

	    } else if (strcmp(keyword, "end") == 0) {
		reset_input(input);
		if ( ! site->latlon) {
		    fprintf(stderr, "missing lat/lon info\n");
		    return -1;
		}

		return 0; /* site */

	    } else {
		fprintf(stderr, "unrecognized keyword \"%s\"\n", keyword);
		return -1;
	    }
	}
	return -1;
    }

/* parse a tributary.  return codes:
 *	 0	: OK
 *	-1	: error
 */

static int parse_tributary(input)
	FILE	       *input;
{
	struct tributary *trib;
	static struct tributary *last;
	char name[MAX_TOKEN_LEN+1];

	if ( ! (trib = (struct tributary *)malloc(sizeof(struct tributary))))
		return -1;

	bzero((char *)trib, sizeof(struct tributary));
	if (last)
		last->next = trib;
	else
		tributary = trib;
	last = trib;



	if (read_name(input, name) == EOF)
		return -1;
	trib->name = make_string(name, ""); 

	while (pop_token(input, keyword, "") != EOF ) {
	    if (strcmp(keyword, "latlon") == 0) {
		struct point   *point;

		if ( ! (point = parse_latlon(input)))
		    return -1;

		trib->latlon = point;

	    } else if (strcmp(keyword, "river_mile") == 0) {
		float	river_mile;

		if (!read_float(input, &river_mile, ""))
		    return -1;

		trib->seg_mile = river_mile;

	    } else if (strcmp(keyword, "end") == 0) {
		reset_input(input);
		if ( ! trib->latlon) {
		    fprintf(stderr, "missing lat/lon info\n");
		    return -1;
		}

		return 0; /* site */

	    } else {
		fprintf(stderr, "unrecognized keyword \"%s\"\n", keyword);
		return -1;
	    }
	}
	return -1;
    }

static struct river_segment *
parse_reach(input)
	FILE	       *input;
{
	struct river_segment *new;
	char		name[MAX_TOKEN_LEN+1];
	int		i;

	if ( ! (new = new_river_segment(REACH_SEG)))
		return (struct river_segment *)0;

	if (read_name(input, name) == EOF)
		return (struct river_segment *)0;
	new->seg_name = make_string(name, ""); 

    /* This initialization is made to flag that the value was not read in.
     *  This may indicate an error in the .desc file.
     */
    new->current->reach.lower_elev = -10000.0;

	while (pop_token(input, keyword, "") != EOF) {
	    if (strcmp(keyword, "volume") == 0) {
		float		volume;
		if (!read_float(input, &volume, ""))
		    return (struct river_segment *)0;
		new->current->reach.volume = volume;
		
	    } else if (strcmp(keyword, "surface_area") == 0) {
		float		surface_area;
		if (!read_float(input, &surface_area,""))
		    return (struct river_segment *)0;
		new->current->reach.surface_area = surface_area;

	    } else if (strcmp(keyword, "width") == 0) {
		float		width;
		if (!read_float(input, &width,""))
		    return (struct river_segment *)0;
		new->current->reach.width = width;
		
	    } else if (strcmp(keyword, "depth") == 0) {
		float		depth;
		if (!read_float(input, &depth,""))
		    return (struct river_segment *)0;
		new->current->reach.depth = depth;
		
	    } else if (strcmp(keyword, "upper_depth") == 0) {
		float		upper_depth;
		if (!read_float(input, &upper_depth,""))
		    return (struct river_segment *)0;
		new->current->reach.upper_depth = upper_depth;

	    } else if (strcmp(keyword, "lower_depth") == 0) {
		float		lower_depth;
		if (!read_float(input, &lower_depth,""))
		    return (struct river_segment *)0;
		new->current->reach.lower_depth = lower_depth;
		
	    } else if (strcmp(keyword, "lower_elev") == 0) {
		float		lower_elev;
		if (!read_float(input, &lower_elev,""))
		    return (struct river_segment *)0;
		new->current->reach.lower_elev = lower_elev;
		
	    } else if (strcmp(keyword, "slope") == 0) {
		float		slope;
		if (!read_float(input, &slope,""))
		    return (struct river_segment *)0;
		/* Convert to radians */
		new->current->reach.slope = slope * M_PI / 180.0;
		
	    } else if (strcmp(keyword, "latlon") == 0) {
		struct point   *point;
		
		if ( ! (point = parse_latlon(input)))
		    return (struct river_segment *)0;
		if ((point->prev = new->top)) {
		    new->current->reach.length +=
			gc_dist(point->prev->lat,
				point->prev->lon,
				point->lat, point->lon);
		    new->top = point;
		    point->prev->next = point;
		} else {
		    new->current->reach.length = 0.0;
		    new->bottom = new->top = point;
		}	
	    } else if (strcmp(keyword, "abbrev") == 0) {
	        if (pop_token(input,  keyword, "") == EOF)
		    return (struct river_segment *)0;
	        new->abbrev = make_string(keyword, "");
		} else if (strcmp(keyword, "default_transport_target") == 0) {
			if (default_transport_target) fprintf(stderr, "Multiple declarations of the default transport target (second in %s)\n", new->seg_name);
			else default_transport_target = new;
	    } else if (strcmp(keyword, "end") == 0) {
		reset_input(input);
		
		if ( ! new->bottom) {
		    fprintf(stderr, "missing lat/lon info, seg %s\n", new->seg_name);
		    return (struct river_segment *) 0;
		}
		
/***
			printf("%s has length %.1f miles\n",
				new->seg_name, new->current->reach.length);
 ***/

		/* initialize the rest of the runs */
		for (i = 1; i < NRUNS; ++i) {
		    new->run[i].reach.volume
			= new->current->reach.volume;
		    new->run[i].reach.surface_area
			= new->current->reach.surface_area;
		    new->run[i].reach.width
			= new->current->reach.width;
		    new->run[i].reach.depth
			= new->current->reach.depth;
		    new->run[i].reach.upper_depth
			= new->current->reach.upper_depth;
		    new->run[i].reach.lower_depth
			= new->current->reach.lower_depth;
		    new->run[i].reach.slope
			= new->current->reach.slope;
		    new->run[i].reach.length
			= new->current->reach.length;
		}
		
		return new;
		
	    } else {
		fprintf(stderr, "unrecognized keyword \"%s\"\n",
			keyword);
		return (struct river_segment *)0;
	    }
	}
	/* premature eof ?? */
	return (struct river_segment *)0;
    }

static struct river_segment *
parse_dam_desc(input)
	FILE	       *input;
{
    char		name[MAX_TOKEN_LEN+1];
    struct river_segment *new;

    if (! (new = new_river_segment(DAM_SEG)))
	return (struct river_segment *) 0;

    if (read_name(input, name) == EOF)
	return (struct river_segment *)0;
    new->seg_name = make_string(name, ""); 

    while (pop_token(input, keyword, "") != EOF) {
	if (strcmp(keyword, "latlon") == 0) {
	    struct point   *point;

	    if ( ! (point = parse_latlon(input)))
		return (struct river_segment *) 0;

	    point->next = (struct point *)0;
	    if ((point->prev = new->top)) {
		new->top = point;
		point->prev->next = point;
	    } else {
		new->bottom = new->top = point;
	    }
	} else if (strcmp(keyword, "powerhouse_capacity") == 0) {
	    float		pcap;

	    if (!read_float(input, &pcap,""))
		return (struct river_segment *)0;
	    new->current->dam.powerhouse_capacity = pcap;
	} else if (strcmp(keyword, "powerhouse_2_capacity") == 0) {
	    float		pcap;
	    struct river_segment *phouse;

	    if (!read_float(input, &pcap,""))
		return (struct river_segment *)0;
	    if (! (phouse = new_river_segment(POWERHOUSE_SEG)) )
		return (struct river_segment *) 0;
	    new->current->dam.additional_pwrhs = phouse;
	    phouse->current->dam.powerhouse_capacity = pcap;
	    phouse->seg_name = make_string(new->seg_name, " 2");
	} else if (strcmp(keyword, "sgr") == 0) {
	    float		sgr;

	    if (!read_float(input, &sgr,""))
		return (struct river_segment *)0;
	    new->current->dam.sgr = sgr;
	} else if (strcmp(keyword, "floor_elevation") == 0) {
	    float		elev;

	    if (!read_float(input, &elev,""))
		return (struct river_segment *)0;
	    new->current->dam.base_elev = elev;
	} else if (strcmp(keyword, "forebay_elevation") == 0) {
	    float		elev;

	    if (!read_float(input, &elev,""))
		return (struct river_segment *)0;
	    new->current->dam.fb_elev = elev;
	} else if (strcmp(keyword, "tailrace_elevation") == 0) {
	    float		elev;

	    if (!read_float(input, &elev,""))
		return (struct river_segment *)0;
	    new->current->dam.tr_elev = elev;
	} else if (strcmp(keyword, "bypass_elevation") == 0) {
	    float		bypass_elev;

	    /* note: this var is read now, converted later */
	    if (!read_float(input, &bypass_elev,""))
		return (struct river_segment *)0;
	    new->current->dam.bypass_height = bypass_elev;
	} else if (strcmp(keyword, "collector") == 0) {
	    new->current->dam.collector = 1;
	} else if (strcmp(keyword, "full_pool_head") == 0) {
	    float		head;

	    if (!read_float(input, &head,""))
		return (struct river_segment *)0;
	    new->current->dam.fp_head = head;
	} else if (strcmp(keyword, "forebay_depth") == 0) {
	    float		depth;

	    if (!read_float(input, &depth,""))
		return (struct river_segment *)0;
	    new->current->dam.fp_fb_depth = depth;
	} else if (strcmp(keyword, "spillway_width") == 0) {
	    float		s_width;

	    if (!read_float(input, &s_width,""))
		return (struct river_segment *)0;
	    new->current->dam.spillway_width = s_width;
	} else if (strcmp(keyword, "spill_side") == 0) {
	    if (pop_token(input,  keyword, "") == EOF)
		return (struct river_segment *)0;
	    if (!strcasecmp(keyword, "left"))
		new->current->dam.spill_side = FLOW_LEFT;
	    else
		new->current->dam.spill_side = FLOW_RIGHT;
	} else if (strcmp(keyword, "pergate") == 0) {
	    float		p_gate;

	    if (!read_float(input,&p_gate,""))
		return (struct river_segment *)0;
	    new->current->dam.pergate = p_gate;
	} else if (strcmp(keyword, "ngates") == 0) {
	    float		n_gates;

	    if (!read_float(input, &n_gates,""))
		return (struct river_segment *)0;
	    new->current->dam.ngates = n_gates;
	} else if (strcmp(keyword, "gate_width") == 0) {
	    float		g_width;

	    if (!read_float(input, &g_width,""))
		return (struct river_segment *)0;
	    new->current->dam.gate_width = g_width;
	} else if (strcmp(keyword, "storage_basin") == 0) {
	    float min_vol, max_vol;

	    if (!read_float(input, &min_vol,""))
		return (struct river_segment *) 0;
	    if (!read_float(input, &max_vol,""))
		return (struct river_segment *) 0;
	    if (add_basin(new, min_vol, max_vol) < 0)
		return (struct river_segment *) 0;
	} else if (strcmp(keyword, "basin_length") == 0) {
	    float		b_length;

	    if (!read_float(input, &b_length,""))
		return (struct river_segment *)0;
	    new->current->dam.basin_length = b_length;
	} else if (strcmp(keyword, "abbrev") == 0) {
	    if (pop_token(input,  keyword, "") == EOF)
		return (struct river_segment *)0;
	    new->abbrev = make_string(keyword, "");
	} else if (strcmp(keyword, "end") == 0) {
	    reset_input(input);

	    if ( ! new->bottom) {
		fprintf(stderr, "missing lat/lon info, seg %s\n",
			new->seg_name);
		return (struct river_segment *) 0;
	    }

	    /* bypass height was read in as elevation, so convert */
	    new->current->dam.bypass_height -= new->current->dam.base_elev;
	    if (new->current->dam.bypass_height < 0)
		new->current->dam.bypass_height = 0;

	    return new;

	} else {
	    fprintf(stderr, "unrecognized keyword \"%s\"\n",
		    keyword);
	    return (struct river_segment *) 0;
	}
    }
    /* return failure */
    return (struct river_segment *) 0;
}


/* parse the river.
 *	 0	:  OK
 *	-1	:  error
 */

static int
parse_river(input)
	FILE	       *input;
{
	char		name[MAX_TOKEN_LEN+1];
	char		temp_name[MAX_TOKEN_LEN+1];
	char	       *river_name;
	struct river_segment *seg;
	struct river_segment *new;
	struct river_segment *hw;
	float		flow_max = 0.0;
	float		flow_min = 0.0;
	int		i;

	seg = (struct river_segment *)0;
	new = (struct river_segment *)0;



	if (read_name(input, name) == EOF)
		return -1;
	river_name = make_string(name, ""); 

	while (pop_token(input, keyword, "") != EOF) {
	    if (strcmp(keyword, "reach") == 0) {
		if ( ! (new = parse_reach(input)))
		    return -1;
		/* attach to top */
		if (seg) {
		    seg->up = new;
		    new->down = seg;
		}
		seg = new;
		seg->river_name = river_name;
		
	    } else if (strcmp(keyword, "dam") == 0) {
		if ( ! (new = parse_dam_desc(input)))
		    return -1;
		/* attach to top */
		if (seg) {
		    seg->up = new;
		    new->down = seg;
		}
		seg = new;
		seg->river_name = river_name;
		if (seg->current->dam.additional_pwrhs)
		    seg->current->
			dam.additional_pwrhs->river_name
			    = river_name;
		
	    } else if (strcmp(keyword, "flow_max") == 0) {
		if (!read_float(input, &flow_max,""))
		    return -1;
	    } else if (strcmp(keyword, "flow_min") == 0) {
		if (!read_float(input, &flow_min,""))
		    return -1;
	    } else if (strcmp(keyword, "end") == 0) {
		reset_input(input);
		if ( ! seg) {
		    fprintf(stderr, "no segs in river def??\n");
		    return -1;
		}
		
		/* make the headwaters structure */
		if ( ! (new = new_river_segment(HEADWATER_SEG)))
		    return -1;
		if (seg) {
		    seg->up = new;
		    new->down = seg;
		}
		seg = new;
		seg->river_name = river_name;
		sprintf(temp_name, "%s Headwater", river_name);
		if (!(seg->seg_name =
		      (char *) malloc((strlen(temp_name)+1) * sizeof(char))))
		{
		    fprintf(stderr, "no mem for headwater name!\n");
		    return -1;
		}
		strcpy(seg->seg_name, temp_name);
		for (i = 0; i < NRUNS; ++i)
		{
		    seg->run[i].headwater.flow_max = flow_max;

		    /* pick up the default flow min if none read from file */
		    if (flow_min == 0)	
			flow_min = seg->run[i].headwater.flow_min;
		    else
			seg->run[i].headwater.flow_min = flow_min;
		}
		
		/* steal lat/lon from next seg downriver */
		seg->bottom = seg->down->top;
		seg->top = seg->bottom;
		hw = seg;
		
		/* place the river mouth */
		while (new->down)
		    new = new->down;
		if (new != river_mouth) {
		    /* scan for match */
		    /* if not found, print diagnostic and return -1 */
		    seg = river_mouth;
		    while (seg->seg_type != REACH_SEG 
			   || !strcmp( seg->river_name, new->river_name )
			   || seg->top->lat != new->bottom->lat
			   || seg->top->lon != new->bottom->lon) {
			if ( ! (seg = seg->next)) {
			    fprintf(stderr,
				    "can't connect river %s\n", river_name);
			    return -1;
			}
		    }
		    if (seg->fork) {
			fprintf(stderr, "malformed river map: multi-way fork\n");
			return -1;
		    }
		    seg->fork = new;
		    new->down = seg;
		} 
		
		/* propagate flow_max and min down the river */
		for (seg = hw; seg; seg = seg->down) {
		    if (seg->seg_type == DAM_SEG)
			for (i = 0; i < NRUNS; ++i)
			{
			    seg->run[i].dam.flow_max +=flow_max;
			    seg->run[i].dam.flow_min_river += flow_min;
			}
		    else if (seg->seg_type == REACH_SEG)
			for (i = 0; i < NRUNS; ++i)
			{
			    seg->run[i].reach.flow_max += flow_max;
			    seg->run[i].reach.flow_min += flow_min;
			}
		}
		
		return 0;
		
	    } else {
		fprintf(stderr, "unrecognized keyword \"%s\"\n",
			keyword);
		return -1;
	    }
	}
	/* return failure */
	return -1;
    }


/* parse a species name.  return codes:
 *	 0	: OK
 *	-1	: error
 */

static int parse_species_config(input)
    FILE *input;
{
    char name[64];

    if (read_name(input, name) == EOF)
	return -1;
	
    /* construct or lengthen the array of species name ptrs */
    if (!num_specs)
    {
	if (!(spec_names = (char **) malloc(sizeof(char *))))
	{
	    fprintf(stderr, "ERROR: no mem for species names!\n");
	    return -1;
	}
    }
    else
	spec_names = (char **)
	    realloc((char *) spec_names, (num_specs+1) * sizeof(char *));

    spec_names[num_specs] = make_string(name, ""); 
    num_specs++;
    
    return 0;
}

/* parse a stock name.  return codes:
 *	 0	: OK
 *	-1	: error
 */

static int parse_stock_config(input)
    FILE *input;
{
    char name[128];

    if (read_name(input, name) == EOF)
	return -1;
	
    /* add to the global list */
    if (!new_global_stock(name)) {
	fprintf(stderr, "ERROR: no mem for stocks!\n");
	    return -1;
    }
    
    return 0;
}


/* init_river(filename)
 *	top level routine
 *	 0	: OK
 *	-1	: error
 */

int init_river(filename)
	char	       *filename;
{
	FILE	       *input;
	int		errc;
	struct release_site	*site;
	struct tributary	*trib, *ttr, *ttrb;
	struct river_segment	*seg;
	struct point		*point;
	int		i, j;

	errc = 0;
	lat_min = 90.0;
	lat_max = -90.0;
	lon_min = 180.0;
	lon_max = -180.0;

	release_site = (struct release_site *)0;
	tributary = (struct tributary *)0;
	river_mouth = (struct river_segment *)0;
	num_specs = 0;			/* global count of species */
	stocks.num = 0;			/* global count of stocks */
	
	if ( ! (input = buf_open(filename, "r"))) {
	    perror(filename);
	    return -1;
	}

	while (pop_token(input, keyword, "") != EOF) {
		if (strcmp(keyword, "river") == 0) {
			if (parse_river(input)) {
				errc = -1;
				break;
			}
		} else if (strcmp(keyword, "release_site") == 0) {
			if (parse_release_site(input)) {
				errc = -1;
				break;
			}
		} else if (strcmp(keyword, "tributary") == 0) {
			if (parse_tributary(input)) {
				errc = -1;
				break;
			}
		} else if (strcmp(keyword, TKS_SPEC) == 0) {
		    	if (river_mouth) {
			    fprintf(stderr, "Error: species info must precede river description in configuration file!\n");
			    errc = -1;
			    break;
			}
		    	else if (parse_species_config(input)) {
			    errc = -1;
			    break;
			}
		} else if (strcmp(keyword, TKS_STOCK) == 0) {
		    	if (river_mouth) {
			    fprintf(stderr, "Error: stock info must precede river description in configuration file!\n");
			    errc = -1;
			    break;
			}
		    	else if (parse_stock_config(input)) {
			    errc = -1;
			    break;
			}
		} else {
			fprintf(stderr, "unrecognized keyword \"%s\"\n",
				keyword);
			errc = -1;
			break;
		}
	}

	if (errc) {
		int		lineno = buf_line( input );
		fprintf(stderr, "file %s line %d\n", filename, lineno);
	}

	buf_close(input);

	if (errc)
		return -1;

	if ((lat_range = lat_max - lat_min) <= 0.0
	 || (lon_range = lon_max - lon_min) <= 0.0) {
		fprintf(stderr, "river map missing or malformed\n");
		return -1;
	}
	lat_min -= lat_range * 0.02;
	lat_max += lat_range * 0.02;
	lon_min -= lon_range * 0.02;
	lon_max += lon_range * 0.02;
	lat_range = lat_max - lat_min;
	lon_range = lon_max - lon_min;

	/* generic init values */
	for (seg = river_mouth; seg; seg = seg->next) 
	    switch (seg->seg_type) {
		  case REACH_SEG:
		    for (i = 0; i < NRUNS; i++) {
			    bzero((char *) seg->run[i].reach.velocity,
				  sizeof(seg->run[i].reach.velocity));
			    bzero((char *) seg->run[i].reach.flow,
				  sizeof(seg->run[i].reach.flow));
			    bzero((char *) seg->run[i].reach.loss,
				  sizeof(seg->run[i].reach.loss));
			    seg->run[i].reach.loss_max =
				seg->run[i].reach.flow_max;
			    seg->run[i].reach.loss_min =
				-(seg->run[i].reach.flow_max);
		    }
		    break;

		  case DAM_SEG:
		    for (i = 0; i < NRUNS; i++) {
			    /* Gawd, what a hack.  we need to init flow
			       information at the dams very early, so
			       we can propagate it into the headwaters
			     */
			    for (j = 0; j < DAYS_IN_SEASON; j++)
				seg->run[i].dam.flow[j] =
				    seg->run[i].dam.flow_max * FLOW_PCT;
			    seg->run[i].dam.transport = NULL;
		    }
		    break;

		  case HEADWATER_SEG:
		    for (i = 0; i < NRUNS; i++) {
			    /* assume unregulated flow */
			    seg->run[i].headwater.flow_coefficient = -1.0;
			    
			    /* just to get us started, assume 40% max */
			    for (j = 0; j < DAYS_IN_SEASON; j++)
				    seg->run[i].headwater.flow[j] =
					seg->run[i].headwater.flow_max *
					FLOW_PCT;
		    }
		    break;

		  default:
		    break;
	    }
			    

	/* do some release processing */
	errc = 0;
	for (site = release_site; site; site = site->next) {
		for (seg = river_mouth; seg; seg = seg->next) {
		    if (seg->seg_type != POWERHOUSE_SEG) {
			for (point = seg->bottom; point; point = point->next) {
			    if (site->latlon->lat == point->lat
				&& site->latlon->lon == point->lon) {
				/* calculate distance to bottom of seg */
				struct point *tmp_pnt;
				float dist = 0;
				for (tmp_pnt = seg->bottom; tmp_pnt != point;
				     tmp_pnt = tmp_pnt->next)
				    dist += gc_dist(tmp_pnt->lat,
						    tmp_pnt->lon,
						    tmp_pnt->next->lat,
						    tmp_pnt->next->lon);
				site->seg_mile = dist;
				/* connect seg */
				site->seg = seg;
				break;
			    }
			}
			if (point)
			    break;
		    }
		}
		if ( ! seg) {
			fprintf(stderr,
				"unable to link release site %s\n", site->name);
			errc = -1;
		}
	}

	/* do some tributary processing */
	errc = 0;
	for (trib = tributary; trib; trib = trib->next) {
		for (seg = river_mouth; seg; seg = seg->next) {
		    if (seg->seg_type != POWERHOUSE_SEG) {
			for (point = seg->bottom; point; point = point->next) {
			    if (trib->latlon->lat == point->lat
				&& trib->latlon->lon == point->lon) {
				/* calculate distance to bottom of seg */
				struct point *tmp_pnt;
				float dist = 0;
				for (tmp_pnt = seg->bottom; tmp_pnt != point;
				     tmp_pnt = tmp_pnt->next)
				    dist += gc_dist(tmp_pnt->lat,
						    tmp_pnt->lon,
						    tmp_pnt->next->lat,
						    tmp_pnt->next->lon);
				trib->seg_mile = dist;
				/* connect seg */
				trib->seg = seg;

                /* Maintain list of the tributaries that hang off a given
                 *  segment. They are sorted by increasing "seg_mile" */
                if (seg->tributaries == NULL) {
                    seg->tributaries = trib;
                }
                else {
                    for (ttr = seg->tributaries, ttrb = NULL;
                         ttr != NULL && ttr->seg_mile < dist;
                         ttrb = ttr, ttr = ttr->trib_list)
                        continue;
                    trib->trib_list = ttr;
                    if (ttrb != NULL)
                        ttrb->trib_list = trib;
                    else
                        seg->tributaries = trib;
                 }
				break;
			    }
			}
			if (point)
			    break;
		    }
		}
		if ( ! seg) {
			fprintf(stderr,
				"unable to link tributary %s\n", trib->name);
			errc = -1;
		}
	}

	/* do some species processing */
	if (allocate_species())
	    errc = -1;

	if (errc)
		return -1;
	else
		return 0;
}

/**
 * \brief
 * 	find and read the river description file
 * \return 0	: OK, -1: error
 */

int get_river_file(fname)
	char *fname;
{
    char	*strtok();
    char	river_file[MAXPATHLEN];

    if (access(fname, R_OK) == 0) 
	return(init_river(fname));

    if (find_compass_file(fname, river_file) && access(river_file, R_OK) == 0)
	return(init_river(river_file));

	fprintf(stderr, "Can't find accessible river description file %s\n",
		fname);
	return -1;
}

