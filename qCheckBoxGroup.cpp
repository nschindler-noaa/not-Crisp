
#include "qCheckBoxGroup.h"
#include <qcheckbox.h>
#include <qptrlist.h>
#include "log.h"
#include <qlayout.h>

qCheckBoxGroup::qCheckBoxGroup(QWidget *w, QRect r, int baseId, char **itemStrings, int numStrings) 
		: QWidget(w), _numStrings(numStrings), _itemStrings(itemStrings), _baseId(baseId), _status(1){

		horizLayout = new QHBoxLayout(this, 3,-1, "horiz layout for check boxes");
		
		setMaximumHeight(25);

		for(int i=0;i<numStrings;i++){
			incItem = new QCheckBox(itemStrings[i],this ,QString::number(baseId+i));
			incItem->setMaximumHeight(15);
			horizLayout->addWidget(incItem);
			horizLayout->addItem( new QSpacerItem(20,15 ) );
			
			rwList.append(incItem);
			if(!connect( incItem, SIGNAL(toggled(bool)),this,SLOT(checkBoxPressed(bool )  )))
				log_msg(0, "Error connecting qCheckBox Group slot");
		}
		show();
}
qCheckBoxGroup::qCheckBoxGroup(QWidget *w, int baseId, char **itemStrings, int numStrings) 
		: QWidget(w), _numStrings(numStrings), _itemStrings(itemStrings), _baseId(baseId), _status(1){

		setMaximumHeight(25);
		horizLayout = new QHBoxLayout(this, 3,-1, "vertical layout for check boxes");
		for(int i=0;i<numStrings;i++){
			incItem = new QCheckBox(itemStrings[i],this,QString::number(baseId+i) );
			incItem->setMaximumHeight(15);
			horizLayout->addWidget(incItem);
			horizLayout->addItem( new QSpacerItem(20,15 ) );
		
			rwList.append(incItem);
			if(!connect( incItem, SIGNAL(toggled(bool)),this,SLOT(checkBoxPressed(bool)  )))
				log_msg(0, "Error connecting qCheckBox Group slot");
			
		}
		show();
}

qCheckBoxGroup::qCheckBoxGroup(QWidget *w, int baseId, QValueVector<QString> *stringVec) 
		: QWidget(w), _baseId(baseId), _status(1){
		_numStrings = stringVec->count();
	
		setMaximumHeight(25);
		horizLayout = new QHBoxLayout(this, 3,-1, "vertical layout for check boxes");
		for(int i=0;i<_numStrings;i++){
			incItem = new QCheckBox((*stringVec)[i] , this, QString::number(baseId+i) );
			incItem->setMaximumHeight(15);
			horizLayout->addWidget(incItem);
			horizLayout->addItem( new QSpacerItem(20,15 ) );
			rwList.append(incItem);
			if(!connect( incItem, SIGNAL(toggled(bool)),this,SLOT(checkBoxPressed(bool )  )))
					log_msg(0, "Error connecting qCheckBox Group slot");
		
		}
		show();
}

qCheckBoxGroup::~qCheckBoxGroup(){
	rwList.clear();
//	delete _itemStrings;
}

int qCheckBoxGroup::setItem(int num, bool onOff){
	if(num < 0 || num >= _numStrings) return 0;
	/*iterator *itr = new rwList.begin();
	(*itr)();
	for(int i=0;i<num;i++) 
		(*itr)();*/
	QCheckBox *cb = rwList.at(num);
	cb-> setChecked(onOff);
	//delete itr;
	return 0;
	//return onOff;
}

int qCheckBoxGroup::checkItem(int num){
	if(num < 0 || num >= _numStrings) return 0;
	
	QCheckBox *cb = rwList.at(num);
	cb->setChecked(true);
	
	return 0;
}


void qCheckBoxGroup::checkBoxPressed( bool state){

	QString num = sender()->name();
	emit command(state, _baseId, num.toInt()-_baseId);
}

void qCheckBoxGroup::enable(){ 
	_status = 1;
	QPtrListIterator<QCheckBox> *itr = new QPtrListIterator<QCheckBox>(rwList);
	while((*itr).current()){
		QCheckBox *cbt = itr->current();
		cbt->setEnabled(true);
		++(*itr);
	}
	delete itr;
}

void qCheckBoxGroup::disable(){ 
	_status = 1;
	QPtrListIterator<QCheckBox> *itr = new QPtrListIterator<QCheckBox>(rwList);
	while((*itr).current()){
		QCheckBox *cbt = itr->current();
		cbt->setEnabled(false);
		++(*itr);
	}
	delete itr;
}

void qCheckBoxGroup::setText(char **itemStr, int num){
	QPtrListIterator<QCheckBox> *itr = new QPtrListIterator<QCheckBox>(rwList);
	int i = 0;
	while((*itr).current() && i < num){
		QCheckBox *cbt = itr->current();
		cbt->setText(itemStr[i]);
		++(*itr);
		i++;
	}
	delete itr;

}

void qCheckBoxGroup::showCBG(int num, bool iShow){
	QCheckBox *cb = rwList.at(num);
	cb->setShown(iShow);

}

void qCheckBoxGroup::paintEvent ( QPaintEvent * ){

	
	QCheckBox * checkbox;
	QPtrListIterator <QCheckBox> * itr = new QPtrListIterator<QCheckBox> ( rwList);
	
	while(itr != 0 && itr->current() != 0){
		checkbox = itr->current();
		++(*itr);
		checkbox->repaint();
	}
	if(itr)
		delete itr;

}

