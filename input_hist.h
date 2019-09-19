/* \file
 * \brief Input hist data struct */

#define HIST_STR_LEN 256

/** \brief Input hist data struct */
typedef struct { 
    /* stuff up here gets set by the user */
    char	label[HIST_STR_LEN];	/* Change to a pointer sometime (!) */
    char	*title;
    int		xmin, xmax;
    float	ymin, ymax;
    int		num_choices; 	/* if > 1, must set array_names */
    float	*data;
    char	*menu_name;
    char 	**menu_choices;
    int		color;
    int		conv_type;
    float	conv_factor;
    int		modify_inhibit;
    int		hist_type;
    int		flags;		/* currently 1 == display month,day */
    char	*help_text;
    float	(*getdata)();	/* The function to get a data value */
    void	(*setdata)();	/* The function to set a data value */
    void	(*refresh)();	/* A refresh function after sets 
 				   NOTE: this refreshes the data after
				   the user sets it in the histogram */
    void 	(*called_on_repaint)(); /* Usually used to re-gather the
					   data after the frame is repainted.
					   Simply a fcn that is called just
					   before a repaint. */
    void 	(*called_on_destroy)(); /* Usually used to destroy a dynamic
					  data segment when the frame is
					  destroyed */
    void	*user_data;	/* hook for user data */
    
    /* stuff down here gets set by this package */
#ifdef DISPLAY
    Frame	frame;
    Canvas	canvas;
    Frame       sched_frame;	/* Existing schedule frame */
    Panel_item  cycle_item;	/* The cycle item, if it exists */
#endif
    int		resize;		/* whether this hist is resizable */
    int		n;		/* number of data points (x axis) */
    int		xinc;		/* number of pixels per point (x) */
    int		y0;		/* top end of Y axis (settable by
				   user, but has default) */
    int		y1;		/* bottom end of Y axis (settable by
				   user, but has default) */
    int		x0;		/* left end of X axis (settable by
				   user, but has default) */
    int		x1;		/* right end of X axis */
    int		xrange;		/* number of pixels spanned (x1-x0) */
    int		yrange;		/* number of pixels spanned (y1-y0) */
    int		yzero;		/* y coordinate of zero */
#ifdef DISPLAY
    Panel_item  dump_item;	/* file to output data in */
#endif
    
} Input_hist;

#define	IH_WHITE	0
#define	IH_RED		1
#define	IH_BLUE		2
#define	IH_GREEN	3
#define	IH_GRAY		4
#define	IH_ORANGE	5
#define	IH_MAROON	6
#define	IH_BLACK	7

#define IH_FLOW		0	/* Flow for segment (kcfs) */
#define IH_LOSS		1	/* Loss for ... */
#define IH_ELEV		2	/* Elevation change for ... */
#define IH_TEMP		3	/* Water Temp vs Julian Day */
#define IH_NSAT		4	/* N Supersat > 100% vs Julian Day */
#define IH_NUMBER	5	/* Number vs Day for release */
#define IH_SWIM_V	6	/* Swimming velocity downstream for species */
#define IH_MIGR_V	7	/* Migration velocity for species */
#define IH_DRIFT_TIME	8	/* Drift Time for species */
#define IH_BASIN_FLOW   9	/* Storage basin outflow */
#define IH_BASIN_VOLUME 10      /* Storage basin volume */
#define IH_PASSAGE	11	/* Passage vs Day */

#define IH_F_DEFAULT	0	/* no flags set */
#define IH_F_MONTH_DAY	1	/* display month and day */

void make_input_hist_frame();
void destroy_input_hist_frame();

/* conversion types */
#define C_NONE		0		/* no conversion */
#define C_COPY		1		/* copy data values into 'conv_factor'-1 adjacent locations*/
#define C_DIV		2		/* divide data value into 'conv_factor' units */
#define C_SPREAD	3		/* spread data values apart by 'conv_factor'-1 units */

#define	IH_TOP_MARGIN		10
#define	IH_BOTTOM_MARGIN	20
#define	IH_LEFT_MARGIN		60
#define	IH_RIGHT_MARGIN		20

#define CMS_IHSIZE        8
#define CMS_IHNAME        "Input Histogram"
