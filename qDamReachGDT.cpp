/* qDamReachGDT.cpp
 *  This file contains several classes derived from ZatGraphDataToken. These
 *   classes know how to fill themselves with data of a certain type. They are
 *   sharing a file because each class is small and there are a lot of classes.
 */

#include "qDamReachGDT.h"
#include "qGraphDataTokenTypes.h"
#include "mapConstants.h"
#include "qUtils.h"
#include "log.h"

/* ******************************************************* */

qDamFlowGDT::qDamFlowGDT( const char *sourceStr ):
          qGraphDataToken( "Flow for", sourceStr, GDTT_DAM_Flow, DAM  )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.size() < DAYS_IN_SEASON)
		dataArray_.push_back( 0.0 );
	insertLabel("Flow Rate (kcfs)");

	Fill( sourceStr );
	setHelpPage( 13 );
	openInstances_->append(this);
}

qDamFlowGDT::~qDamFlowGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam flow data for a given dam. */
int qDamFlowGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, DAM_SEG);	
	if(riverSeg && riverSeg->current->dam.flow){
		for (i = 0; i < DAYS_IN_SEASON; i++)
			dataArray_[i] = isRunning() ? 0.0 : riverSeg->current->dam.flow[i];
		setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qDamFlowGDT::groupYMax_ = 0.0;
double qDamFlowGDT::groupYMin_ = 0.0;


void qDamFlowGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qDamFlowGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qDamFlowGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qDamFlowGDT::getGroupYMax( void ) {
	return groupYMax_;
}


void qDamFlowGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qDamFlowGDT>* itr = new QPtrListIterator<qDamFlowGDT>(*qDamFlowGDT::openInstances_);
	qDamFlowGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		++(*itr);
	}
	delete itr;

}

QPtrList<qDamFlowGDT> * qDamFlowGDT::openInstances_ = new QPtrList<qDamFlowGDT>;


/* ******************************************************* */

qReachFlowGDT::qReachFlowGDT( const char *sourceStr ):
          qGraphDataToken( "Flow for", sourceStr, GDTT_REACH_Flow, REACH  )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.size() < DAYS_IN_SEASON)
		dataArray_.push_back( 0.0 );

	insertLabel("Flow Rate (kcfs)");

	Fill( sourceStr );
	setHelpPage( 14 );
	openInstances_->append(this);
}

qReachFlowGDT::~qReachFlowGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam flow data for a given dam. */
int qReachFlowGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, REACH_SEG);	
	if(riverSeg && riverSeg->current->reach.flow){
		for (i = 0; i < DAYS_IN_SEASON; i++)
			dataArray_[i] = isRunning() ? 0.0 : riverSeg->current->reach.flow[i];
		setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change. Dam flows and reach flows are in the same y-scale group.
 */
void qReachFlowGDT::setGroupYMin( double d ) {
    qDamFlowGDT::groupYMin_ = d;
}

void qReachFlowGDT::setGroupYMax( double d ) {
	qDamFlowGDT::groupYMax_ = d;
}

double qReachFlowGDT::getGroupYMin( void ) {
	return qDamFlowGDT::groupYMin_;
}

double qReachFlowGDT::getGroupYMax( void ) {
	return qDamFlowGDT::groupYMax_;
}


void qReachFlowGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qReachFlowGDT>* itr = new QPtrListIterator<qReachFlowGDT>(*qReachFlowGDT::openInstances_);
	qReachFlowGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();	
		++(*itr);
	}
	delete itr;

}

QPtrList<qReachFlowGDT> * qReachFlowGDT::openInstances_ = new QPtrList<qReachFlowGDT>;


/* ******************************************************* */

qDamNSatGDT::qDamNSatGDT( const char *sourceStr ):
          qGraphDataToken( "TDG Saturation > 100% Output for", 
                                sourceStr, GDTT_DAM_NSat, DAM  )
{
	setNumLines(2);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON*2 );
	while (dataArray_.size() < DAYS_IN_SEASON*2)
		dataArray_.push_back( 0.0 );
    insertLabel("Powerhouse side");
	insertLabel("Spill side");

	Fill( sourceStr );
	setHelpPage( 15 );
	openInstances_->append(this);
}

qDamNSatGDT::~qDamNSatGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam nsat data for a given dam. */
int qDamNSatGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i, j, k;
    double sum;
    FlowLocation floc;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, DAM_SEG);	
	if(riverSeg && riverSeg->current->dam.gas_out){
        if (isRunning()) {
            for (i = 0; i < DAYS_IN_SEASON*2; i++)
                dataArray_[i] = 0.0;
        }
        else {
            /* Nitrogen supersaturation is kept for the right and left sides
             *  of the dam, and for each dam time slice of each day in the
             *  season. It is presented as an averaged value for each day.
             *  It is presented as the nitrogen for the powerhouse side and
             *  for the spillway side, for each dam individually.
             */
            floc = riverSeg->current->dam.spill_side;
            floc = FLOW_OTHER_SIDE( floc );  // powerhouse first
            for (i = 0; i < DAYS_IN_SEASON; i++) {
                sum = 0.0;
                for (k = 0; k < DAM_TIME_SLC; k++)
                    sum += riverSeg->current->dam.gas_out->gas[floc]
                                [i*DAM_TIME_SLC + k];
                dataArray_[i] = sum / DAM_TIME_SLC;
            }
            floc = FLOW_OTHER_SIDE( floc );  // then spill side
            for (j = 0 ; j < DAYS_IN_SEASON; i++, j++) {
                sum = 0.0;
                for (k = 0; k < DAM_TIME_SLC; k++)
                    sum += riverSeg->current->dam.gas_out->gas[floc]
                                [j*DAM_TIME_SLC + k];
                dataArray_[i] = sum / DAM_TIME_SLC;
            }
        }
        setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qDamNSatGDT::groupYMax_ = 0.0;
double qDamNSatGDT::groupYMin_ = 0.0;
//signal_null ZasDamNSatGDT::evScaleChanged_;

void qDamNSatGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qDamNSatGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qDamNSatGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qDamNSatGDT::getGroupYMax( void ) {
	return groupYMax_;
}



void qDamNSatGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qDamNSatGDT>* itr = new QPtrListIterator<qDamNSatGDT>(*qDamNSatGDT::openInstances_);
	qDamNSatGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		++(*itr);
	}
	delete itr;

}

QPtrList<qDamNSatGDT> * qDamNSatGDT::openInstances_ = new QPtrList<qDamNSatGDT>;


/* ******************************************************* */

qDamSpillGDT::qDamSpillGDT( const char *sourceStr ):
          qGraphDataToken( "Spill fraction for", sourceStr, GDTT_DAM_Spill, DAM  )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.size() < DAYS_IN_SEASON)
		dataArray_.push_back( 0.0 );
	insertLabel("Spill fraction");

	Fill( sourceStr );
	setHelpPage( 16 );
	openInstances_->append(this);
}

qDamSpillGDT::~qDamSpillGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam flow data for a given dam. */
int qDamSpillGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i, j;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, DAM_SEG);	
	if(riverSeg && riverSeg->current->dam.spill){
		for (i = 0; i < DAYS_IN_SEASON; i++) {
			dataArray_[i] = 0.0;
			if (!isRunning()) {
				for (j = 0; j < DAM_TIME_SLC; j++)
					dataArray_[i] 
						+= riverSeg->current->dam.spill[i * DAM_TIME_SLC + j];
				dataArray_[i] /= DAM_TIME_SLC;
			}
		}
		setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qDamSpillGDT::groupYMax_ = 0.0;
double qDamSpillGDT::groupYMin_ = 0.0;
//signal_null ZasDamSpillGDT::evScaleChanged_;

void qDamSpillGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qDamSpillGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qDamSpillGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qDamSpillGDT::getGroupYMax( void ) {
	return groupYMax_;
}

void qDamSpillGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qDamSpillGDT>* itr = new QPtrListIterator<qDamSpillGDT>(*qDamSpillGDT::openInstances_);
	qDamSpillGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		++(*itr);
	}
	delete itr;

}

QPtrList<qDamSpillGDT> * qDamSpillGDT::openInstances_ = new QPtrList<qDamSpillGDT>;


/* ******************************************************* */

qReachNSatGDT::qReachNSatGDT( const char *sourceStr ):
          qGraphDataToken( "TDG Saturation > 100% Output for", 
                                sourceStr, GDTT_REACH_NSatOut, REACH  )
{
	setNumLines(2);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON*2 );
	while (dataArray_.size() < DAYS_IN_SEASON*2)
		dataArray_.push_back( 0.0 );
	insertLabel("Left");
    insertLabel("Right");

	Fill( sourceStr );
	setHelpPage( 17 );
	openInstances_->append(this);
}

qReachNSatGDT::~qReachNSatGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam flow data for a given dam. */
int qReachNSatGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i, j, k;
    double sum;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, REACH_SEG);	
	if(riverSeg && riverSeg->current->reach.gas_out){
        if (isRunning()) {
            for (i = 0; i < DAYS_IN_SEASON*2; i++)
                dataArray_[i] = 0.0;
        }
        else {
            /* Nitrogen supersaturation is kept for the right and left sides
             *  of the reach, and for each dam time slice of each day in the
             *  season. It is presented as an averaged value for each day.
             */
            for (i = 0; i < DAYS_IN_SEASON; i++) {
                sum = 0.0;
                for (k = 0; k < DAM_TIME_SLC; k++)
                    sum += riverSeg->current->reach.gas_out->gas[FLOW_LEFT]
                                [i*DAM_TIME_SLC + k];
                dataArray_[i] = sum / DAM_TIME_SLC;
            }
            for (j = 0 ; j < DAYS_IN_SEASON; i++, j++) {
                sum = 0.0;
                for (k = 0; k < DAM_TIME_SLC; k++)
                    sum += riverSeg->current->reach.gas_out->gas[FLOW_RIGHT]
                                [j*DAM_TIME_SLC + k];
                dataArray_[i] = sum / DAM_TIME_SLC;
            }
        }
        setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change. The dam and reach nsat graphs are in the same group. 
 */

void qReachNSatGDT::setGroupYMin( double d ) {
    qDamNSatGDT::groupYMin_ = d;
}

void qReachNSatGDT::setGroupYMax( double d ) {
	qDamNSatGDT::groupYMax_ = d;
}

double qReachNSatGDT::getGroupYMin( void ) {
	return qDamNSatGDT::groupYMin_;
}

double qReachNSatGDT::getGroupYMax( void ) {
	return qDamNSatGDT::groupYMax_;
}

void qReachNSatGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qReachNSatGDT>* itr = new QPtrListIterator<qReachNSatGDT>(*qReachNSatGDT::openInstances_);
	qReachNSatGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		++(*itr);
	}
	delete itr;

}

QPtrList<qReachNSatGDT> * qReachNSatGDT::openInstances_ = new QPtrList<qReachNSatGDT>;


/* ******************************************************* */

qDamWaterTempGDT::qDamWaterTempGDT( const char *sourceStr ):
          qGraphDataToken( "Water Temperature (C) for",
			                  sourceStr, GDTT_DAM_WaterTemp, DAM  )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.size() < DAYS_IN_SEASON)
		dataArray_.push_back( 0.0 );
	insertLabel("Water Temperature (C)");

	Fill( sourceStr );
	setHelpPage( 18 );
	openInstances_->append(this);
}

qDamWaterTempGDT::~qDamWaterTempGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam flow data for a given dam. */
int qDamWaterTempGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i, j;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, DAM_SEG);	
	if(riverSeg && riverSeg->current->dam.water_temp){
		for (i = 0; i < DAYS_IN_SEASON; i++) {
			dataArray_[i] = 0.0;
			if (!isRunning()) {
				for (j = 0; j < STEPS_PER_DAY; j++)
					dataArray_[i] 
						+= riverSeg->current->dam.water_temp[i*STEPS_PER_DAY+j];
				dataArray_[i] /= STEPS_PER_DAY;
			}
		}
		setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qDamWaterTempGDT::groupYMax_ = 0.0;
double qDamWaterTempGDT::groupYMin_ = 0.0;
//signal_null ZasDamWaterTempGDT::evScaleChanged_;

void qDamWaterTempGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qDamWaterTempGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qDamWaterTempGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qDamWaterTempGDT::getGroupYMax( void ) {
	return groupYMax_;
}

void qDamWaterTempGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qDamWaterTempGDT>* itr = new QPtrListIterator<qDamWaterTempGDT>(*qDamWaterTempGDT::openInstances_);
	qDamWaterTempGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		++(*itr);
	}
	delete itr;

}

QPtrList<qDamWaterTempGDT> * qDamWaterTempGDT::openInstances_ = new QPtrList<qDamWaterTempGDT>;

/* ******************************************************* */

qReachWaterTempGDT::qReachWaterTempGDT( const char *sourceStr ):
          qGraphDataToken( "Water Temperature (C) for", 
			                  sourceStr, GDTT_REACH_WaterTemp, REACH  )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.size() < DAYS_IN_SEASON)
		dataArray_.push_back( 0.0 );
	insertLabel("Water Temperature (C)");

	Fill( sourceStr );
	setHelpPage( 19 );
	openInstances_->append(this);
}

qReachWaterTempGDT::~qReachWaterTempGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam flow data for a given dam. */
int qReachWaterTempGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i, j;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, REACH_SEG);	
	if(riverSeg && riverSeg->current->reach.water_temp){
		for (i = 0; i < DAYS_IN_SEASON; i++) {
			dataArray_[i] = 0.0;
			if (!isRunning()) {
				for (j = 0; j < STEPS_PER_DAY; j++)
					dataArray_[i] 
						+= riverSeg->current->reach.water_temp[i*STEPS_PER_DAY+j];
				dataArray_[i] /= STEPS_PER_DAY;
			}
		}
		setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change. Dam and reach water temperatures are in the same scaling
 *  group.
 */

void qReachWaterTempGDT::setGroupYMin( double d ) {
    qDamWaterTempGDT::groupYMin_ = d;
}

void qReachWaterTempGDT::setGroupYMax( double d ) {
	qDamWaterTempGDT::groupYMax_ = d;
}

double qReachWaterTempGDT::getGroupYMin( void ) {
	return qDamWaterTempGDT::groupYMin_;
}

double qReachWaterTempGDT::getGroupYMax( void ) {
	return qDamWaterTempGDT::groupYMax_;
}

void qReachWaterTempGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qReachWaterTempGDT>* itr = new QPtrListIterator<qReachWaterTempGDT>(*qReachWaterTempGDT::openInstances_);
	qReachWaterTempGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		++(*itr);
	}
	delete itr;

}

QPtrList<qReachWaterTempGDT> * qReachWaterTempGDT::openInstances_ = new QPtrList<qReachWaterTempGDT>;


/* ******************************************************* */

qDamTurbidityGDT::qDamTurbidityGDT( const char *sourceStr ):
          qGraphDataToken( "Turbidity for",
			                  sourceStr, GDTT_DAM_Turbidity, DAM  )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.size() < DAYS_IN_SEASON)
		dataArray_.push_back( 0.0 );
	insertLabel("Turbidity");

	Fill( sourceStr );
	setHelpPage( 418 );
	openInstances_->append(this);
}

qDamTurbidityGDT::~qDamTurbidityGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam flow data for a given dam. */
int qDamTurbidityGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i, j;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, DAM_SEG);	
	if(riverSeg && riverSeg->current->dam.turbidity){
		for (i = 0; i < DAYS_IN_SEASON; i++) {
			dataArray_[i] = 0.0;
			if (!isRunning()) {
				for (j = 0; j < STEPS_PER_DAY; j++)
					dataArray_[i] 
						+= riverSeg->current->dam.turbidity[i*STEPS_PER_DAY+j];
				dataArray_[i] /= STEPS_PER_DAY;
			}
		}
		setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qDamTurbidityGDT::groupYMax_ = 0.0;
double qDamTurbidityGDT::groupYMin_ = 0.0;
//signal_null ZasDamTurbidityGDT::evScaleChanged_;

void qDamTurbidityGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qDamTurbidityGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qDamTurbidityGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qDamTurbidityGDT::getGroupYMax( void ) {
	return groupYMax_;
}

//void qDamTurbidityGDT::evScaleChanged( void ) {
//	return &evScaleChanged_;
//}
void qDamTurbidityGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qDamTurbidityGDT>* itr = new QPtrListIterator<qDamTurbidityGDT>(*qDamTurbidityGDT::openInstances_);
	qDamTurbidityGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qDamTurbidityGDT> * qDamTurbidityGDT::openInstances_ = new QPtrList<qDamTurbidityGDT>;


/* ******************************************************* */

qReachTurbidityGDT::qReachTurbidityGDT( const char *sourceStr ):
          qGraphDataToken( "Turbidity for", 
			                  sourceStr, GDTT_REACH_Turbidity, REACH  )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.size() < DAYS_IN_SEASON)
		dataArray_.push_back( 0.0 );
	insertLabel("Turbidity");

	Fill( sourceStr );
	setHelpPage( 419 );
	openInstances_->append(this);
}

qReachTurbidityGDT::~qReachTurbidityGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam flow data for a given dam. */
int qReachTurbidityGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i, j;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, REACH_SEG);	
	if(riverSeg && riverSeg->current->reach.turbidity){
		for (i = 0; i < DAYS_IN_SEASON; i++) {
			dataArray_[i] = 0.0;
			if (!isRunning()) {
				for (j = 0; j < STEPS_PER_DAY; j++)
					dataArray_[i] 
						+= riverSeg->current->reach.turbidity[i*STEPS_PER_DAY+j];
				dataArray_[i] /= STEPS_PER_DAY;
			}
		}
		setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change. Dam and reach water temperatures are in the same scaling
 *  group.
 */

void qReachTurbidityGDT::setGroupYMin( double d ) {
    qDamTurbidityGDT::groupYMin_ = d;
}

void qReachTurbidityGDT::setGroupYMax( double d ) {
	qDamTurbidityGDT::groupYMax_ = d;
}

double qReachTurbidityGDT::getGroupYMin( void ) {
	return qDamTurbidityGDT::groupYMin_;
}

double qReachTurbidityGDT::getGroupYMax( void ) {
	return qDamTurbidityGDT::groupYMax_;
}

void qReachTurbidityGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qReachTurbidityGDT>* itr = new QPtrListIterator<qReachTurbidityGDT>(*qReachTurbidityGDT::openInstances_);
	qReachTurbidityGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		++(*itr);
	}
	delete itr;

}

QPtrList<qReachTurbidityGDT> * qReachTurbidityGDT::openInstances_ = new QPtrList<qReachTurbidityGDT>;


/* ******************************************************* */

qReachVelGDT::qReachVelGDT( const char *sourceStr ):
          qGraphDataToken( "Reach Velocity for", sourceStr, GDTT_REACH_Velocity, REACH  )
{
	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.size() < DAYS_IN_SEASON)
		dataArray_.push_back( 0.0 );
	insertLabel("Velocity (mi/day)");

	Fill( sourceStr );
	setHelpPage( 20 );
	openInstances_->append(this);
}

qReachVelGDT::~qReachVelGDT( )
{
	openInstances_->remove(this);
}

/* This routine gets the dam flow data for a given dam. */
int qReachVelGDT::Fill( const char *feature, const char *species )
{
	struct river_segment *riverSeg;
	int i,j;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	riverSeg = getRiverSegmentHandle(feature, REACH_SEG);	
	if(riverSeg && riverSeg->current->reach.velocity){
		for (i = 0; i < DAYS_IN_SEASON; i++) {
			dataArray_[i] = 0.0;
			if (!isRunning()) {
				for (j = 0; j < STEPS_PER_DAY; j++)
					dataArray_[i]
						+= riverSeg->current->reach.velocity[i*STEPS_PER_DAY+j];
			}
		}
		setYBounds( 1 );
	}
	return 1;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qReachVelGDT::groupYMax_ = 0.0;
double qReachVelGDT::groupYMin_ = 0.0;

void qReachVelGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qReachVelGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qReachVelGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qReachVelGDT::getGroupYMax( void ) {
	return groupYMax_;
}

void qReachVelGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qReachVelGDT>* itr = new QPtrListIterator<qReachVelGDT>(*qReachVelGDT::openInstances_);
	qReachVelGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		++(*itr);
	}
	delete itr;

}

QPtrList<qReachVelGDT> * qReachVelGDT::openInstances_ = new QPtrList<qReachVelGDT>;


