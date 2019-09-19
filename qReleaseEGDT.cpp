/* qReleaseEGDT.cpp
 *  This is a class derived from qEditGraphDataToken. This class
 *  uses the local list of "struct release" objects maintained by the
 *  qReleaseTool as the underlying data structure. This class redefines
 *  the qEditGraphDataToken virtual functions which access the underlying
 *  data structure.
 *
 * All qEditGraphDataTokens are assumed to contain a single line of
 *  DAYS_IN_SEASON data points.
 */

#include "qReleaseEGDT.h"
#include "release.h"
#include "qReleaseTool.h"
#include "log.h"

qReleaseEGDT::qReleaseEGDT(qReleaseTool *tool, const char *graphTitle) :
	    qEditGraphDataToken( graphTitle, "", 0,
			-1, NULL, GR_POSITIVE ) {

	relTool_ = tool;

	setNumLines(1);
	setNumElementsPerLine(DAYS_IN_SEASON);
	dataArray_.resize( DAYS_IN_SEASON );
	while (dataArray_.count() < DAYS_IN_SEASON)
		dataArray_.append( 0.0 );
	insertLabel("Fish Released");
	setHelpPage( 40 );

	Fill( );

	RegisterEvents();
}

qReleaseEGDT::~qReleaseEGDT( ) {
	DetachEvents();
}

/* Connect to underlying IDC's "invalidate" event. */
void qReleaseEGDT::RegisterEvents( void ) {
	connect(relTool_, SIGNAL(evInvalidate()), this, SLOT(cbInvalidate()));
   //relTool_->evInvalidate()->attach(this, &qReleaseEGDT::_cbInvalidate);
}

void qReleaseEGDT::DetachEvents( void ) {
   //relTool_->evInvalidate()->detach(this, &qReleaseEGDT::_cbInvalidate);
}

/* Actions taken on account of user pushing buttons in dialog. */
void qReleaseEGDT::Apply( void ) {
	relTool_->cbApply(  );
}

void qReleaseEGDT::ApplyAll( void ) {
	relTool_->cbApplyAll(  );
}

void qReleaseEGDT::Reset( void ) {
	relTool_->cbReset( );
}

void qReleaseEGDT::ResetAll( void ) {
	relTool_->cbResetAll( );
}

int qReleaseEGDT::isWritable( void ) {
	return relTool_->currentStruct() != NULL;
}

/* Virtual method to fill a GraphDataToken. NOTE that editable graphs
 *  probably won't be using more than one string to describe the data
 *  to be edited. */
int qReleaseEGDT::Fill( const char *feature,  const char *species) {
	int i, ii, j;
	struct release *rel;

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );

	/* Get data from underlying release structure. */
	rel = relTool_->currentStruct();

	/* Copy data to DataToken (this). */
	for (i = 0; i < numElementsPerLine_; i++)
		dataArray_[i] = 0.0;
	if (rel != NULL)
		for (i = 0, ii = rel->start_date; 
		     ii < numElementsPerLine_ && i < DAYS_IN_RUN;
			 i++, ii++) {
			for (j = 0; j < STEPS_PER_DAY; j++)
				dataArray_[ii] += rel->number[i*STEPS_PER_DAY + j];
		}

	setYBounds( 1 );

	/* Notify that we have re-filled. */
	emit evFilled_();
	//evFilled_.notify();

	return 1;
}

void qReleaseEGDT::SetData( void ) {
	struct release *rel;
	int i, ii, j;
	rel = relTool_->currentStruct();
	if (rel != NULL) {
		for (i = 0, ii = rel->start_date; 
		     ii < numElementsPerLine_ && i < DAYS_IN_RUN;
			 i++, ii++) {
			/* NOTE underlying engine saves all released fish in the 
			 *  first time period of each day. 
			 */
			rel->number[i*STEPS_PER_DAY] = dataArray_[ii];
			//log_msg(0,(char*)QString::number(dataArray_[ii]).ascii());
			for (j = 1; j < STEPS_PER_DAY; j++)
				rel->number[i*STEPS_PER_DAY + j] = 0;
		}
		rel->dirty_flag = 1;
		relTool_->emitInvalidate();
		//relTool_->evInvalidate()->notify();
	}
}

int qReleaseEGDT::isDirty( void ) {
	struct release *rel;
	rel = relTool_->currentStruct();
	if (rel != NULL)
		return rel->dirty_flag;
	else
		return 0;
}

void qReleaseEGDT::SetDirty( void ) {
	struct release *rel;
	rel = relTool_->currentStruct();
	if (rel != NULL)
		rel->dirty_flag = 1;
}

