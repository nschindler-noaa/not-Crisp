
#include "qEditSliderBase.h"
#include "log.h"


qEditSliderBase::qEditSliderBase(QWidget *w, char *text, int baseId, int editSliderType, long divSize) 
		: QWidget(w), nft_(NULL), baseId_(baseId), lock_(0), delta_(0), editSliderType_(editSliderType), divSize_(divSize){
		
		setPaletteBackgroundColor(red);
		st_ = new QLabel(this, text,baseId);
	
		ics_ = new qImConScroller(this, "child slider for qeditsliderBase", editSliderType);
		
	
}

qEditSliderBase::~qEditSliderBase(){

	if(nft_)delete nft_;

}

void qEditSliderBase::disable(void){
	ics_->setEnabled(false);

}

void qEditSliderBase::enable(void){
	ics_->setEnabled(true);

}

void qEditSliderBase::setText(char *str){
	if(!lock_){
		lock_=1;
		st_->setText(str);
		lock_=0;
	}
}




