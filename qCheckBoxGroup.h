//qCheckBoxGroup
//
//This class creates a group of multiple check boxes and space for their tags.
//There are functions for turning the checks on and off and an event handler so
//that you can be notified of status changes as needed.  This class is used
//extensively with the ZatTabDialog, and ZatDialogBase classes.

#ifndef _qCheckBoxGroup_h_
#define _qCheckBoxGroup_h_

//#include "implicit.h"
//#include "ZatItem.h"
#include "qDefines.h"
#include <qcheckbox.h>
#include <qwidget.h>
#include <qptrlist.h>
#include <qvaluevector.h>
#include <qlayout.h>
//#include <rw/tvordvec.h>

//typedef utImplicit3<const int, const int, const int> signal_iii;

class qCheckBoxGroup : public QWidget {

	Q_OBJECT

	QCheckBox *incItem;
	QPtrList<QCheckBox> rwList;
	int _numStrings;
	char **_itemStrings;
	int _baseId;
	//signal_iii cbgEvent;
	int _status;
	QHBoxLayout* horizLayout;  //the vertical box layout
	virtual void paintEvent ( QPaintEvent * );

  public:

	qCheckBoxGroup(QWidget *w, int baseId, QValueVector<QString> *stringVec); 
	qCheckBoxGroup(QWidget *w, int baseId, char **itemStrings, int numStrings);
	qCheckBoxGroup(QWidget *w, QRect r, int baseId, char **itemStrings, int numStrings);
	virtual ~qCheckBoxGroup();
	//virtual int command(zCommandEvt *Ev);

	int setItem(int num, bool onOff);
	int checkItem(int num);
	inline int numItems(){return _numStrings;}

	//signal_iii *cbgRegisterEvent(){ return &cbgEvent; }
	int getStatus(){ return _status; }
	int isEnabled(){ return _status; }

	void enable();
	void disable();
	void setText(char **itemStr, int num);
	void showCBG( int num, bool iShow  ); 

  protected slots:
	virtual void checkBoxPressed( bool state  );

signals:
	void command(bool state, int id, int box );
};

#endif
