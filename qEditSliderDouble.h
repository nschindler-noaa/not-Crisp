//An edit slider that controls a double.
//What is an edit slider? it's a label tag, a scroll bar, and a text edit
//field that work together to make it easy for a user to modify a (double)
//value

#ifndef _qEditSliderDouble_h_
#define _qEditSliderDouble_h_

#include "qEditSliderBase.h"
//#include "implicit.h"

//typedef utImplicit2<const double, const int> signalDoubleInt;

class qEditSliderDouble : public qEditSliderBase {

	Q_OBJECT
	
	double rDoubleMin_, rDoubleMax_, doubleValue_;
//	signalDoubleInt evDoubleChanged_;

	// internal conversion functions
	int getPos(double newPos);
	double getDoubleValue(int pos);

  public:

	qEditSliderDouble(QWidget *w, char *text, int baseId, double rMin, double rMax, char *numFormat=NULL, long divSize=1000); 	
	~qEditSliderDouble();

	inline double doubleValue() const { return ics_->getDoubleValue(); }
	
	void setRange(double r1, double r2);
    void setValue(double val);

	//Events and event notifiers
    //signalDoubleInt *evDoubleChanged() { return &evDoubleChanged_; }
  protected slots:
	void cbScrollBarChanged(double);
  signals:
	void evDoubleChanged(double, int);

//	int cbEditLineChanged(zEvent *);
//	void cbScrollBarChanged(const int val);

};


#endif    
