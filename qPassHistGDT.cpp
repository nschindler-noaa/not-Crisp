/* qPassHistGDT.cpp
 * This file defines the derived classes for GraphDataTokens for the various
 *  types of passage histograms. All of them are in the same file to
 *  prevent needless proliferation of files. All are derived from
 *  qGraphDataToken via a common intermediate class qBasePassHistGDT.
 */

#include <stdio.h>
#include "qPassHistGDT.h"
#include "qGraphDataTokenTypes.h"
#include "mapConstants.h"
#include "qUtils.h"
#include "log.h"

/* The common intermediate class just knows how to fill a passage histogram.
 *  The constructor and destructor just pass along values to the base class.
 */

qBasePassHistGDT::qBasePassHistGDT( 
						const char *graphTitle, const char *sourceStr, 
		                int graphType, int evTrigger, int graphDataRange):
			qGraphDataToken( graphTitle, sourceStr, graphType, evTrigger, graphDataRange )
{
	/* Label the data. */
    setOutputType( 1 );
}

qBasePassHistGDT::~qBasePassHistGDT()
{

}

/* This method is called on the base class to re-fill the GraphDataToken,
 *  given new values for the two strings which define where the data
 *  comes from. */
int qBasePassHistGDT::Fill( const char *feature, const char *species )
{
	
	if ((feature != NULL && sourceStr_.compare( feature ))
		|| (species != NULL && extraStr_.compare( species )))
	{
		/* Common actions taken by base class. */
		qGraphDataToken::Fill( feature, species );

		if (feature != NULL)
			sourceStr_ = feature;
		if (species != NULL)
			extraStr_ = species;
		FillHistogram();
	}
	return 1;
}

/* This method iterates through all of the releases in the system, and for
 *  each one which is of a species we are interested in, runs through the
 *  list of release segments to find the one we are interested in. Finally,
 *  it sums up the data for that release in the desired river segment,
 *  offset by the release starting date. Derived classes decide which data
 *  is actually being histogrammed.
 */
void qBasePassHistGDT::FillHistogram( ) {
	int i, j;
    struct release	*release;
    struct rls_seg	*rls_seg;
	float f, releaseTotal = 0, rlsTotal = 0, histTotal = 0;

	/* Initialization. */
	setNumLines(1);
	setNumElementsPerLine( DAYS_IN_SEASON );
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.size() < DAYS_IN_SEASON)
		dataArray_.append( 0.0 );
	for (i = 0; i < DAYS_IN_SEASON; i++)
		dataArray_[i] = 0.0;

	if (isRunning())
		return;
	log_msg(1, "Running through all releases");

	/* Run through all releases for the current run. */
    for (release = run->release_list; release; release = release->next)
    {
		/* Decide if this release is a species we are interested in.
		 *  "extraStr_" may be "", "all", or a species name.
		 * Fall through to data accumulation code unless "speciesName"
		 *  is a species we are not interested in. */
		if (extraStr_.compare( "" ) && 
			(1 != extraStr_.contains( "all",FALSE ))  //has one occurance of "all"
			&& extraStr_.compare( release->species->name ))
				continue;

		releaseTotal += release->total_released;
        rlsTotal = 0;

		/* Run through each release segment for this release. */
		for (rls_seg = release->rls_seg; rls_seg; rls_seg = rls_seg->down )
		{
			/* See if this is the release segment we are interested in. */
			if (!sourceStr_.compare( rls_seg->river_segment->seg_name ))
			{				
				/* Sum the passage histogram for this species in this segment. */
				for (i = 0; i < STEPS_IN_RUN; i++) {
					/* Find destination slot in data token. */
					j = release->start_date + i / STEPS_PER_DAY;

					/* Get source data item and accumulate histogram. */
					f = HistData( rls_seg, i );
					if (j < DAYS_IN_SEASON)    // beware of overrun.
					    dataArray_[j] += f;
					rlsTotal += f;
				}
			}
		}
        histTotal += rlsTotal;
	}

	/* Get YMax for the data array. Forbid Ymin == Ymax. */
	setYBounds( 1 );
    char strBuf[50];
	sprintf( strBuf, "total passage %.0f = %.0f%%", histTotal,
		                           100 * histTotal / releaseTotal );
	graphXAxisLabel_ = strBuf;
}

/* Try to be a little more clever about the headers of the Save As... files.
 */
void qBasePassHistGDT::writeHeader( FILE **fpp )
{
	unsigned i;

	fprintf( *fpp, "%s\n", getGraphTitle().ascii() );
	fprintf( *fpp, "Species: %s\n", extraStr_.ascii() );
	for (i = 0; i < legendArray_.count(); i++)
		fprintf( *fpp, "%s\n", legendArray_[i].ascii() );
}

/* **************** */

/* Each derived class implements a particular type of passage histogram.
 *  The main reason for having derived classes is so that they can know
 *  how to fill themselves with data from the appropriate part of the database.
 */

qDamPassHistGDT::qDamPassHistGDT( const char *sourceStr ):
          qBasePassHistGDT( "Passage for", sourceStr, GDTT_PASSAGE_Dam, DAM  )
{
	insertLabel("Fish leaving dam (per day)");	
	Fill( sourceStr, "all" );
	setHelpPage( 39 );
	openInstances_->append(this);
}

qDamPassHistGDT::~qDamPassHistGDT()
{
	openInstances_->remove(this);
}

float qDamPassHistGDT::HistData( struct rls_seg *rls_seg, int i )
{
	float f = 0.0;

	if (rls_seg->data)
		if (rls_seg->data[0])
			if (rls_seg->data[0]->output)
				f = rls_seg->data[0]->output[i];

	return f;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qDamPassHistGDT::groupYMax_ = 0.0;
double qDamPassHistGDT::groupYMin_ = 0.0;
//signal_null qDamPassHistGDT::evScaleChanged_;

void qDamPassHistGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qDamPassHistGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qDamPassHistGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qDamPassHistGDT::getGroupYMax( void ) {
	return groupYMax_;
}
/*
signal_null *qDamPassHistGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/
void qDamPassHistGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qDamPassHistGDT>* itr = new QPtrListIterator<qDamPassHistGDT>(*qDamPassHistGDT::openInstances_);
	qDamPassHistGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qDamPassHistGDT> * qDamPassHistGDT::openInstances_ = new QPtrList<qDamPassHistGDT>;

/* ************* */

qReachPassHistGDT::qReachPassHistGDT( const char *sourceStr ):
          qBasePassHistGDT( "Passage for", sourceStr, GDTT_PASSAGE_Dam, REACH  )
{
	insertLabel("Fish leaving reach (per day)");	
	Fill( sourceStr, "all" );
	setHelpPage( 39 );
	openInstances_->append(this);
}


qReachPassHistGDT::~qReachPassHistGDT()
{
	openInstances_->remove(this);
}

float qReachPassHistGDT::HistData( struct rls_seg *rls_seg, int i )
{
	float f = 0.0;

	if (rls_seg->data)
		if (rls_seg->data[0])
			if (rls_seg->data[0]->output)
				f = rls_seg->data[0]->output[i];

	return f;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
/* NOTE we share the qDamPassHistGDT static values so that the two
 *  types of graphs share the same scaling group.
 */
void qReachPassHistGDT::setGroupYMin( double d ) {
	qDamPassHistGDT::groupYMin_ = d;
}

void qReachPassHistGDT::setGroupYMax( double d ) {
	qDamPassHistGDT::groupYMax_ = d;
}

double qReachPassHistGDT::getGroupYMin( void ) {
	return qDamPassHistGDT::groupYMin_;
}

double qReachPassHistGDT::getGroupYMax( void ) {
	return qDamPassHistGDT::groupYMax_;
}
/*
signal_null *qReachPassHistGDT::evScaleChanged( void ) {
	return &qDamPassHistGDT::evScaleChanged_;
}
*/
void qReachPassHistGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qReachPassHistGDT>* itr = new QPtrListIterator<qReachPassHistGDT>(*qReachPassHistGDT::openInstances_);
	qReachPassHistGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qReachPassHistGDT> * qReachPassHistGDT::openInstances_ = new QPtrList<qReachPassHistGDT>;

/* ************* */

qReachLengthHistGDT::qReachLengthHistGDT( const char *sourceStr ):
          qBasePassHistGDT( "Length for", sourceStr, GDTT_PASSAGE_Dam, REACH  )
{
	insertLabel("Mean fish length (per day)");	
	Fill( sourceStr, "all" );
	setHelpPage( 98 );
	openInstances_->append(this);
}

qReachLengthHistGDT::~qReachLengthHistGDT()
{
		openInstances_->remove(this);
}

/* This method iterates through all of the releases in the system, and for
 *  each one which is of a species we are interested in, runs through the
 *  list of release segments to find the one we are interested in. Finally,
 *  it sums up the data for that release in the desired river segment,
 *  offset by the release starting date. This must be reimplemented for 
 *  the derived class because we need to average the mean fish lengths
 *  weighted by the number of fish exiting each feature.
 */
void qReachLengthHistGDT::FillHistogram( ) {
	int i, j, k;
    struct release	*release;
    struct rls_seg	*rls_seg;
	double numHere, numAdd, lenHere, lenAdd;

	/* Initialization. */
	setNumLines(1);
	setNumElementsPerLine( DAYS_IN_SEASON );
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.count() < DAYS_IN_SEASON)
		dataArray_.append( 0.0 );
	for (i = 0; i < DAYS_IN_SEASON; i++)
		dataArray_[i] = 0.0;
	while (numberArray_.count() < DAYS_IN_SEASON)
		numberArray_.append( 0.0 );
	for (i = 0; i < DAYS_IN_SEASON; i++)
		numberArray_[i] = 0.0;

	if (isRunning())
		return;

	/* Run through all releases for the current run. */
    for (release = run->release_list; release; release = release->next)
    {
		/* Decide if this release is a species we are interested in.
		 *  "extraStr_" may be "", "all", or a species name.
		 * Fall through to data accumulation code unless "speciesName"
		 *  is a species we are not interested in. */
		if (extraStr_.compare( "" ) && 
			(1 != extraStr_.contains( "all", false))
			&& extraStr_.compare( release->species->name ))
				continue;

		/* Run through each release segment for this release. */
		for (rls_seg = release->rls_seg; rls_seg; rls_seg = rls_seg->down )
		{
    	    if (rls_seg->data == NULL)
                continue;
		    if (rls_seg->data[0] == NULL)
                continue;
            if (rls_seg->data[0]->length == NULL
                                   || rls_seg->data[0]->output == NULL) 
                continue;

			/* See if this is the release segment we are interested in. */
			if (!sourceStr_.compare( rls_seg->river_segment->seg_name ))
			{
				/* Sum the passage histogram for this species in this segment. */
				for (i = 0; i < STEPS_IN_RUN; i += STEPS_PER_DAY) {
					/* Find destination slot in data token. */
					j = release->start_date + i / STEPS_PER_DAY;
                    if (j >= DAYS_IN_SEASON)
                        break;

					/* Get source data item and accumulate histogram. */				
                    numAdd = lenAdd = 0.0;
                    for (k = 0; k < STEPS_PER_DAY; k++) {
                        numAdd += rls_seg->data[0]->output[i+k];
                        lenAdd += rls_seg->data[0]->output[i+k]
                                     * rls_seg->data[0]->length[i+k];
                    }
                    numHere = numberArray_[j];
                    lenHere = dataArray_[j];
                    if (numHere + numAdd > 0.00001) {
					    dataArray_[j] = (lenHere * numHere + lenAdd) 
                                               / (numHere + numAdd);
                        numberArray_[j] = numHere + numAdd;
                    }
				}
			}
		}
	}

	/* Get YMax for the data array. Forbid Ymin == Ymax. */
	setYBounds( 1 );
}

float qReachLengthHistGDT::HistData( struct rls_seg *rls_seg, int i )
{
	float f = 0.0;

    // Special hack to give back a weighted average of the lengths over
    //  a given day.
                if (i % STEPS_PER_DAY == 0) {
                    int j;
                    float t = 0.0;
                    for (j = 0; j < STEPS_PER_DAY; j++) {
				        f += rls_seg->data[0]->length[i+j] *
                                rls_seg->data[0]->output[i+j];
                        t += rls_seg->data[0]->output[i+j];
                    }
                    if (t < 0.00001)
                        f = 0.0;
                    else
                        f /= t;
                }
	return f;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qReachLengthHistGDT::groupYMax_ = 0.0;
double qReachLengthHistGDT::groupYMin_ = 0.0;
//signal_null qReachLengthHistGDT::evScaleChanged_;

void qReachLengthHistGDT::setGroupYMin( double d ) {
	qReachLengthHistGDT::groupYMin_ = d;
}

void qReachLengthHistGDT::setGroupYMax( double d ) {
	qReachLengthHistGDT::groupYMax_ = d;
}

double qReachLengthHistGDT::getGroupYMin( void ) {
	return qReachLengthHistGDT::groupYMin_;
}

double qReachLengthHistGDT::getGroupYMax( void ) {
	return qReachLengthHistGDT::groupYMax_;
}
/*
signal_null *qReachLengthHistGDT::evScaleChanged( void ) {
	return &qReachLengthHistGDT::evScaleChanged_;
}
*/
void qReachLengthHistGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qReachLengthHistGDT>* itr = new QPtrListIterator<qReachLengthHistGDT>(*qReachLengthHistGDT::openInstances_);
	qReachLengthHistGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qReachLengthHistGDT> * qReachLengthHistGDT::openInstances_ = new QPtrList<qReachLengthHistGDT>;

/* ************* */

#ifdef ROUTE_TRACKING

qBypassPassHistGDT::qBypassPassHistGDT( const char *sourceStr ):
          qBasePassHistGDT( "Bypass Passage for", sourceStr, GDTT_PASSAGE_Dam, DAM  )
{
	insertLabel("Fish leaving bypass (per day)");	
	Fill( sourceStr, "all" );
	setHelpPage( 39 );
	openInstances_->append(this);
}

qBypassPassHistGDT::~qBypassPassHistGDT()
{
}

float qBypassPassHistGDT::HistData( struct rls_seg *rls_seg, int i )
{
	float f = 0.0;

	if (rls_seg->data)
		if (rls_seg->data[0])
			if (rls_seg->data[0]->bypass_pass)
				f = rls_seg->data[0]->bypass_pass[i];

	return f;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qBypassPassHistGDT::groupYMax_ = 0.0;
double qBypassPassHistGDT::groupYMin_ = 0.0;
//signal_null qBypassPassHistGDT::evScaleChanged_;

void qBypassPassHistGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qBypassPassHistGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qBypassPassHistGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qBypassPassHistGDT::getGroupYMax( void ) {
	return groupYMax_;
}
/*
signal_null *qBypassPassHistGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/
void qBypassPassHistGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qBypassPassHistGDT>* itr = new QPtrListIterator<qBypassPassHistGDT>(*qBypassPassHistGDT::openInstances_);
	qBypassPassHistGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qBypassPassHistGDT> * qBypassPassHistGDT::openInstances_ = new QPtrList<qBypassPassHistGDT>;

/* ************* */

qSluicewayPassHistGDT::qSluicewayPassHistGDT( const char *sourceStr ):
          qBasePassHistGDT( "Sluiceway Passage for", sourceStr, GDTT_PASSAGE_Dam, DAM  )
{
	insertLabel("Fish leaving sluiceway (per day)");	
	Fill( sourceStr, "all" );
	setHelpPage( 39 /* TODO: select a page */ );
	openInstances_->append(this);
}

qSluicewayPassHistGDT::~qSluicewayPassHistGDT()
{
}

float qSluicewayPassHistGDT::HistData( struct rls_seg *rls_seg, int i )
{
	float f = 0.0;

	if (rls_seg->data)
		if (rls_seg->data[0])
			if (rls_seg->data[0]->sluiceway_pass)
				f = rls_seg->data[0]->sluiceway_pass[i];

	return f;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qSluicewayPassHistGDT::groupYMax_ = 0.0;
double qSluicewayPassHistGDT::groupYMin_ = 0.0;

void qSluicewayPassHistGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qSluicewayPassHistGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qSluicewayPassHistGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qSluicewayPassHistGDT::getGroupYMax( void ) {
	return groupYMax_;
}
void qSluicewayPassHistGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qSluicewayPassHistGDT>* itr = new QPtrListIterator<qSluicewayPassHistGDT>(*qSluicewayPassHistGDT::openInstances_);
	qSluicewayPassHistGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qSluicewayPassHistGDT> * qSluicewayPassHistGDT::openInstances_ = new QPtrList<qSluicewayPassHistGDT>;

/* ************* */

qSpillwayPassHistGDT::qSpillwayPassHistGDT( const char *sourceStr ):
          qBasePassHistGDT( "Spillway Passage for", sourceStr, GDTT_PASSAGE_Dam, DAM  )
{
	insertLabel("Fish leaving spillway (per day)");	
	Fill( sourceStr, "all" );
	setHelpPage( 39 );
	openInstances_->append(this);
}

qSpillwayPassHistGDT::~qSpillwayPassHistGDT()
{
	openInstances_->remove(this);
}

float qSpillwayPassHistGDT::HistData( struct rls_seg *rls_seg, int i )
{
	float f = 0.0;

	if (rls_seg->data)
		if (rls_seg->data[0])
			if (rls_seg->data[0]->spillway_pass)
				f = rls_seg->data[0]->spillway_pass[i];

	return f;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qSpillwayPassHistGDT::groupYMax_ = 0.0;
double qSpillwayPassHistGDT::groupYMin_ = 0.0;
//signal_null qSpillwayPassHistGDT::evScaleChanged_;

void qSpillwayPassHistGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qSpillwayPassHistGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qSpillwayPassHistGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qSpillwayPassHistGDT::getGroupYMax( void ) {
	return groupYMax_;
}
/*
signal_null *qSpillwayPassHistGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/
void qSpillwayPassHistGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qSpillwayPassHistGDT>* itr = new QPtrListIterator<qSpillwayPassHistGDT>(*qSpillwayPassHistGDT::openInstances_);
	qSpillwayPassHistGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qSpillwayPassHistGDT> * qSpillwayPassHistGDT::openInstances_ = new QPtrList<qSpillwayPassHistGDT>;


/* ************* */

qRSWPassHistGDT::qRSWPassHistGDT( const char *sourceStr ):
          qBasePassHistGDT( "RSW Passage for", sourceStr, GDTT_PASSAGE_Dam, DAM  )
{
	insertLabel("Fish leaving RSW (per day)");	
	Fill( sourceStr, "all" );
	setHelpPage( 39 );
	openInstances_->append(this);
}

qRSWPassHistGDT::~qRSWPassHistGDT()
{
	openInstances_->remove(this);
}

float qRSWPassHistGDT::HistData( struct rls_seg *rls_seg, int i )
{
	float f = 0.0;

	if (rls_seg->data)
		if (rls_seg->data[0])
			if (rls_seg->data[0]->rsw_pass)
				f = rls_seg->data[0]->rsw_pass[i];

	return f;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qRSWPassHistGDT::groupYMax_ = 0.0;
double qRSWPassHistGDT::groupYMin_ = 0.0;

void qRSWPassHistGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qRSWPassHistGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qRSWPassHistGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qRSWPassHistGDT::getGroupYMax( void ) {
	return groupYMax_;
}
void qRSWPassHistGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qRSWPassHistGDT>* itr = new QPtrListIterator<qRSWPassHistGDT>(*qRSWPassHistGDT::openInstances_);
	qRSWPassHistGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qRSWPassHistGDT> * qRSWPassHistGDT::openInstances_ = new QPtrList<qRSWPassHistGDT>;

/* ************* */

qTurbinePassHistGDT::qTurbinePassHistGDT( const char *sourceStr ):
          qBasePassHistGDT( "Turbine Passage for", sourceStr, GDTT_PASSAGE_Dam, DAM  )
{
	insertLabel("Fish leaving turbine (per day)");	
	Fill( sourceStr, "all" );
	setHelpPage( 39 );
	openInstances_->append(this);
}

qTurbinePassHistGDT::~qTurbinePassHistGDT()
{
	openInstances_->remove(this);
}

float qTurbinePassHistGDT::HistData( struct rls_seg *rls_seg, int i )
{
	float f = 0.0;

	if (rls_seg->data)
		if (rls_seg->data[0])
			if (rls_seg->data[0]->turbine_pass)
				f = rls_seg->data[0]->turbine_pass[i];

	return f;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qTurbinePassHistGDT::groupYMax_ = 0.0;
double qTurbinePassHistGDT::groupYMin_ = 0.0;
//signal_null qTurbinePassHistGDT::evScaleChanged_;

void qTurbinePassHistGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qTurbinePassHistGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qTurbinePassHistGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qTurbinePassHistGDT::getGroupYMax( void ) {
	return groupYMax_;
}
/*
signal_null *qTurbinePassHistGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/
void qTurbinePassHistGDT::cbGroupYScaleChanged(){
	

	QPtrListIterator<qTurbinePassHistGDT>* itr = new QPtrListIterator<qTurbinePassHistGDT>(*qTurbinePassHistGDT::openInstances_);
	qTurbinePassHistGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qTurbinePassHistGDT> * qTurbinePassHistGDT::openInstances_ = new QPtrList<qTurbinePassHistGDT>;


/* ************* */

qTransportPassHistGDT::qTransportPassHistGDT( const char *sourceStr ):
          qBasePassHistGDT( "Transport Passage for", sourceStr, GDTT_PASSAGE_Dam, DAM  )
{
	insertLabel("Fish leaving transport (per day)");	
	Fill( sourceStr, "all" );
	setHelpPage( 39 );
	openInstances_->append(this);
}

qTransportPassHistGDT::~qTransportPassHistGDT()
{
	openInstances_->remove(this);
}

float qTransportPassHistGDT::HistData( struct rls_seg *rls_seg, int i )
{
	float f = 0.0;

	if (rls_seg->data)
		if (rls_seg->data[0])
			if (rls_seg->data[0]->transport_pass)
				f = rls_seg->data[0]->transport_pass[i];

	return f;
}

/*  These values are used to implement the "grouping" of Y-scale values.
 *  Each derived type of graph data token will maintain a single, static
 *  YMin and YMax, and a single event which is signalled when those 
 *  values change.
 */
double qTransportPassHistGDT::groupYMax_ = 0.0;
double qTransportPassHistGDT::groupYMin_ = 0.0;
//signal_null qTransportPassHistGDT::evScaleChanged_;

void qTransportPassHistGDT::setGroupYMin( double d ) {
	groupYMin_ = d;
}

void qTransportPassHistGDT::setGroupYMax( double d ) {
	groupYMax_ = d;
}

double qTransportPassHistGDT::getGroupYMin( void ) {
	return groupYMin_;
}

double qTransportPassHistGDT::getGroupYMax( void ) {
	return groupYMax_;
}
/*
signal_null *qTransportPassHistGDT::evScaleChanged( void ) {
	return &evScaleChanged_;
}
*/
void qTransportPassHistGDT::cbGroupYScaleChanged(){
	
	log_msg(0,"sub classs called");
	QPtrListIterator<qTransportPassHistGDT>* itr = new QPtrListIterator<qTransportPassHistGDT>(*qTransportPassHistGDT::openInstances_);
	qTransportPassHistGDT * curr;
	while ((curr  = itr->current()) != 0){
		curr->emitgroupScaleUpdated();
		//curr->cbGroupYScaleChanged();
		++(*itr);
	}
	delete itr;

}

QPtrList<qTransportPassHistGDT> * qTransportPassHistGDT::openInstances_ = new QPtrList<qTransportPassHistGDT>;

#endif /* ROUTE_TRACKING */

/* ************* */

