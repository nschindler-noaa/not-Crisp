CONFIG += console staticlib
CONFIG -= qt thread windows x11 plugin dll
DEFINES += SOLARIS2 NOVAR ROUTE_TRACKING REALTIME DOWNSTREAM NOMINMAX
SOURCES += altern_list.c \
	args.c \
	at_frame.c \
	at_hist.c \
	base_gas_mort.c \
	basin.c \
	batch.c \
	command.c \
	compute_gas_mort.c \
	crisp.c \
	crisp1bat.c \
	custom_survival.c \
	dam_model.c \
	dataio.c \
	daylight.c \
	delay.c \
	delayed_mort.c \
	return.c \
	dir.c \
	dump.c \
	eqns_dflt.c \
	eqns_util.c \
	exp_table.c \
	fge.c \
	file.c \
	flow.c \
	fpe.c \
	gas_mort.c \
	gas_mort_eqn.c \
	gc_dist.c \
	growth.c \
	headw.c \
	high_gas_mort.c \
	include.c \
	init.c \
	init_river.c \
	list.c \
	log.c \
	main.c \
	migration.c \
	moment.c \
	monte.c \
	monte_frame.c \
	monte_index.c \
	monte_output.c \
	monte_spill.c \
	multi_frame.c \
	nrutil_short.c \
	parse.c \
	parse_alt.c \
	parse_archive.c \
	parse_at.c \
	parse_monte_index.c \
	parseutil.c \
	passage.c \
	pbn.c \
	period.c \
	phouse.c \
	pred.c \
	reach_model.c \
	realtime.c \
	release.c \
	release_util.c \
	report_altern.c \
	report_frame.c \
	report_init.c \
	report_out.c \
	report_setup.c \
	report_tool.c \
	river.c \
	rsw.c \
	rz_model.c \
	sample.c \
	scenario_tracking.c \
	spec_ext.c \
	species.c \
	spill.c \
	stock.c \
	strtools.c \
	summary.c \
	supersat.c \
	table.c \
	time.c \
	token.c \
	tokens_alt.c \
	tokens_archive.c \
	tokens_at.c \
	tokens_mindex.c \
	transport.c \
	transport_mort.c \
	transport_util.c \
	travel_table.c \
	tree.c \
	var.c \
	vectable.c \
	water_travel.c \
	writeutil.c

HEADERS += alt_report.h \
	altern_list.h \
	args.h \
	basin.h \
	batch.h \
	c1tokens.h \
	command.h \
	compute_gas_mort.h \
	crisp.h \
	crisp_defs.h \
	crisp_time.h \
	custom_survival.h \
	dataio.h \
	date.h \
	daylight.h \
	defaults.h \
	delay.h \
	delayed_mort.h \
	return.h \
	dir.h \
	disp_defs.h \
	dump.h \
	eqns.h \
	exp_table.h \
	file.h \
	filerext.h \
	flow.h \
	fpe.h \
	gas_mort.h \
	gas_mort_params.h \
	gc_dist.h \
	gif.h \
	gui.h \
	growth.h \
	help.h \
	implicit.h \
	implicit_base.h \
	include.h \
	init.h \
	init_river.h \
	input_hist.h \
	list.h \
	log.h \
	main.h \
	main_app_inc.h \
	map.h \
	mapConstants.h \
	mapPoint.h \
	migration.h \
	mode.h \
	monte.h \
	monte_index.h \
	monte_output.h \
	mouse.h \
	mouse_tool_base.h \
	nrutil_short.h \
	nsat.h \
	output_flags.h \
	parse.h \
	parser_file.h \
	parseutil.h \
	passage.h \
	period.h \
	phouse.h \
	pred.h \
	probability.h \
	reach_gas_specs.h \
	realtime.h \
	realtime_defs.h \
	realtime_types.h \
	release.h \
	release_util.h \
	report.h \
	resource.h \
	resrc1.h \
	river.h \
	rsw.h \
	rtrr.h \
	run_mode.h \
	run_state.h \
	sample.h \
	schedule.h \
	smart_ptr.h \
	species.h \
	spill.h \
	stock.h \
	strtools.h \
	struct_at.h \
	table.h \
	token.h \
	tokens_alt.h \
	tokens_archive.h \
	tokens_at.h \
	tokens_mindex.h \
	transport.h \
	travel_table.h \
	tree.h \
	ttopt.h \
	var.h \
	vectable.h \
	writeutil.h
