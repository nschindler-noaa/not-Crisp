/* qEditLine.h
 * This defines a class derived from zEditLine. The derived class detects
 *  the Enter (Return) key being pressed and calls the function provided
 *  by the parent.
 */

#ifndef __qEDITLINE_H__
#define __qEDITLINE_H__

//#include <zapp.hpp>
//#include <rw/cstring.h>
#include  <qlineedit.h>

class qEditLine: public  QLineEdit {
protected:
	//virtual int ch( zKeyEvt *ev );
	//zEvH *obj_;
	//void (*func_)( zEvH * );

public:
	qEditLine( QWidget *parent, int id, QString *dest = 0,
					unsigned long fieldFlags = 0 );
//	qEditLine( QWidget *parent, int id, QString *dest,
//					unsigned long fieldFlags = 0 );
	qEditLine( QWidget *parent,  unsigned long style = 0,
					const char *text = 0, int id = 0 );
	//void setCallback( zEvH *obj, void (*func)( zEvH *obj ) );
};

#endif

