#ifndef _qEditSliderLong_h_
#define _qEditSliderLong_h_

#include "qEditSliderBase.h"
//#include "implicit.h"

//typedef utImplicit2<const long, const int> signalLongInt;

class qEditSliderLong : public qEditSliderBase {

	Q_OBJECT

	long rLongMin_, rLongMax_, longValue_;
	//signalLongInt evLongChanged_;

	// internal conversion functions
	int getLongPos();
	long getLongValue();

  public:

	qEditSliderLong(QWidget *w, char *text, int baseId, long rMin, long rMax, char *numFormat=NULL); 
	~qEditSliderLong();

	inline long longValue() const { return ics_->getLongValue(); } 
	
	void setRange(long r1, long r2);
	void setValue(long val);

  protected slots:
	void cbScrollBarChanged(long);
  signals:
	void evLongChanged(long, int);

	//Events and event notifiers
	//signalLongInt *evLongChanged() { return &evLongChanged_; }

	//int cbEditLineChanged(zEvent *);
	//void cbScrollBarChanged(const int val);

};


#endif    
