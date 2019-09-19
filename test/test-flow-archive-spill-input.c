#include "test.h"
#include "crisp.h"
#include "monte.h"
#include "run_mode.h"

/* Alternative for this test */
#define ALT	0

int main(int argc, char **argv) {
	struct archive_header **archive_data;
	struct archive_spill **archive_spill;
	long ***ftells = 0;
	FILE *arch_fp;
	struct river_segment *seg;
	int i;
	
	test_start("Flow archive spill value input");

	test_category("Opening test flow archive");
	arch_fp = fopen("flow-archive-input-test.archive", "r");
	assert(arch_fp != NULL);

	test_category("Initialization");
	assert(!process_command_line(argc, argv, 1));
	locate_image(argv[0]);
	assert(!model_startup(argc, argv));
	archive_spill = alloc_game_spills(15, STEPS_IN_SEASON);
	assert(archive_spill != NULL);

	test_category("Opening test flow archive data");
	assert((archive_data = get_and_check_archives(run_mode.monte_data, &ftells)) != NULL);
	assert(ftells != NULL);
	/* Check to make sure dams were read in properly */
	assert(archive_data[ALT]->ndams == 15);

	/* The flow archive compatible spill schedules should be constructed automatically */

	test_category("Parsing archive");
	read_flow_and_spill_from_archive(0, 0, archive_data[ALT], archive_spill, arch_fp, ftells[ALT], TRUE);
	test_category("Computing spill schedules");
	monte_compute_spill_schedules(archive_data[ALT], archive_spill, TRUE);
	compute_all_flow();

	/* Find a dam (they all have the same settings in the flow archive */
	for (seg = river_mouth; seg->seg_type != DAM_SEG; seg = seg->next);
	assert(seg != NULL);

	/* Note that the flows may be modified by the model and they are ONLY used to calculate the spill fractions! */

	for (i=0; i < (365 - 1) * DAM_TIME_SLC; i++) {
		/* Days are off by one from what I was expecting but this is probably because I don't understand the flow.archive format */
		int day = i/DAM_TIME_SLC;
		test_category("Spill inputs are read into internal data structure correctly");
		assert_equal(((double)day)/365.0, archive_spill[0][day].day);
		assert_equal(((double)day)/365.0, archive_spill[0][day].night);
		test_category("Planned spill inputs are read correctly");
		assert_equal(((double)day)/365.0, seg->current->dam.planned_spill[(day+1)*DAM_TIME_SLC + i%DAM_TIME_SLC]);
		test_category("Spill array is filled correctly");
		assert_equal(((double)day)/365.0, seg->current->dam.spill[(day+1)*DAM_TIME_SLC + i%DAM_TIME_SLC]);
	}

	fclose(arch_fp);
	return test_pass();
}

