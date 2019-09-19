//This class builds most of the data oriented dialogs.  That is, all
//the graphs and dialogs showing data from the engine.
//The function is the makeDialog method which acutally builds the dialog given
//mapObjects which contain the name of the data source (Bonneville Dam, etc).
//The builder then creates the proper dialog, mediator and 
//data token(from the ZatGraphDataTokenBuilder for graphs)

#ifndef _qMediatorBuilder_
#define _qMediatorBuilder_


#include "qDialogMedBase.h" 
#include "qGraphMediator.h"
#include "qGraphDataTokenBuilder.h"
#include "qEditGraphMediator.h"
#include "qEquationDialogBase.h"
#include <qwidget.h>

class QmapObject;

class qMediatorBuilder {

	QWidget *_w;
	long ref_;
	static qMediatorBuilder * _instance;
	qGraphDataTokenBuilder *tokenBuilder_;

  protected:
	//This class is a singleton, so our constructor must be protected
	qMediatorBuilder(QWidget *);

  public:
	~qMediatorBuilder();
	
	
	static qMediatorBuilder* instance(QWidget *w_);

	//ref is needed by the utSmartPtr template for reference counting
	inline long ref(){ return ++ref_; }
	inline long unref(){ return --ref_; }
	
	int makeDialog(const int type, QmapObject *geoObject1, QmapObject *geoObject2);
	int makeDialog(const int type, const char *source);

	/* This is the new way to make mediators - the GraphDataToken is
	 *  actually a member of a derived class which knows how to fill
	 *  itself with data. */
	int makeSpeciesDialog( qGraphDataToken *token );
	int makeGraphDialog( qGraphDataToken *token );
	qGraphDialog * makeEditGraphDialog( qEditGraphDataToken *token,int flags = 0 );

	/* Another way to make mediators: they connect themselves with the
	 *  proper interface data token (a unique global, static object).
	 */
	qEquationDialogBase *makeEquationDialog( 
						const int type, const char *source=NULL );
	int makeTimeBaseDialog( const int type );

};

#endif







