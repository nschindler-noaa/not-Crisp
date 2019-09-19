//#include <zapp.hpp>
//#include <intpack/zip.hpp>
//#include <rw/cstring.h>
#include "qEditSliderDouble.h"

qEditSliderDouble::qEditSliderDouble(QWidget *w, char *text, int baseId, double rMin, double rMax, char *numFormat, long divSize) 
		: qEditSliderBase(w,text,baseId,Q_DOUBLEEDITSLIDER,divSize), rDoubleMin_(rMin), rDoubleMax_(rMax), doubleValue_(0.0)  {
		//if(!numFormat)
			//el_ = new zDoubleEdit(this, new zPercentSizer(ZGRAV_RIGHT,100.0,28.0,sizer()), WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,baseId_,&doubleValue_);
		//else {
		//	nft_ = new zNumFormatter(FLD_NOTREQUIRED | FLD_STRIPLIT | FLD_COMPLETE, numFormat);
		//	el_ = new zDoubleEdit(this, new zPercentSizer(ZGRAV_RIGHT,100.0,28.0,sizer()), WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,baseId_,&doubleValue_,nft_);
		//}
		//el_->setNotifyChange(this, (NotifyProc)&qEditSliderDouble::cbEditLineChanged);
		//sb_->info()->limits(zRange(0,divSize_));
		ics_->setNumberFormat(numFormat);
		ics_->setDoubleRange( rMin,  rMax);
		ics_->setDoubleValue( 1.0);
		connect(ics_, SIGNAL(valueChanged(double)), this, SLOT(cbScrollBarChanged(double)));
		//st_->show();
		//sb_->show();
		//el_->show();
		show();
}

qEditSliderDouble::~qEditSliderDouble(){}


void qEditSliderDouble::cbScrollBarChanged(double val){
	if(!lock_){
		lock_=1;
		//char buf[40];
		double oldValue = doubleValue_;
		doubleValue_ = val;
		//sprintf(buf,"%lf",doubleValue_);
		//el_->text(buf);	
		if(delta_)
			emit evDoubleChanged(doubleValue_-oldValue,baseId_);
		else
			emit evDoubleChanged(doubleValue_,baseId_);
		//el_->setDirty();
		lock_=0;
	}
}

int qEditSliderDouble::getPos(double newPos){
	
	return ics_->getPos();
	/*double percent;
		int max = sb_->info()->limits().hi() - sb_->info()->limits().lo();
		if(newPos >= rDoubleMax_)
			return sb_->info()->limits().hi();
		if(newPos <= rDoubleMin_)
			return sb_->info()->limits().lo();
		percent = (newPos-rDoubleMin_)/(rDoubleMax_-rDoubleMin_);
		return (int(percent*double(max))+sb_->info()->limits().lo());*/

}

double qEditSliderDouble::getDoubleValue(int pos){
		
		return ics_->getDoubleValue();

		//int max = sb_->info()->limits().hi() - sb_->info()->limits().lo();
		//return double((double(pos-sb_->info()->limits().lo())/double(max))*double(rDoubleMax_-rDoubleMin_))+rDoubleMin_;
}

void qEditSliderDouble::setValue(double val){


	if(!lock_){
		lock_=1;
		ics_->setDoubleValue(val);
		/*
		doubleValue_ = val;
		sb_->info()->pos(getPos(doubleValue_));
		char buf[40];
		sprintf(buf,"%lf",val);
		el_->text(buf);
		el_->setDirty();
		sb_->setDirty();*/
		lock_=0;
	}
	
}

void qEditSliderDouble::setRange(double r1,double r2){
	rDoubleMin_=r1;
	rDoubleMax_=r2;
	ics_->setDoubleRange(r1,r2);
	//sb_->info()->limits(zRange(0,10000));
}
