#include "test.h"
#include "crisp.h"
#include "period.h"

int main(int argc, char *argv) {
	struct river_segment seg_s;
	struct river_segment seg2_s;
	struct river_segment *seg = &seg_s;
	struct river_segment *seg2 = &seg2_s;

	test_start("Flow min for multi-powerhouse dam spill fractions");

	/* Initialize segments */
	seg->current = &seg->run[0];
	seg2->current = &seg2->run[0];

	{
		int day = 123; /* Some arbitrary day */
		int slice;
		struct dam *dam = &seg->current->dam;
		struct dam *ph2 = &seg2->current->dam;

		/* Test parameters */
		float spill = 0.5f;
		float flow_min = 5.0f;
		float flow_min2;
		float hydrocap;
		float hydrocap2;
		float spillcap;
		float flow = 10.0f;
		float max_spill = (flow - flow_min) / flow;

		test_category("Initialization of powerhouse_schedule");
		dam->phouse_schedule = make_default_phouse_schedule();
		ph2->phouse_schedule = make_default_phouse_schedule();

		test_category("Initialization of planned_spill[]");
		dam->planned_spill_day = new_period_struct();
		dam->planned_spill_day->start = 0;
		dam->planned_spill_day->end = 365;
		dam->planned_spill_day->value = spill;
		dam->planned_spill_night = new_period_struct();
		dam->planned_spill_night->start = 0;
		dam->planned_spill_night->end = 365;
		dam->planned_spill_night->value = spill;
		set_dam_planned_spill(seg);
		/* Test */
		for (slice = day*DAM_TIME_SLC; slice < (day+1) * DAM_TIME_SLC; slice++) {
			assert(dam->planned_spill[slice] == spill);
		}

		/* Test with multiple powerhouses */
		test_category("Initialization of planned_spill[]");
		dam->additional_pwrhs = seg2;
		ph2->additional_pwrhs = NULL;
		dam->phouse_priority = 0;
		dam->num_powerhouses = 2;
		ph2->phouse_priority = 1;

		/* If flow is too low, the model continues so this condition is not checked */

		test_category("Not enough flow for full spill, ph2 disabled (parameters)");
		flow_min = 6.0f;
		flow_min2 = 2.0f;
		hydrocap = 6.0f;
		hydrocap2 = 5.0f;
		spillcap = 8.0f;
		assert(flow < flow_min + spill*flow);
		test_category("Not enough flow for full spill, ph2 disabled");
		dam->flow_min_project = flow_min;
		dam->powerhouse_capacity = hydrocap;
		ph2->flow_min_project = flow_min2;
		ph2->powerhouse_capacity = hydrocap2;
		dam->flow[day] = flow;
		set_dam_flow_routing(seg, day);
		assert(dam->spill[day * DAM_TIME_SLC] < spill);

		test_category("PH1 to hydraulic capacity ph2 disabled (parameters)");
		flow_min = 4.0f;
		flow_min2 = 2.0f;
		hydrocap = 5.0f;
		hydrocap2 = 5.0f;
		spillcap = 8.0f;
		assert(flow < hydrocap + flow_min2 + spill * flow);
		assert(flow >= hydrocap + spill * flow);
		test_category("PH1 to hydraulic capacity ph2 disabled");
		dam->flow_min_project = flow_min;
		dam->powerhouse_capacity = hydrocap;
		ph2->flow_min_project = flow_min2;
		ph2->powerhouse_capacity = hydrocap2;
		dam->flow[day] = flow;
		set_dam_flow_routing(seg, day);
		assert(dam->spill[day * DAM_TIME_SLC] == spill);
		/* Don't check the flow since set_powerhouse_fraction() wasn't used */

		/* No need to test if ph2 is being used since spill should stay full */

		test_category("Excess (but acceptable) spill (parameters)");
		flow_min = 0.5f;
		flow_min2 = 0.5f;
		hydrocap = 1.0f;
		hydrocap2 = 1.0f;
		spillcap = 8.0f;
		assert(flow <= hydrocap + hydrocap2 + spillcap);
		assert(flow > hydrocap + hydrocap2 + spill * flow);
		test_category("Excess (but acceptable) spill");
		dam->flow_min_project = flow_min;
		dam->powerhouse_capacity = hydrocap;
		dam->spill_cap = spillcap;
		ph2->flow_min_project = flow_min2;
		ph2->powerhouse_capacity = hydrocap2;
		dam->flow[day] = flow;
		set_dam_flow_routing(seg, day);
		assert(dam->spill[day * DAM_TIME_SLC] > spill);

		/* Beyond maximum acceptable range the model continues so there is no error to check for */
	}

	return test_pass();
}

