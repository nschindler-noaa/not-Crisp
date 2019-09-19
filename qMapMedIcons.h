//qMapMedIcons
//
//This one is pretty simple.. it draws icons.
//The mapContoller object is passed in which contains our lists
//of mapObjects to be draw as icons such as a Dam or Release Site


#ifndef _qMapMedIcons_h_
#define _qMapMedIcons_h_

//#include <rw/tpordvec.h>
#include "qMapSubMediator.h"
#include <qptrlist.h>
#include <qpixmap.h>

class QmapController;

class qMapMedIcons : public qMapSubMediator {
		//Who knows how may types of bitmaps we'll be displaying
		//so I decided to maintain a vector of em.
		//RWTPtrOrderedVector<zBitmapDisplay> bdVector_;
		QPtrList <QPixmap> bdVector_;
		QmapController *mapCon_;

		//The rest of this is the same as the grid lines mediator
		//with the addition of our own RegisterEvents() functions.
		//(totally unnecessary but nice for showing an example
		//of how to customize your ZatMapSubMediator.)
	protected:

		virtual void RegisterEvents();
		//virtual void DetachEvents();

	public:

		qMapMedIcons(qMapPane *mp, QmapController *mc);
		virtual ~qMapMedIcons();
		virtual int isA();
	
		virtual void evDraw();

	private:
		QPixmap image0;
		QPixmap image1;
		QPixmap image2;
		QPixmap image3;
		QPixmap image4;
		QPixmap image5;
		QPixmap image6;
		QPixmap image7;

};


#endif


