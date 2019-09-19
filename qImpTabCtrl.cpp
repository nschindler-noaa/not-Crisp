/* qImpTabCtrl.cpp
 *  Implementation of our improved zTabCtrl class.
 */

#include "qImpTabCtrl.h"

qImpTabCtrl::qImpTabCtrl(QWidget *w, WFlags style_,
                             const char* ctrlId_) 
			: QTabWidget(w, ctrlId_, style_) {
/*
    leftBorder_ = new zPaneControl( w, 
                            new zGravSizer( ZGRAV_LEFT, 1, s ) );
    leftBorder_->backgroundColor( zColor( WHITE ) );
    leftBorder_->show();
    bottomBorder_ = new zPaneControl( w, 
                            new zGravSizer( ZGRAV_BOTTOM, 1, s ) );
    bottomBorder_->backgroundColor( zColor( BLACK ) );
    bottomBorder_->show();
    rightBorder_ = new zPaneControl( w, 
                            new zGravSizer( ZGRAV_RIGHT, 2, s ) );
    rightBorder_->backgroundColor( zColor( BLACK ) );
    rightBorder_->show();

    interiorSizer_ = new zGravSizer( ZGRAV_MIDDLE, 0, s );
    originalSizer_ = s;
	*/
	currentIndex = 0;  //set to the first index
	connect(this,SIGNAL(currentChanged(QWidget *)), this, SLOT(onPageChange(QWidget *)));
		
}

qImpTabCtrl::~qImpTabCtrl() {
	// the zTabs belonging to a zTabCtrl should be cleaned up automatically,
	//  but appearently they are not.
	tabs_.clear();
    //delete interiorSizer_;
    //delete originalSizer_;
    //delete leftBorder_;
    //delete bottomBorder_;
    //delete rightBorder_;
}

void qImpTabCtrl::appendTab( QWidget *tab, QString name) {
	tabs_.append( tab );

	addTab( tab, name );
}


void qImpTabCtrl::onPageChange(QWidget *t){
	
	int newIndex =  indexOf(t);   //the index is the position in the QTabWidget
	emit evPageChange_(newIndex, currentIndex);
	currentIndex = newIndex;

}


void qImpTabCtrl::paintEvent ( QPaintEvent * pe){

	QTabWidget::paintEvent(pe);
	currentPage ()->repaint();
}

