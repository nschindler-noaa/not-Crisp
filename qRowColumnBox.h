//This is basically a container class for graphic objects like buttons
//sliders etc.  I was having it handle the clean up of such objects before
//I was aware that I could have zApp do it just by calling it's "destroy
//everything on dialog shutdown" function.	I use this class to do the 
//button maintenance and positioning in the ZatDialogBase and ZatTabDialog 
//classes.

#ifndef _qRowColumnBox_
#define _qRowColumnBox_

#include "qItem.h"
#include "qDefines.h"
#include "qConstants.h"


class qRowColumnBox {
	
	char *title;
	int numRows, numCols;
	float *rowPercent, *colPercent;
	QRect winPosition;
	int lastRow, lastCol;
	//rwItemList rwList;
	QPtrList<qItem> rwList;
	//zSizer *rSizer_;
    QWidget *wParent_;

	/* Used only by constructors.
	 */
	void consCommon( QWidget *w );

public:
 
	qRowColumnBox(QWidget *w, QRect r, char *text=NULL, int baseId=0);
	qRowColumnBox(QWidget *w, char *text=NULL, int baseId=0);
	~qRowColumnBox();

	//This allows you to iterate through the sliders and check boxes so that
	//you can update their status
	virtual QPtrListIterator<qItem> *getItemIterator(){return new QPtrListIterator<qItem> (rwList);}

	//zSizer *sizer(){ return rSizer_; }
    QWidget *parent() { return wParent_; }

	//These allow you to set the number of rows and columns you'd like for
	//your window objects. The int tells the number of rows/columns and the
	//pointer to a list of floats contains percentages that indicate how to
	//divide up the row/column. If you want it all divided up evenly, just pass
	//NULL in for the float *.  (BTW, I've never divided up a column anything 
	//but evenly so carefull with that one.)
	void setRows(int r, float *rp);
	void setCols(int c, float *cp);
	qItem *insert(QWidget *w, int r=-1, int c=-1, int id=0, int groupId=0); 		

};

#endif


