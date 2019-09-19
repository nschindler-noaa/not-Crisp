#include "test.h"
#include "crisp.h"
#include "period.h"
#include "spill.h"
#include "phouse.h"

int main(int argc, char *argv) {
	struct river_segment seg_s;
	struct river_segment seg2_s;
	struct river_segment *seg = &seg_s;
	struct river_segment *seg2 = &seg2_s;

	test_start("Flow partitioning in two-powerhouse dam");

	/* Initialize segments */
	seg->current = &seg->run[0];
	seg2->current = &seg2->run[0];

	{
		int day, slice;
		struct dam *dam = &seg->current->dam;
		struct dam *ph2 = &seg2->current->dam;

		/* Test parameters */
		float spill = 0.2f;
		float flow_min = 5.0f;
		float flow_min2 = 4.0f;
		float hydrocap = 10.0f;
		float hydrocap2 = 7.0f;
		float spillcap = 9.0f;

		test_category("Initialization of flow ramp");
		for (day=0; day < DAYS_IN_SEASON; day++) {
			/* Set the flow to increase linearly with the day up to the maximum allowable flow */
			float flow = (hydrocap + hydrocap2 + spillcap) * (day + 1) / (DAYS_IN_SEASON * 9/10);
			dam->flow[day] = flow;
		}

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
		for (day=0; day < DAYS_IN_SEASON; day++) {
			for (slice = day*DAM_TIME_SLC; slice < (day+1) * DAM_TIME_SLC; slice++) {
				assert(dam->planned_spill[slice] == spill);
			}
		}

		/* Test with multiple powerhouses */
		test_category("Initialization of planned_spill[]");
		dam->additional_pwrhs = seg2;
		ph2->additional_pwrhs = NULL;
		dam->phouse_priority = 0;
		dam->num_powerhouses = 2;
		ph2->phouse_priority = 1;

		test_category("Flow routing");
		dam->flow_min_project = flow_min;
		dam->powerhouse_capacity = hydrocap;
		dam->spill_cap = spillcap;
		ph2->flow_min_project = flow_min2;
		ph2->powerhouse_capacity = hydrocap2;
		for (day=0; day < DAYS_IN_SEASON; day++) {
			set_dam_flow_routing(seg, day);
			for (slice = day*DAM_TIME_SLC; slice < (day+1) * DAM_TIME_SLC; slice++) {
				printf("%f\t%f\t%f\t%f\n", dam->flow[day], dam->flow_fraction[slice] * dam->flow[day], ph2->flow_fraction[slice] * dam->flow[day], dam->spill[slice] * dam->flow[day]);
			}
		}

		for (day=0; day < DAYS_IN_SEASON; day++) {
			double flow = dam->flow[day];
			for (slice = day*DAM_TIME_SLC; slice < (day+1) * DAM_TIME_SLC; slice++) {
				double spill_fraction = dam->spill[slice];
				double ph1_fraction = dam->flow_fraction[slice];
				double ph2_fraction = ph2->flow_fraction[slice];
	
				if (flow >= flow_min) {
					test_category("Flow is fully partitioned");
					assert_equal(1.0, spill_fraction + ph1_fraction + ph2_fraction);

					test_category("Flow does not exceed hydraulic capacity");
					assert_less_than(ph1_fraction * flow, hydrocap);
					assert_less_than(ph2_fraction * flow, hydrocap2);

					test_category("Spill as requested (if possible)");
					if (flow >= (flow_min + spill*flow) && flow <= (hydrocap + hydrocap2 + spill * flow)) assert_equal(spill, spill_fraction);

					/* Note that we allow spill to exceed spill capacity, but there will be a warning emitted and hopefully the user will notice */
					if (flow <= (hydrocap + hydrocap2 + spillcap)) {
						test_category("Spill does not exceed spill capacity");
						assert(spill_fraction * flow <= spillcap);
					}
				}
			}
		}
	}

	return test_pass();
}

