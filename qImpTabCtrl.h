
#ifndef _qImpTabCtrl_ 
#define _qImpTabCtrl_ 

//#include <zapp.hpp>
//#include <rw/tpdlist.h>
//#include "implicit.h"
#include <qtabwidget.h>

//typedef utImplicit2<const int, const int> signalIntInt;

class qImpTabCtrl : public QTabWidget {
	
	Q_OBJECT
	
	//signalIntInt evPageChange_;
	//RWTPtrDlist<zTab> tabs_;
	QPtrList<QWidget> tabs_;  //each tab holds a widget
    //zPaneControl *leftBorder_, *bottomBorder_, *rightBorder_;
    //zSizer *interiorSizer_, *originalSizer_;
	int currentIndex;

	void paintEvent ( QPaintEvent * );
  public:

	qImpTabCtrl(QWidget *w, WFlags style_=0, 
                            const char*  ctrlId_="A tab");
	~qImpTabCtrl();
	//signalIntInt *evPageChange(){ return &evPageChange_; }
	//void onPageChange(int newIndex, int oldIndex) {	evPageChange_.notify(newIndex,oldIndex); }
	void appendTab( QWidget *tab, QString name );
    //zSizer *sizer() { return interiorSizer_; }
  protected slots:
	virtual void onPageChange(QWidget *);
  signals:
	void evPageChange_(int newIndex, int oldIndex);


};

#endif

