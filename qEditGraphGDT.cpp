/* qEditGraphGDT.h
 * This file contains a lot of small classes which define graph data
 *  token types derived from qEditGraphDataToken.
 */

#include <stdio.h>
#include "crisp_defs.h"
#include "qUtils.h"
#include "qGraphDataTokenTypes.h"
#include "qEditGraphGDT.h"
#include "qIdcEditGraph.h"
#include "mapConstants.h"
#include "log.h"

/* NOTE evTrigger = -10 implies that live updating is not enabled for
 *  headwaters. This is because the map mediator doesn't seem to report
 *  events on them.
 * TODO get live updating working for headwaters.
 */
qHeadwaterFlowGDT::qHeadwaterFlowGDT( const char *id ) :
		qEditGraphDataToken( "Flow for", id, GDTT_HEADWATER_Flow,
			                  HEADWATER, &qIdcegHeadwaterFlow_ )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.count() < DAYS_IN_SEASON)
		dataArray_.append( 0.0 );
	insertLabel("Flow Rate (kcfs)");
	setHelpPage( 42 );
	openInstances_->append(this);
	Fill( id );
}

qHeadwaterFlowGDT::~qHeadwaterFlowGDT() {
	openInstances_->remove(this);
}

/* Headwater flow directly influences storage basin volume for those
 *  rivers which have a dam as the upstream-most segment. Therefore
 *  we intercept any UI actions that change headwater flow and also
 *  force updates of the associated basin volume and outflow if necessary.
 */
void qHeadwaterFlowGDT::Apply( void ) {
	qEditGraphDataToken::Apply();

	qIdcegEditDamFlow_.invalidate();
}

void qHeadwaterFlowGDT::ApplyAll( void ) {
	qEditGraphDataToken::ApplyAll();

	qIdcegEditDamFlow_.invalidate();
}

void qHeadwaterFlowGDT::Reset( void ) {
	qEditGraphDataToken::Reset();
	prodBasin();
}

void qHeadwaterFlowGDT::ResetAll( void ) {
	qEditGraphDataToken::ResetAll();
	prodBasin();
}

void qHeadwaterFlowGDT::SetData( void ) {
	qEditGraphDataToken::SetData();
	prodBasin();
}

void qHeadwaterFlowGDT::prodBasin( void ) {
	river_segment *seg 
		= getRiverSegmentHandle( sourceStr_.ascii(), HEADWATER_SEG );
	if (seg == NULL)
		return;
	seg = seg->down; // segment below headwater
	if (seg == NULL)
		return;
	if (seg->seg_type != DAM_SEG)
		return;
	if (seg->current == NULL)
		return;
	if (seg->current->dam.storage_basin == NULL)
		return;

	//qIdcegBasinVolume_.makeLegal( seg->seg_name );
	//qIdcegEditDamFlow_.calcFlow( seg->seg_name );
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qHeadwaterFlowGDT::groupYMax_ = 0.0;
double qHeadwaterFlowGDT::groupYMin_ = 0.0;
QPtrList<qHeadwaterFlowGDT> * qHeadwaterFlowGDT::openInstances_ = new QPtrList<qHeadwaterFlowGDT>;
//signal_null qHeadwaterFlowGDT::evScaleChanged_;

void qHeadwaterFlowGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qHeadwaterFlowGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qHeadwaterFlowGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qHeadwaterFlowGDT::getGroupYMax( void ) {
	return groupYMax_;
}
void qHeadwaterFlowGDT::cbGroupYScaleChanged(){
		
	QPtrListIterator<qHeadwaterFlowGDT>* itr = new QPtrListIterator<qHeadwaterFlowGDT>(*qHeadwaterFlowGDT::openInstances_);
	qHeadwaterFlowGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}


qHeadwaterTempGDT::qHeadwaterTempGDT( const char *id ) :
		qEditGraphDataToken( "Water Temperature for", id, GDTT_HEADWATER_WaterTemp,
			                  HEADWATER, &qIdcegHeadwaterTemp_ )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON );
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.count() < DAYS_IN_SEASON )
		dataArray_.append( 0.0 );
	insertLabel("Water Temperature (C)");
	setHelpPage( 43 );
	openInstances_->append(this);
	Fill( id );
}

qHeadwaterTempGDT::~qHeadwaterTempGDT() {
	openInstances_->remove(this);
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qHeadwaterTempGDT::groupYMax_ = 0.0;
double qHeadwaterTempGDT::groupYMin_ = 0.0;
QPtrList<qHeadwaterTempGDT> * qHeadwaterTempGDT::openInstances_ = new QPtrList<qHeadwaterTempGDT>;


void qHeadwaterTempGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qHeadwaterTempGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qHeadwaterTempGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qHeadwaterTempGDT::getGroupYMax( void ) {
	return groupYMax_;
}
void qHeadwaterTempGDT::cbGroupYScaleChanged(){

	QPtrListIterator<qHeadwaterTempGDT>* itr = new QPtrListIterator<qHeadwaterTempGDT>(*qHeadwaterTempGDT::openInstances_);
	qHeadwaterTempGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}
/*
signal_null *qHeadwaterTempGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/

/* *********** */
/* **** */
qHeadwaterTurbidityGDT::qHeadwaterTurbidityGDT( const char *id ) :
		qEditGraphDataToken( "Turbidity for", id, GDTT_HEADWATER_WaterTemp,
			                  HEADWATER, &qIdcegHeadwaterTurbidity_ )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON );
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.count() < DAYS_IN_SEASON )
		dataArray_.append( 0.0 );
	insertLabel("Turbidity");
	setHelpPage( 443 );
	openInstances_->append(this);
	Fill( id );
}

qHeadwaterTurbidityGDT::~qHeadwaterTurbidityGDT() {
	openInstances_->remove(this);
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qHeadwaterTurbidityGDT::groupYMax_ = 0.0;
double qHeadwaterTurbidityGDT::groupYMin_ = 0.0;
QPtrList<qHeadwaterTurbidityGDT> * qHeadwaterTurbidityGDT::openInstances_ = new QPtrList<qHeadwaterTurbidityGDT>;
//signal_null qHeadwaterTurbidityGDT::evScaleChanged_;

void qHeadwaterTurbidityGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qHeadwaterTurbidityGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qHeadwaterTurbidityGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qHeadwaterTurbidityGDT::getGroupYMax( void ) {
	return groupYMax_;
}
void qHeadwaterTurbidityGDT::cbGroupYScaleChanged(){

	QPtrListIterator<qHeadwaterTurbidityGDT>* itr = new QPtrListIterator<qHeadwaterTurbidityGDT>(*qHeadwaterTurbidityGDT::openInstances_);
	qHeadwaterTurbidityGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}
/*
signal_null *qHeadwaterTurbidityGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/

/* *********** */

/* Flow / Reservoirs submenu item. */

qBasinVolumeGDT::qBasinVolumeGDT( const char *id ) :
		qEditGraphDataToken( "Basin Volume for", id, GDTT_DAM_BasinVolume,
			                  DAM, &qIdcegBasinVolume_ )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON );
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.count() < DAYS_IN_SEASON )
		dataArray_.append( 0.0 );
	insertLabel("Volume in Kiloacre-feet");
	setHelpPage( 45 );
	openInstances_->append(this);
	Fill( id );
}

qBasinVolumeGDT::~qBasinVolumeGDT() {
	openInstances_->remove(this);
}

/* Called when UI wants to update basin volume somehow. */
void qBasinVolumeGDT::Apply( void ) {
	qEditGraphDataToken::Apply();
	qIdcegEditDamFlow_.invalidate();
}

void qBasinVolumeGDT::ApplyAll( void ) {
	qEditGraphDataToken::ApplyAll();
	qIdcegEditDamFlow_.invalidate();
}


void qBasinVolumeGDT::Reset( void ) {
	qEditGraphDataToken::Reset();
	qIdcegEditDamFlow_.calcFlow( sourceStr_.ascii() );
}


void qBasinVolumeGDT::ResetAll( void ) {
	qEditGraphDataToken::ResetAll();
	qIdcegEditDamFlow_.calcFlow();
}

/*
 * Called when UI wants to change volume in a storage basin. Base class
 *  action copies dataArray_ to the underlying qIdc_. This routine first
 *  makes sure that the desired volume is within the min,max bounds and
 *  furthermore that the volume doesn't increase by more than the headwater
 *  flow allows.
 */
void qBasinVolumeGDT::SetData( void ) {

	/* First call the base class with the updated values. */
	idc_->setLock(0);  // 0 = locked
	qEditGraphDataToken::SetData();   //submit new data to idc

	/* Then make every day's volume be within limits. */
	qIdcegBasinVolume_.makeLegal( sourceStr_.ascii() );  //emits invalidate  (if changed)

	/* Recalculate the flow which must result from these volumes */
	qIdcegEditDamFlow_.calcFlow( sourceStr_.ascii() );  //emits invalidate
	idc_->setLock(1);  // unlock
	idc_->invalidate(); //now call invalidate
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qBasinVolumeGDT::groupYMax_ = 0.0;
double qBasinVolumeGDT::groupYMin_ = 0.0;
QPtrList<qBasinVolumeGDT> * qBasinVolumeGDT::openInstances_ = new QPtrList<qBasinVolumeGDT>;
//signal_null qBasinVolumeGDT::evScaleChanged_;

void qBasinVolumeGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qBasinVolumeGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qBasinVolumeGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qBasinVolumeGDT::getGroupYMax( void ) {
	return groupYMax_;
}
void qBasinVolumeGDT::cbGroupYScaleChanged(){

	QPtrListIterator<qBasinVolumeGDT>* itr = new QPtrListIterator<qBasinVolumeGDT>(*qBasinVolumeGDT::openInstances_);
	qBasinVolumeGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}
/*
signal_null *qBasinVolumeGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/
/* *************** */

/* Hidden type for "editing" flows at dams with storage basins. Actually
 *  this is done for UI purposes only and does not directly influence
 *  model results.
 */
qEditDamFlowGDT::qEditDamFlowGDT( const char *id ) :
		qEditGraphDataToken( "Outflow for", id, GDTT_DAM_EditFlow,
			                  DAM, &qIdcegEditDamFlow_ )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON );
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.count() < DAYS_IN_SEASON )
		dataArray_.append( 0.0 );
	insertLabel("Outflow in kcfs");
	setHelpPage( 45 );
	openInstances_->append(this);
	Fill( id );
}

qEditDamFlowGDT::~qEditDamFlowGDT() {
	openInstances_->remove(this);
}

/* Called when UI wants to update dam outflow somehow. */
void qEditDamFlowGDT::Apply( void ) {
	qDatContainer *dat = qIdcegBasinVolume_.GetData( sourceStr_.ascii() );
	qIdcegBasinVolume_.ApplyData( dat );
	qIdcegEditDamFlow_.invalidate();
	qIdcegBasinVolume_.invalidate();
}

void qEditDamFlowGDT::ApplyAll( void ) {
	qIdcegBasinVolume_.ApplyCache();
	qIdcegEditDamFlow_.invalidate();
	qIdcegBasinVolume_.invalidate();
}


void qEditDamFlowGDT::Reset( void ) {
	qDatContainer *dat = qIdcegBasinVolume_.GetData( sourceStr_.ascii() );
	qIdcegBasinVolume_.ClearCache( dat );
	qIdcegEditDamFlow_.calcFlow( sourceStr_.ascii() );
}


void qEditDamFlowGDT::ResetAll( void ) {
	qIdcegBasinVolume_.ClearCache();
	qIdcegEditDamFlow_.calcFlow();
}

/*
 * Called when UI wants to change flow out of a storage basin. Base class
 *  action copies dataArray_ to the underlying qIdc_. This routine coerces
 *  the volume based on the desired flow, then updates the flow based on
 *  the actually obtained volumes.
 */
void qEditDamFlowGDT::SetData( void ) {

	/* Then call the base class with the updated values. */
	//lock the idc to prevent infinite loop witht he storage basin
	idc_->setLock(0);  // 0 = locked
	qEditGraphDataToken::SetData();    //submits the data to the IDC

	/* Recalculate the volume which must result from these flows */
	qIdcegEditDamFlow_.calcVolume( sourceStr_.ascii() );    //doesn't emit invalidate
	qIdcegBasinVolume_.invalidate();

	/* Recalculate the flow which must result from the volumes. */
	qIdcegEditDamFlow_.calcFlow( sourceStr_.ascii() ); //emits invalidate
	idc_->setLock(1);  // unlock
	//idc_->invalidate();
}

/* The outflow graph is dirty if the corresponding basin volume or
 *   headwater flow graphs are dirty.
 */
int qEditDamFlowGDT::isDirty( void ) {
	river_segment *seg 
		= getRiverSegmentHandle( sourceStr_.ascii(), DAM_SEG );
	if (seg == NULL || seg->current == NULL)
		return 1;
	if (seg->current->dam.storage_basin == NULL)
		return 1;
	qDatContainer *volume = qIdcegBasinVolume_.GetData( sourceStr_.ascii() );
	if (volume == NULL)
		return 1;
	qDatContainer *inflow = qIdcegHeadwaterFlow_.GetData( seg->up->seg_name );
	if (inflow == NULL)
		return 1;

	return volume->isDirty() || inflow->isDirty();

}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qEditDamFlowGDT::groupYMax_ = 0.0;
double qEditDamFlowGDT::groupYMin_ = 0.0;
QPtrList<qEditDamFlowGDT> * qEditDamFlowGDT::openInstances_ = new QPtrList<qEditDamFlowGDT>;
//signal_null qEditDamFlowGDT::evScaleChanged_;

void qEditDamFlowGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qEditDamFlowGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qEditDamFlowGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qEditDamFlowGDT::getGroupYMax( void ) {
	return groupYMax_;
}
void qEditDamFlowGDT::cbGroupYScaleChanged(){

	QPtrListIterator<qEditDamFlowGDT>* itr = new QPtrListIterator<qEditDamFlowGDT>(*qEditDamFlowGDT::openInstances_);
	qEditDamFlowGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}
/*
signal_null *qEditDamFlowGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/
/* ***************** */

/* Flow / Loss submenu item, get Reach Loss class.
 */
qFlowLossGDT::qFlowLossGDT( const char *id ) :
		qEditGraphDataToken( "Loss for", id, GDTT_REACH_Loss,
			                  REACH, &qIdcegFlowLoss_, GR_BOTH )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.count() < DAYS_IN_SEASON)
		dataArray_.append( 0.0 );
	insertLabel("Loss Rate (kcfs)");
	setHelpPage( 33 );
	openInstances_->append(this);
	Fill( id );
}

qFlowLossGDT::~qFlowLossGDT() {
	openInstances_->remove(this);

}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qFlowLossGDT::groupYMax_ = 0.0;
double qFlowLossGDT::groupYMin_ = 0.0;
QPtrList<qFlowLossGDT> * qFlowLossGDT::openInstances_ = new QPtrList<qFlowLossGDT>;
//signal_null qFlowLossGDT::evScaleChanged_;

void qFlowLossGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qFlowLossGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qFlowLossGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qFlowLossGDT::getGroupYMax( void ) {
	return groupYMax_;
}
void qFlowLossGDT::cbGroupYScaleChanged(){

	QPtrListIterator<qFlowLossGDT>* itr = new QPtrListIterator<qFlowLossGDT>(*qFlowLossGDT::openInstances_);
	qFlowLossGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}
/*
signal_null *qFlowLossGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/
/* **************** */

/* Flow / Elevation Change submenu item, get Elevation Change class.
 */
qElevationChangeGDT::qElevationChangeGDT( const char *id ) :
		qEditGraphDataToken( "Elevation Change for", id, GDTT_REACH_ElevationChange,
			                  REACH, &qIdcegElevationChange_, GR_NEGATIVE )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.count() < DAYS_IN_SEASON)
		dataArray_.append( 0.0 );
	insertLabel("Elevation Change (ft)");
	setHelpPage( 32 );
	openInstances_->append(this);
	Fill( id );
}

qElevationChangeGDT::~qElevationChangeGDT() {
	openInstances_->remove(this);
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qElevationChangeGDT::groupYMax_ = 0.0;
double qElevationChangeGDT::groupYMin_ = 0.0;
QPtrList<qElevationChangeGDT> * qElevationChangeGDT::openInstances_ = new QPtrList<qElevationChangeGDT>;
//signal_null qElevationChangeGDT::evScaleChanged_;

void qElevationChangeGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qElevationChangeGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qElevationChangeGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qElevationChangeGDT::getGroupYMax( void ) {
	return groupYMax_;
}
void qElevationChangeGDT::cbGroupYScaleChanged(){

	QPtrListIterator<qElevationChangeGDT>* itr = new QPtrListIterator<qElevationChangeGDT>(*qElevationChangeGDT::openInstances_);
	qElevationChangeGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

//signal_null *qElevationChangeGDT::evScaleChanged( void ) {
//	return &evScaleChanged_;
//}

