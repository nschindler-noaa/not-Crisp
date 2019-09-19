#include "qIdcTimeBase.h"
#include "crisp.h"

qDatContainer *qIdcTimeBase::GetData(const char *strFeature, const char *strSpecies){

	qDatContainer *target;
	//RWCollectable *rc;

	target = NewData();
	if(!target)
		return NULL;

	target->setString(0,strFeature);
	target->setString(1,strSpecies);
	
	int index = bt.find(target);

	if(index != -1){
		delete target;
		return bt.at(index);
	}

	if(FillData(target)){
		bt.inSort(target);
		return target;
	}
	return NULL;
}

int qIdcTimeBase::ModifyCache(const int index,		//position in the qDatContainer array
	const int flagFeature,	//If the D check box is on, modify for all dams
	const int flagSpecies,  //If the S check box is on, modify for all species
	const double dValue,    //The value returned 
	const char *strFeature, //Name of the dam, reach, etc
	const char *strSpecies, //Name of the species
	const int isDelta){		//If we're in Delta mode, dValue is the amount changed.


	//RWBinaryTreeIterator iterate(bt);
	//QPtrListIterator <qDatContainer> itr = QPtrListIterator <qDatContainer>(bt);
	qDatContainer *dat = NULL;
	int s, f;

	if(!(dat = GetData(strFeature,strSpecies))) 
		return 0;

	if(!flagFeature && !flagSpecies){
		dat->setDouble(index,dValue,isDelta);
	}
	else if(flagFeature && !flagSpecies){
		for (f = 0; f < (int)(listFeature_.count()); f++){
			if(dat = GetData(listFeature_[f],strSpecies))
				dat->setDouble(index,dValue,isDelta);
		}
	}
	else if(!flagFeature && flagSpecies){
		for (s = 0; s < (int)(listSpecies_.count()); s++){
			if(dat = GetData(strFeature,listSpecies_[s]))
				dat->setDouble(index,dValue,isDelta);
		}
	}
	else /* if(flagFeature && flagSpecies) */ {
		for (f = 0; f < (int)(listFeature_.count()); f++){
			for (s = 0; s < (int)(listSpecies_.count()); s++){
				if(dat = GetData(listFeature_[f],listSpecies_[s]))
					dat->setDouble(index,dValue,isDelta);
			}
		}
	}
	//invalidate();
	emit evInvalidate_();
	//evInvalidate_.notify();
	return 1;
}

