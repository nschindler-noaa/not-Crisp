# EXTRA_LOCAL: any extra local code that needs to be linked in
EXTRA_LOCAL = 

# EXTRA_FOREIGN: any extra code that needs to be linked in
EXTRA_FOREIGN = tree.o spill.o crisp.o log.o sample.o period.o time.o phouse.o parse_archive.o parse.o parseutil.o include.o token.o init.o transport_util.o migration.o scenario_tracking.o var.o report_init.o main.o monte.o monte_output.o list.o monte_frame.o multi_frame.o file.o dir.o flow.o water_travel.o supersat.o river.o basin.o species.o spec_ext.o realtime.o release_util.o fge.o summary.o writeutil.o dataio.o stock.o eqns_dflt.o release.o delayed_mort.o dump.o fpe.o report_altern.o report_out.o report_frame.o altern_list.o at_hist.o moment.o at_frame.o monte_index.o parse_at.o command.o tokens_at.o parse_monte_index.o tokens_mindex.o monte_spill.o travel_table.o vectable.o batch.o parse_alt.o tokens_alt.o init_river.o gc_dist.o args.o dam_model.o transport.o transport_mort.o rz_model.o pred.o custom_survival.o growth.o return.o reach_model.o passage.o delay.o rsw.o gas_mort.o compute_gas_mort.o high_gas_mort.o exp_table.o nrutil_short.o base_gas_mort.o daylight.o eqns_util.o table.o tokens_archive.o pbn.o extra_batch.o

# SRC_FOREIGN: directory in which the objects in EXTRA_FOREIGN are located (to save typing)
SRC_FOREIGN = ..


# Tests are named test-NAME.c and are found by wildcard
TESTC := $(wildcard test-*.c)
TESTS := $(patsubst %.c,%,$(TESTC))

# results.txt is written by run_tests.sh and has the results
results.txt: $(TESTS) run_tests.sh Makefile
	./run_tests.sh $(TESTS)

check: results.txt

clean:
	rm -f $(TESTS) $(EXTRA_FOREIGN) $(EXTRA_LOCAL)

$(TESTS): % : %.c test.c test.h $(EXTRA_FOREIGN) $(EXTRA_LOCAL)
	gcc -g -I$(SRC_FOREIGN) -o $@ $< test.c $(EXTRA_LOCAL) $(EXTRA_FOREIGN) -lm

# Extra local code
$(EXTRA_LOCAL): %.o : %.c
	gcc -g -I$(SRC_FOREIGN) -c $<

# The objects are copied into this directory
# so that the tests could be built elsewhere
$(EXTRA_FOREIGN): % : $(SRC_FOREIGN)/%
	cp $< $@

