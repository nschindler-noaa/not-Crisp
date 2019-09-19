/** \file
 * \brief Alternative data structure for use in the report tool
 *
 * See report_altern.c for more information on how this is used.
 */

/** Data structure for the report tool that holds alternative data */
struct alt_data {
    float mean_passage;
    float stddev_passage;
    float var_passage;
    float skew_passage;
    float kurt_passage;
    float mean_travel;
    float stddev_travel;
    float var_travel;
    float skew_travel;
    float kurt_travel;
    float avg_flow;
#ifdef ROUTE_TRACKING
    float sluiceway_avg;
    float bypass_avg;
    float turbine_avg;
    float spill_avg;
    float rsw_avg;
    float transport_avg;
    float premort_transport_total;
    float postmort_transport_total;
    float sluiceway_in_avg;
    float bypass_in_avg;
    float turbine_in_avg;
    float spillway_in_avg;
    float rsw_in_avg;
    float project_in_total;
    float fpe;
#endif
};

#define ART_PASSAGE_MEAN	"mean_passage"
#define ART_PASSAGE_STDDEV	"passage_std_dev"
#define ART_PASSAGE_COEFFVAR	"passage_coeff_variance"
#define ART_PASSAGE_SKEW	"passage_skew"
#define ART_PASSAGE_KURT	"passage_kurtosis"
#define ART_TRAVEL_MEAN		"mean_travel_time"
#define ART_TRAVEL_STDDEV	"travel_time_std_dev"
#define ART_TRAVEL_COEFFVAR	"travel_time_coeff_variance"
#define ART_TRAVEL_SKEW		"travel_time_skew"
#define ART_TRAVEL_KURT		"travel_time_kurt"
#define ART_AVERAGE_FLOW	"average_flow"
#define ART_SLUICEWAY_AVG		"sluiceway_passage"
#define ART_BYPASS_AVG		"bypass_passage"
#define ART_TURBINE_AVG		"turbine_passage"
#define ART_SPILL_AVG		"spill_passage"
#define ART_RSW_AVG		"rsw_passage"
#define ART_TRANSPORT_AVG	"transport_passage"
#define ART_PREMORT_TOTAL	"premort_transport_total"
#define ART_POSTMORT_TOTAL	"postmort_transport_total"
#define ART_FPE			"fpe"
#define ART_SLUICEWAY_IN_AVG	"sluiceway_input"
#define ART_BYPASS_IN_AVG	"bypass_input"
#define ART_TURBINE_IN_AVG	"turbine_input"
#define ART_SPILL_IN_AVG	"spill_input"
#define ART_RSW_IN_AVG	"rsw_input"
#define ART_PROJECT_IN_TOTAL	"project_input"
