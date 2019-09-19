/*
 * qIdcEditGraph.h
 * This file contains several classes derived from qIdcEditGraphBase.
 *  Each class defines an interface data container for a particular
 *  type of data in the data base. These are grouped together because
 *  each class is likely to be small, but there are many classes.
 */

#include "qIdcEditGraph.h"
#include "qUtils.h"
#include "log.h"

#define SCALE_FACTOR	1.98347 /* Scaling factor for kcfs (flow) to give
	kiloacre-feet (volume).  If you change it here, change in flow.c. */

/*
 * Get data from headwater segment described by getString(0).
 *  Return 1 for success, 0 for failure.
 */
int qIdcegHeadwaterFlow::FillData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), HEADWATER_SEG );
	if (seg == NULL || seg->current == NULL)
		return 0;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		dat->setDouble( i, seg->current->headwater.flow[i] );
	dat->clearDirty();

	return 1;
}

/*
 * Put data from qDatContainer back into headwater segment described
 *  by getString(0). No reporting of errors. Hopefully this will work.
 */
void qIdcegHeadwaterFlow::ApplyData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), HEADWATER_SEG );
	if (seg == NULL || seg->current == NULL)
		return;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		seg->current->headwater.flow[i] = dat->getDouble( i );
	dat->clearDirty();
	qIdcEditGraphBase::ApplyData( dat );

	return;
}

/*
 * Get data from headwater segment described by getString(0).
 *  Return 1 for success, 0 for failure.
 */
int qIdcegHeadwaterTemp::FillData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), HEADWATER_SEG );
	if (seg == NULL || seg->current == NULL)
		return 0;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		dat->setDouble( i, 
		       seg->current->headwater.water_temp[i*STEPS_PER_DAY] );
	dat->clearDirty();

	return 1;
}

/*
 * Put data from qDatContainer back into headwater segment described
 *  by getString(0). No reporting of errors. Hopefully this will work.
 */
void qIdcegHeadwaterTemp::ApplyData(qDatContainer *dat) {
	int i,j;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), HEADWATER_SEG );

	if (seg == NULL || seg->current == NULL)
		return;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		for (j = 0; j < STEPS_PER_DAY; j++)
		seg->current->headwater.water_temp[i*STEPS_PER_DAY + j]
		                                            = dat->getDouble( i );
	dat->clearDirty();
	qIdcEditGraphBase::ApplyData( dat );

	return;
}

/*
 * Get data from headwater segment described by getString(0).
 *  Return 1 for success, 0 for failure.
 */
int qIdcegHeadwaterTurbidity::FillData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), HEADWATER_SEG );
	if (seg == NULL || seg->current == NULL)
		return 0;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		dat->setDouble( i, 
		       seg->current->headwater.turbidity[i*STEPS_PER_DAY] );
	dat->clearDirty();

	return 1;
}

/*
 * Put data from qDatContainer back into headwater segment described
 *  by getString(0). No reporting of errors. Hopefully this will work.
 */
void qIdcegHeadwaterTurbidity::ApplyData(qDatContainer *dat) {
	int i,j;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), HEADWATER_SEG );
	if (seg == NULL || seg->current == NULL)
		return;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		for (j = 0; j < STEPS_PER_DAY; j++)
		seg->current->headwater.turbidity[i*STEPS_PER_DAY + j]
		                                            = dat->getDouble( i );
	dat->clearDirty();
	qIdcEditGraphBase::ApplyData( dat );

	return;
}

/*
 * Get data from dam segment (w. storage basin) described by getString(0).
 *  Return 1 for success, 0 for failure.
 */
int qIdcegBasinVolume::FillData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL || seg->current == NULL)
		return 0;
	if (seg->current->dam.storage_basin == NULL)
		return 0;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		dat->setDouble( i, seg->current->dam.storage_basin->volume[i] );
	dat->clearDirty();

	return 1;
}



/*
 * Put data from qDatContainer back into dam segment (w. basin) described
 *  by getString(0). No reporting of errors. Hopefully this will work.
 */
void qIdcegBasinVolume::ApplyData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL || seg->current == NULL)
		return;
	if (seg->current->dam.storage_basin == NULL)
		return;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		seg->current->dam.storage_basin->volume[i] = dat->getDouble( i );
	dat->clearDirty();
	qIdcEditGraphBase::ApplyData( dat );

	return;
}

/* Make basin volumes legal given min and max basin volumes
 *  and current headwater flows.
 */
void qIdcegBasinVolume::makeLegal( const char *id ) {
	int i;
	river_segment *seg;

	/* If incoming argument is NULL, that means do this operation to
	 *  all dams with storage basins.
	 */
	if (id == NULL) {
		for (seg = river_mouth; seg != NULL; seg = seg->next)
			if (seg->seg_type == DAM_SEG)
				if (seg->current->dam.storage_basin != NULL)
					makeLegal( seg->seg_name );
		return;
	}

	/* First get the river segment identified by "id" and make sure it
	 *  is a dam with a storage basin.
	 */
	seg = getRiverSegmentHandle( id, DAM_SEG );
	if (seg == NULL || seg->current == NULL)
		return;
	if (seg->current->dam.storage_basin == NULL)
		return;

	double min_volume = seg->current->dam.storage_basin->min_volume;
	double max_volume = seg->current->dam.storage_basin->max_volume;

	/* Then get the inflow from the headwater just upstream. */
	qDatContainer *inflow = qIdcegHeadwaterFlow_.GetData( seg->up->seg_name );
	if (inflow == NULL)
		return;

	/* Then get the volumes requested. */
	qDatContainer *volume = GetData( id );
	if (volume == NULL)
		return;

	double savevol, vol;
	double prev = volume->getDouble(0);
	double inf = 0.0;
	int changed = 0;
	for (i = 0; i < DAYS_IN_SEASON; i++) {
		savevol = vol = volume->getDouble( i );
		if (vol < min_volume)
			vol = min_volume;
		if (vol > max_volume)
			vol = max_volume;
		if (vol > prev + inf * SCALE_FACTOR)
			vol = prev + inf * SCALE_FACTOR;

		/* Set up previous day's volume and inflow for next loop */
		if (vol != savevol) {
			ModifyCache( id, i, vol );
			changed = 1;
		}
		prev = vol;
		inf = inflow->getDouble(i);
	}
	if (changed)
		invalidate();  //data container was changed, notify all displaying graphs
		//emit evInvalidate_();
}

/* qIdcegEditDamFlow is a weird IDC because it really doesn't edit the
 *  underlying data base - it just exists to use the editing UI functionality
 *  to edit qIdcegBasinVolume data indirectly.
 */
/*
 * Get data from dam segment (w. storage basin) described by getString(0).
 *  Return 1 for success, 0 for failure.
 */
int qIdcegEditDamFlow::FillData(qDatContainer *dat) {
	int i;

	/* First get the river segment and make sure it
	 *  is a dam with a storage basin.
	 */
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL || seg->current == NULL)
		return 0;
	if (seg->current->dam.storage_basin == NULL)
		return 0;

	/* Now get the data from the underlying river segment into the
	 *  qIdc cache structure.
	 */
	for (i = 0; i < DAYS_IN_SEASON; i++)
		dat->setDouble( i, seg->current->dam.flow[i] );
	dat->clearDirty();
	return 1;
}

/*
 * Put data from qDatContainer back into dam segment (w. basin) described
 *  by getString(0). No reporting of errors. Hopefully this will work.
 */
void qIdcegEditDamFlow::ApplyData(qDatContainer *dat) {
	int i;

	/* First get the river segment and make sure it
	 *  is a dam with a storage basin.
	 */
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL || seg->current == NULL)
		return;
	if (seg->current->dam.storage_basin == NULL)
		return;

	/* Now get the data from the underlying river segment into the
	 *  qIdc cache structure.
	 */
	for (i = 0; i < DAYS_IN_SEASON; i++)
		seg->current->dam.flow[i] = dat->getDouble( i );
	dat->clearDirty();
	qIdcEditGraphBase::ApplyData( dat );

	return;
}

/* Calculate what the flow must be given the
 *  headwater flow and the storage basin volume for that dam and it's
 *  headwater.
 */
void qIdcegEditDamFlow::calcFlow( const char *id ) {
	int i;
	river_segment *seg;

	/* If incoming argument is NULL, that means do this operation to
	 *  all dams with storage basins.
	 */
	if (id == NULL) {
		for (seg = river_mouth; seg != NULL; seg = seg->next)
			if (seg->seg_type == DAM_SEG)
				if (seg->current->dam.storage_basin != NULL)
					calcFlow( seg->seg_name );
		return;
	}

	/* First get the river segment identified by "id" and make sure it
	 *  is a dam with a storage basin.
	 */
	seg = getRiverSegmentHandle( id, DAM_SEG );
	if (seg == NULL || seg->current == NULL)
		return;
	if (seg->current->dam.storage_basin == NULL)
		return;

	/* Then get the storage basin volume. */
	qDatContainer *volume = qIdcegBasinVolume_.GetData( seg->seg_name );
	if (volume == NULL)
		return;

	/* Then get the inflow from the headwater just upstream. */
	qDatContainer *inflow = qIdcegHeadwaterFlow_.GetData( seg->up->seg_name );
	if (inflow == NULL)
		return;

	/* Now calculate what the flow must be. */
	for (i = 0; i < DAYS_IN_SEASON-1; i++) {
		ModifyCache( id, i, inflow->getDouble(i) + 
					(volume->getDouble(i) - volume->getDouble(i+1))
									/ SCALE_FACTOR );
	}
	ModifyCache( id, DAYS_IN_SEASON - 1,
					inflow->getDouble(DAYS_IN_SEASON-1) );
	
	/* Update any windows displaying this flow data. */
	//emit evInvalidate_();
	invalidate();

}

/* Caluclate what the volume must be for a given
 *  headwater flow and dam outflow. Make sure the volume is legal
 *  volume[0] will be the initial volume and
 *  will not be disturbed - furthermore, volumes [1] to [DAYS_IN_SEASON-1]
 *  will depend on it.
 */
void qIdcegEditDamFlow::calcVolume( const char *id ) {
	int i;
	double prev, vol, min_vol, max_vol;
	river_segment *seg; 

	/* If incoming argument is NULL, that means do this operation to
	 *  all dams with storage basins.
	 */
	if (id == NULL) {
		for (seg = river_mouth; seg != NULL; seg = seg->next)
			if (seg->seg_type == DAM_SEG)
				if (seg->current->dam.storage_basin != NULL)
					calcVolume( seg->seg_name );
		return;
	}

	/* First get the river segment identified by "id" and make sure it
	 *  is a dam with a storage basin.
	 */
	seg = getRiverSegmentHandle( id, DAM_SEG );
	if (seg == NULL || seg->current == NULL)
		return;
	if (seg->current->dam.storage_basin == NULL)
		return;

	/* Then get the dam outflow. */
	qDatContainer *outflow = GetData( seg->seg_name );
	if (outflow == NULL)
		return;

	/* Then get the inflow from the headwater just upstream. */
	qDatContainer *inflow = qIdcegHeadwaterFlow_.GetData( seg->up->seg_name );
	if (inflow == NULL)
		return;

	/* Then get the data continer for the basin volumes. */
	qDatContainer *volume = qIdcegBasinVolume_.GetData( seg->seg_name );
	if (volume == NULL)
		return;

	/* Now calculate what the volumes must be. */
	min_vol = seg->current->dam.storage_basin->min_volume;
	max_vol = seg->current->dam.storage_basin->max_volume;
	prev = volume->getDouble( 0 );

	for (i = 1; i < DAYS_IN_SEASON; i++) {
		vol = prev + (inflow->getDouble( i-1 ) - outflow->getDouble( i-1 ))
								* SCALE_FACTOR;
		if (vol < min_vol)
			vol = min_vol;
		if (vol > max_vol)
			vol = max_vol;
		qIdcegBasinVolume_.ModifyCache( id, i, vol );
		prev = vol;
	}

}

/* Reach loss stuff.
 *
 * Get data from reach segment described by getString(0).
 *  Return 1 for success, 0 for failure.
 */
int qIdcegFlowLoss::FillData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), REACH_SEG );
	if (seg == NULL || seg->current == NULL)
		return 0;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		dat->setDouble( i, seg->current->reach.loss[i] );
	dat->clearDirty();

	return 1;
}

/*
 * Put data from qDatContainer back into reach segment described
 *  by getString(0). No reporting of errors. Hopefully this will work.
 */
void qIdcegFlowLoss::ApplyData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), REACH_SEG );
	if (seg == NULL || seg->current == NULL)
		return;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		seg->current->reach.loss[i] = dat->getDouble( i );
	dat->clearDirty();
	qIdcEditGraphBase::ApplyData( dat );
	
	return;
}

/* Elevation change stuff.
 *
 * Get data from reach segment described by getString(0).
 *  Return 1 for success, 0 for failure.
 */
int qIdcegElevationChange::FillData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), REACH_SEG );
	if (seg == NULL || seg->current == NULL)
		return 0;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		dat->setDouble( i, seg->current->reach.elev_change[i] );
	dat->clearDirty();

	return 1;
}

/*
 * Put data from qDatContainer back into reach segment described
 *  by getString(0). No reporting of errors. Hopefully this will work.
 */
void qIdcegElevationChange::ApplyData(qDatContainer *dat) {
	int i;
	river_segment *seg 
		= getRiverSegmentHandle( dat->getString(0), REACH_SEG );
	if (seg == NULL || seg->current == NULL)
		return;

	for (i = 0; i < DAYS_IN_SEASON; i++)
		seg->current->reach.elev_change[i] = dat->getDouble( i );
	dat->clearDirty();
	qIdcEditGraphBase::ApplyData( dat );
	return;
}


/* Static objects instantiate these classes (one object per class.).
 */
qIdcegHeadwaterFlow qIdcegHeadwaterFlow_;
qIdcegHeadwaterTemp qIdcegHeadwaterTemp_;
qIdcegHeadwaterTurbidity qIdcegHeadwaterTurbidity_;
qIdcegBasinVolume qIdcegBasinVolume_;
qIdcegEditDamFlow qIdcegEditDamFlow_;
qIdcegFlowLoss qIdcegFlowLoss_;
qIdcegElevationChange qIdcegElevationChange_;
