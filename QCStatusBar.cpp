/* Implementation for the QCStatusBar
*/

#include "QCStatusBar.h"
#include <qapplication.h>
#include <qtoolbutton.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include <qlabel.h>


QCStatusBar::QCStatusBar ( QWidget * parent , const char * name  ) : QStatusBar(parent,name){
	
	setSizeGripEnabled(false);
	

	timeFormat = QString("h:mm AP");

	timeLabel = new QLabel( this, "current Time" );
		
	timeLabel->setText(QTime::currentTime().toString(timeFormat));
	
	latLabel = new QLabel( this, "" );	
	lonLabel = new QLabel( this, "" );
	reachDamLabel= new QLabel( this, "" );
	releaseLabel = new QLabel( this, "" );
	
	//Let these take up any extra space
	QSizePolicy policy = QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred, 1, 0);
	reachDamLabel->setSizePolicy (policy);
	releaseLabel->setSizePolicy (policy);

	
	latLabel->setFixedSize ( 80,20 );
	lonLabel->setFixedSize ( 80,20 );
	//releaseLabel->setFixedSize ( 180,20 );

	//Add toolbar widgets
	addWidget(reachDamLabel ,1,true);
	addWidget(releaseLabel ,0,true);	
	addWidget(latLabel ,0,true);
	addWidget(lonLabel ,0,true);
	addWidget(timeLabel ,0,true);



	//start a 500 milisecond update to the clock
	startTimer(500);
}

QCStatusBar::~QCStatusBar (){

}

void QCStatusBar::timerEvent( QTimerEvent *e )
{
//	qDebug( "timer event, id %d", e->timerId() );
	timeLabel->setText(QTime::currentTime().toString(timeFormat));
}
    
void QCStatusBar::setDamReach(char *msg){

	reachDamLabel->setText(msg);
}
//Sets a new message in the left message box
void QCStatusBar::setMessage(char *msg){

	reachDamLabel->setText(msg);
}

void QCStatusBar::setRelease(char *msg){
	releaseLabel->setText(msg);
}

void QCStatusBar::setLat(char* msg){
	latLabel->setText(msg);
}

void QCStatusBar::setLon(char* msg){
	lonLabel->setText(msg);
}
