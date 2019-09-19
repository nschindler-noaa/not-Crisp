//#include <zapp.hpp>
//#include <intpack/zip.hpp>

#include "qGraphSpeciesMediator.h"
#include "qGraphDialog.h"
#include "qGraphDataTokenBuilder.h"
#include "crisp.h"
#include <qstring.h>

/* This is a derived class which uses the ZatGraphDialog portable class.
 *  The base class's combobox is exposed, and filled with a list
 *  of species plus "All". Update events from the combobox indicate
 *  a change of species selection. Update events from the map mediator
 *  must be handled with the current species selection preserved.
 */

qGraphSpeciesMediator::qGraphSpeciesMediator(qGraphDialog *gd, qGraphDataToken *dt)
			 : qGraphMediator( gd, dt ) {
	int i;
	QString str;
	
	/* Add species to combobox, followed by "All" (which is default selection).
	 *  "add" inserts in sorted order by default, so after that we have
	 *  to search for the default selection. */

	for (i = 0; i < num_specs; i++)
		_gd->addComboBoxText( spec_names[i] );
	_gd->addComboBoxText( "All" );	

	/* Get "All" selected. */
	for (i = 0; i < num_specs+1; i++) {
		_gd->getComboBoxText( str, i );
		if (str.contains( "All", false ) == 1) {
			_gd->selectComboBox( i );
			break;
		}
	}

	/* Now it is safe to receive events from the combobox. */
	RegisterEvents();

}
 
qGraphSpeciesMediator::~qGraphSpeciesMediator(){
	DetachEvents();
}

/* Register and detach events connect us with the combobox change
 *  notification events of the ZatDialogBox, as well as all the other
 *  events for the base class.
 */
void
qGraphSpeciesMediator::RegisterEvents(void){
   //_gd->evCombobox()->attach(this, &qGraphSpeciesMediator::_cbCombobox);
	connect(_gd, SIGNAL(evCombobox_()), this, SLOT(speciesCombobox()));
}

void
qGraphSpeciesMediator::DetachEvents(void){
    //_gd->evCombobox()->detach(this, &qGraphSpeciesMediator::_cbCombobox);
	disconnect(_gd, SIGNAL(evCombobox_()), this, SLOT(speciesCombobox()));
}

/* Callback event for the combobox in the graph dialog. Tell data token
 *  to refill itself with data for the current feature and newly selected
 *  species. */

void
qGraphSpeciesMediator::speciesCombobox(){
	QString species;
	_gd->getComboBoxText( species );
    if( dataToken_ ){
		if (dataToken_->getExtraStr().compare( species )) {
			dataToken_->Fill( NULL, species.ascii() );
			_gd->setTitleLabel(dataToken_->getGraphTitle().ascii());	
			_gd->setCaption(dataToken_->getGraphTitle());
			//_gp->size(0);
			_gp->repaint();
			_gd->scaleUpdated();
		}
	}
}
/*
void
qGraphSpeciesMediator::_cbCombobox(void *msg){
    qGraphSpeciesMediator *zcg = (qGraphSpeciesMediator *)msg;
    zcg->cbCombobox();
}
*/

/* The graph update event must take the combobox state into account. */
void
qGraphSpeciesMediator::cbGraphUpdate(const char *sourceName, const int type){
	QString species;
	_gd->getComboBoxText( species );
    if(_gd->isActive() && dataToken_ && (dataToken_->getEventTrigger() == type)){
		if(dataToken_->getSourceStr().compare( sourceName )) {
			dataToken_->Fill( sourceName, NULL );
			_gd->scaleUpdated();
			_gd->setTitleLabel(dataToken_->getGraphTitle().ascii());	
			_gd->setCaption(dataToken_->getGraphTitle());
			//_gp->size(0);
			_gp->repaint();
		}
	}
}

