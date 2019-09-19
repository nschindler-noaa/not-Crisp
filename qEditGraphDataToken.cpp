/* qEditGraphDataToken.cpp
 *  This is a class derived from qGraphDataToken. This is a base class
 *  for further derivations which will support user-editable graphs.
 *  This class provides an invalidation event, a dirty-flag mechanism
 *  for either updating or not updating the underlying IDC, and Apply
 *  and ApplyAll methods for causing the underlying IDC to update the
 *  underlying data base.
 *
 * All qEditGraphDataTokens are assumed to contain a single line of
 *  DAYS_IN_SEASON data points.
 */

#include "qEditGraphDataToken.h"
#include "log.h"

qEditGraphDataToken::qEditGraphDataToken(const char *graphTitle, 
					 const char *sourceStr, 
		             int graphType, int evTrigger, qIdcEditGraphBase *idc,
					 int graphDataRange) :
	    qGraphDataToken( graphTitle, sourceStr, graphType,
			evTrigger, graphDataRange ) {
	idc_ = idc;
	dirty_ = 0;
	isEdit_ = 1;
	RegisterEvents();
}

qEditGraphDataToken::~qEditGraphDataToken( ) {
	DetachEvents();
}

/* Connect to underlying IDC's "invalidate" event. */
void qEditGraphDataToken::RegisterEvents( void ) {
	if (idc_ != NULL)
		connect( idc_, SIGNAL(evInvalidate_()), this, SLOT(cbInvalidate()));
	}

void qEditGraphDataToken::DetachEvents( void ) {
   if (idc_ != NULL)
	   disconnect( idc_, SIGNAL(evInvalidate_()), this, SLOT(cbInvalidate()));
	
}

/* Receive signal from underlying IDC that it has been invalidated; get
 *  updated values for this GDT; pass that signal on to anyone who 
 *  is interested.
 */
void qEditGraphDataToken::cbInvalidate(void){
	/* Refresh the data we are interested in. Tell our graph dialog to
	 *  repaint itself (really, tell anyone who is interested, that we
	 *  have changed. */
	dirty_ = 0;
	Fill( sourceStr_.ascii() );
	emit evInvalidate_(); //repaints graph
}
/*
void qEditGraphDataToken::_cbInvalidate(void *msg){
    ZatEditGraphDataToken *zeg = (ZatEditGraphDataToken *)msg;
    zeg->cbInvalidate();
}
*/
/* Actions taken on account of user pushing buttons in dialog. */
void qEditGraphDataToken::Apply( void ) {

	qDatContainer *dc = idc_->GetData( sourceStr_.ascii() );
	if (dc == NULL)
		return;
	idc_->ApplyData( dc );
	idc_->invalidate();
}

void qEditGraphDataToken::ApplyAll( void ) {
	idc_->ApplyCache();
	idc_->invalidate();
}

void qEditGraphDataToken::Reset( void ) {
	/* Get data container (idc cache element) we are pointing to now. */
	qDatContainer *dc = idc_->GetData( sourceStr_.ascii() );
	if (dc == 0)
		return;

	/* Blast that set of values out of the cache and notify appropriately.
	 *  NOTE this object will be notified and will re-fill.
	 */
	idc_->ClearCache( dc );
}

void qEditGraphDataToken::ResetAll( void ) {
	/* Clear all cached data for this type of data (this underlying IDC).
	 *  We will receive a notification callback and redisplay the data
	 *  we are interested in. */
	idc_->ClearCache();
}

/* Virtual method to fill a GraphDataToken. NOTE that editable graphs
 *  probably won't be using more than one string to describe the data
 *  to be edited. */
int qEditGraphDataToken::Fill( const char *feature,  const char *species) {
	int i;
	qDatContainer *dc;


	/* First put old data back if dirty. */
	if (dirty_)
		SetData();

	/* Common actions taken by base class. */
	qGraphDataToken::Fill( feature, species );
	//sets SourceStr_ = features

	/* Get data from underlying IDC. */
	dc = idc_->GetData( sourceStr_.ascii() );

	if (dc == NULL)
		return 0;

	/* Copy data to DataToken (this). */
	for (i = 0; i < numElementsPerLine_; i++)
		dataArray_[i] = dc->getDouble(i);

	if (allowAutoscale_)
		setYBounds( 1 );

	/* Notify that we have re-filled. */
	emit evFilled_();

	return 1;
}

void qEditGraphDataToken::SetData( void ) {
	int i;
	qDatContainer *dc = idc_->GetData( sourceStr_.ascii() );
	if (dc == NULL)
		return;

	for (i = 0; i < numElementsPerLine_; i++){
		dc->setDouble( i, dataArray_[i] );	
	}
	dirty_ = 0;
	
	idc_->invalidate();
}

/* Try to be a little more clever with Save As... */
void qEditGraphDataToken::writeHeader( FILE **fpp ) {
	unsigned i;

	fprintf( *fpp, "%s\n", getGraphTitle().ascii() );
	for (i = 0; i < legendArray_.count(); i++)
		fprintf( *fpp, "%s\n", legendArray_[i].ascii() );
}

int qEditGraphDataToken::isDirty( void ) {
	qDatContainer *dc = idc_->GetData( sourceStr_.ascii() );
	if (dc == NULL)
		return 0;
	return dc->isDirty();
}

