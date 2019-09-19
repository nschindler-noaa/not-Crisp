// qMapPane.h
//
//  This class is derived from zPane and customized to provide
//  notification for various events such as map area selections
//  button presses, mouse movements, etc.  It also stores the
//  the current map transform (ZatPointTransform), and handles all the 
//  screen buffering. This way the user can build custom
//  sub mediators(ZatMapSubMediator base) to handle everything from drawing geo features
//  to grid lines on the map without having to deal with the
//  ugly stuff.  See ZatMapPane.cpp for detailed function desc.


#ifndef _qMapPane_H_
#define _qMapPane_H_

#include <qobject.h>
#include <qapplication.h>

//#include "implicit.h"


//	This is probably the place where utImplicit is most used so I'll give a quick overview here.
//	utImplicit is a class written by Bill Hess at the UW Applied Physics Lab, who got the idea from 
//	John Vlissides' Interviews system.  I just think it's the cat's meow, so here goes:
//	utImplicit provides a mechanism for implicit invocation of member functions
//  from any class.
//  Ok lets take the "signal_point evLButtonPressed_;" "event" for example.  If you want
//  to register a function with the ZatMapPane so that your object gets notified whenever 
//  the Left Mouse Button is pressed you need to make 2 functions in your class:
//
//  static void MyClass::_myFunction(void *ptr, const zPoint p){
//		MyClass *mc	= (MyClass *)ptr;
//		mc->myFunction(p);
//	}
//  void MyClass::myFunction(const zPoint &p){
//     // YOUR CODE HERE
//  }
//
//  Q: Why do we need two functions?
//  A: If we didn't use a static function then utImplicit would need to know about every class
//		that it communcates with because it would need to know what 'this' is before it could
//		call a this method.  Static functions can be called without this knowledge so we have
//		it do the type cast and call.  This adds a few lines of code, but it makes it so that
//		the object you're registering your callback with doesn't need to "know" about the objects
//		it communicates with.
//	Q: How do I attach the static function _myFunction to the evLButtonPressed_ event?
//	A: The ZatMapPane class provides a function called evLButtonPressed that returns a reference
//		to the evLButtonPressed_ object.  So a typical attachment will look like this:
//	mapPane->evLButtonPressed()->attach(this, &MyClass::_myFunction);
//		you want to make sure to detach your function before you kill it so that
//		the evLButtonPressed_ doesn't invoke your object after it's killed. Here's how
//		you do a detachment:  (same as above with attach replaced with detach)
//	mapPane->evLButtonPressed()->detach(this, &MyClass::_myFunction);
//	
//	So that's it.. nothing big, 2 functions, an attach call and a detach call.  
//  The ZatMapSubMediator abstract base class might be worth a look as another example.


#include "log.h"
#include <qpixmap.h>

class qPointTransform;

//class QWidget;

class qMapPane : public QWidget {

	Q_OBJECT

  public:
	
	qMapPane(QWidget *p);
	~qMapPane();
	void printMap();

	QPainter *currPainter;
	QPixmap pm;
	QRect  drawArea;
	bool selecting ;
	bool painting;
	void initiateDraw();
	void endDraw();

    qPointTransform *zpt_;  // this is the current point tranform, I'm thinking of using a stack here.
	
	QPoint p1_, p2_, p0_;  // used by the DrawSelect function for area selections (zooming, etc)
    int leftMouseButtonPressed_, rightMouseButtonPressed_, 
		middleMouseButtonPressed_;
	bool isPrintMode_; 
   
	// Announcement event, used so that mediators have a mechanism for notifing other mediators
	// of custom information like "A Icon has been pressed, and here it is"


	//Process Announcements
	void sendAnnouncement(const int type, void *data){ 

		emit evAnnounce(type,data); }

	//Transform Methods
	virtual void pushTransform(qPointTransform *zpt);
	virtual qPointTransform *popTransform();
	virtual void clearTransforms();
	virtual qPointTransform *getTransform();
    

	virtual void paintEvent ( QPaintEvent * );
 
	virtual void resizeEvent ( QResizeEvent * );
    /*
	Well handle printing later
	virtual int print(zPrinterDisplay *pr, zRect *r);
	*/
  
	virtual void mousePressEvent ( QMouseEvent * e );
  
	virtual void mouseReleaseEvent ( QMouseEvent * e );
    
    virtual void mouseMoveEvent ( QMouseEvent * e );

	//Print functions

	virtual int isPrintMode(){ return isPrintMode_; }
  
 // public signals:
	//User Events
signals:
	void evRefresh();
	void evConfigure();
	void evDisplay();
	void evDraw();
	void evAnnounce(const int , void* );
	void evLButtonPressed( QPoint);
	void evMButtonPressed( QPoint);
	void evRButtonPressed( QPoint);
	void evMouseMoved(const QPoint );
	void evRButtonAreaSelection(const QRect );
	void evLButtonAreaSelection(const QRect );

  
  private:

    void drawSelect(bool openPainter);

};



#endif

