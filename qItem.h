#ifndef _qItem_h_
#define _qItem_h_

//#include <rw/tpdlist.h>
//typedefs for iterator and list types are at the bottom...
#include <qwidget.h>

class qItem { 

private:

	QWidget *win_;
	int r_;
	int c_;
	int id_;
	int groupId_;
	qItem *nextItem_;
	qItem *prevItem_;

public:

	qItem(QWidget *win, int r=0 , int c=0, int id=0, int groupId=0) 
		: win_(win), r_(r), c_(c), id_(id), groupId_(groupId), nextItem_(NULL), prevItem_(NULL) {}

	virtual ~qItem(){ /*delete win_;*/ }

	qItem *next(){ return nextItem_; }
	qItem *prev(){ return prevItem_; }

	QWidget *w(){ return win_; }
	int r(void) const { return r_; }
	int c(void) const { return c_; }
	int id(void) const{ return id_;	}
	int groupId(){return groupId_;}

};



#endif

