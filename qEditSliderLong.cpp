//#include <zapp.hpp>
//#include <intpack/zip.hpp>
//#include <rw/cstring.h>
#include "qEditSliderLong.h"


qEditSliderLong::qEditSliderLong(QWidget *w, char *text, int baseId, long rMin, long rMax, char *numFormat)  
		: qEditSliderBase(w,text,baseId,Q_LONGEDITSLIDER,rMax-rMin), rLongMin_(rMin), rLongMax_(rMax), longValue_(0){
		
		//if(!numFormat)
		//	el_ = new zLongEdit(this, new zPercentSizer(ZGRAV_RIGHT,100.0,28.0,sizer()), WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,baseId_,&longValue_);
		//else {
		//	nft_ = new zNumFormatter(FLD_NOTREQUIRED | FLD_STRIPLIT | FLD_COMPLETE, numFormat);
		//	el_ = new zLongEdit(this, new zPercentSizer(ZGRAV_RIGHT,100.0,28.0,sizer()), WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER,baseId_,&longValue_,nft_);
		//}
		ics_->setNumberFormat(numFormat);
		ics_->setLongRange( rMin,  rMax);
		ics_->setLongValue( 1);
		connect(ics_, SIGNAL(valueChanged(long)), this, SLOT(cbScrollBarChanged(long)));
		//el_->setNotifyChange(this, (NotifyProc)&qEditSliderLong::cbEditLineChanged);
		//sb_->info()->limits(zRange(0,divSize_));
		//st_->show();
		//sb_->show();
		//el_->show();
		show();
}

qEditSliderLong::~qEditSliderLong(){}
/*
void qEditSliderLong::cbEditLineChanged(long newval){
	if(!lock_){
		lock_=1;
		//char buf[40];
		long oldValue = longValue_;
		//sscanf(el_->text().data(),"%ld",&longValue_);
		if(longValue_ > rLongMax_ || longValue_ < rLongMin_){
			if(longValue_ > rLongMax_)longValue_ = rLongMax_;
			else longValue_ = rLongMin_;
			//sprintf(buf,"%ld",longValue_);
			//el_->text(buf);	
			//el_->setDirty();
		}
		if(delta_)
			emit evLongChanged(longValue_-oldValue);
		else
			emit evLongChanged(longValue_);

		//sb_->info()->pos(getPos(longValue_));
		//sb_->setDirty();
		lock_=0;
	}
	//return 0;	
	
}
*/
void qEditSliderLong::cbScrollBarChanged( long val){
	if(!lock_){
		lock_=1;
	//	char buf[40];
		long oldValue = longValue_;
		longValue_ = val;
	//	sprintf(buf,"%ld",longValue_);
	//	el_->text(buf);	
		if(delta_)
			emit evLongChanged(longValue_-oldValue,baseId_);
		else
			emit evLongChanged(longValue_,baseId_);
		//el_->setDirty();
		lock_=0;
	}
}

int qEditSliderLong::getLongPos(){
	return ics_->getPos();
	/*double percent;
		int max = sb_->info()->limits().hi() - sb_->info()->limits().lo();

		if(newPos >= rLongMax_)
			return sb_->info()->limits().hi();
		if(newPos <= rLongMin_)
			return sb_->info()->limits().lo();
		percent = double(newPos-rLongMin_)/double(rLongMax_-rLongMin_);
		return (int(percent*double(max))+sb_->info()->limits().lo());*/
}

long qEditSliderLong::getLongValue(){
	return ics_->getLongValue();
	/*int max = sb_->info()->limits().hi() - sb_->info()->limits().lo();
		return long((double(pos-sb_->info()->limits().lo())/double(max))*double(rLongMax_-rLongMin_))+rLongMin_;*/
}

void qEditSliderLong::setValue(long val){
	if(!lock_){
		lock_=1;
		longValue_ = val;
		ics_->setLongValue(val);
		//sb_->info()->pos(getPos(longValue_));
		//char buf[40];
		//sprintf(buf,"%ld",val);
		//el_->text(buf);	sb_->text(buf);
		//el_->setDirty();
		//sb_->setDirty();
		lock_=0;
	}
}
void qEditSliderLong::setRange(long r1,long r2){
	rLongMin_=r1;
	rLongMax_=r2;
	ics_->setLongRange( r1,  r2);
	//sb_->info()->limits(zRange(0,10000));

}
