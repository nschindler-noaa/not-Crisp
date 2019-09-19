/** \file
 * \brief Internal data structure for sending passage information between passage.c and dam_model.c. */

/** Structure describing dam passage survival probabilities */
struct dam_surv {
    double	spill_surv;
    double	rsw_surv;
    double	turbine_surv;
    double	bypass_surv;
    double	sluiceway_surv;
    double	pred_surv;     /* forebay predation survival, full slice */
    double  pred_quarter_surv; /* fb. predation survival, quarter slice. */
    double  fb_pred_surv;  /* predation survival, traversing forebay to dam face. */
    double	tr_pred_surv;
    double	bypass_prob;
    double	collection_prob;
    double	pass_prob;
#ifdef ROUTE_TRACKING
    double	sluiceway_prob;
    double	turbine_prob;
    double	spill_prob;
    double	rsw_prob;
#endif    

	/* Route-specific dam delay (these account for multiple powerhouses, but spillways are shared) */
	double	bypass_extra;
	double	bypass_day;
	double	sluiceway_extra;
	double	sluiceway_day;
	double	turbine_extra;
	double	turbine_day;
};

extern void	pass_surv(struct river_segment* seg, int spc, StockT *stock,
			  int day, int time, double collection_eff,
			  double smolt_age, struct dam_surv *dmprb
#ifdef ROUTE_TRACKING
		  , int tracking_flag
#endif		  
		  , int delay);
