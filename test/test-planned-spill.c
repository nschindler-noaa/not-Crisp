#include "test.h"
#include "crisp.h"
#include "period.h"

int main(int argc, char *argv) {
	struct river_segment seg_s;
	struct river_segment *seg = &seg_s;

	test_start("Spill schedule to planned spill");

	/* Initialize segment */
	seg->current = &seg->run[0];

	{
		int day;
		int slice;
		struct dam *dam = &seg->current->dam;

		test_category("Initialization of dam segment");
		assert(dam != NULL);
		assert(dam->planned_spill != NULL);
		
		test_category("Dam with no spill schedule");
		dam->planned_spill_day = NULL;
		dam->planned_spill_night = NULL;
		set_dam_planned_spill(seg);
		for (day=0; day < DAYS_IN_SEASON; day++) {
			for (slice = day*DAM_TIME_SLC; slice < (day+1)*DAM_TIME_SLC; slice++) {
				assert(dam->planned_spill[slice] == 0.0f);
			}
		}
		
		test_category("Dam with all day spill");
		dam->planned_spill_day = new_period_struct();
		dam->planned_spill_night = NULL;
		dam->planned_spill_day->start = 0;
		dam->planned_spill_day->end = 365;
		dam->planned_spill_day->value = 1.0f;
		set_dam_planned_spill(seg);
		for (day=0; day < DAYS_IN_SEASON; day++) {
			for (slice = day*DAM_TIME_SLC; slice < (day+1)*DAM_TIME_SLC; slice++) {
				if (!is_night(slice % DAM_TIME_SLC)) {
					assert(dam->planned_spill[slice] == 1.0f);
				}
			}
		}

		/* TODO: more tests, obviously. But this code seems to be
		 * working, so this is a low priority... */
	}

	return test_pass();
}

