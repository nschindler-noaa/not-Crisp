#ifndef _qGraphSpeciesMediator_H_
#define _qGraphSpeciesMediator_H_

#include "qGraphMediator.h"

/* This is a derived class which uses the ZatGraphDialog portable class.
 *  The base class's combobox is exposed, and filled with a list
 *  of species plus "All". Update events from the combobox indicate
 *  a change of species selection. Update events from the map mediator
 *  must be handled with the current species selection preserved.
 */

class qGraphSpeciesMediator : public qGraphMediator {
	Q_OBJECT

public:
	qGraphSpeciesMediator(qGraphDialog *gd, qGraphDataToken *dt);
	virtual ~qGraphSpeciesMediator();

	// event methods.
	virtual void RegisterEvents(void);
	virtual void DetachEvents(void);
	virtual void cbGraphUpdate(const char *index, const int type);
	void cbCombobox();
//	static void _cbCombobox(void *msg);
protected slots:
	virtual void speciesCombobox();
};

#endif


