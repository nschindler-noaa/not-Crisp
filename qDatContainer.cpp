// botom of the barrel data container, inherits QObject for signal processing


#include "qDatContainer.h"

qDatContainer::qDatContainer(int is, int ds) : lock_(0) {

	nStrComp_ = 0;  /* all strings signif. in comparisons, by default. */

	iVec_.resize(is, 0);
	dVec_.resize(ds, 0.0);
}

qDatContainer::qDatContainer(int ds, int ss, int is) : lock_(0) {
	//int i;

	nStrComp_ = 0;  /* all strings signif. in comparisons, by default. */
	isDirty_ = 0;	/* 1 = data has been edited and not stored; else 0 */

	iVec_.resize((unsigned int)is, 0);
	
	strVec_.resize((unsigned int) ss, "");

	dVec_.resize((unsigned int) ds, 0.0);

}

/* Get or set number of strings which are significant in comparisons.
 */
int qDatContainer::nStrComp( int n ) {
	if (n >= 0)
		nStrComp_ = n;
	return nStrComp_;
}

int qDatContainer::getInt(int i) const {

	if(i>=0 && i<(int)(iVec_.count()))
		return iVec_[i];
	return 0;
}

QString qDatContainer::getString(int i){
	if(i>=0 && i<(int)(strVec_.count()))
		return (QString)strVec_[i];
	return QString("OUT OF BOUNDS");
}


double qDatContainer::getDouble(int i) {

	if(i>=0 && i<(int)(dVec_.count()))
		return dVec_[i];
	return 0.0;
}


int qDatContainer::setDouble(int i, double val, int delta){ 

	if(lock_)
		return 0;
	if(delta) {
		if (val != 0.0) {
			setDirty();
			dVec_[i] += val;
		}
	}
	else {
		if (val != dVec_[i]) {
			setDirty();
			dVec_[i] = val; 
		}
	}

	return 1;
}



int qDatContainer::setInt(int i, int val){ 
	if(i>=0 && i<(int)(iVec_.count())){
		iVec_[i] = val; 
		return 1;
	}	
	return 0;
}


int qDatContainer::setString(int i, const char *val){

	if(i>=0 && i<(int)(strVec_.count())){
		strVec_[i] = val; 
		return 1;
	}	
	return 0;

}


int qDatContainer::lengthIntVec(void) const { 
	return iVec_.count(); 
} 


int qDatContainer::lengthDoubleVec(void) const { 
	return dVec_.count(); 
} 


int qDatContainer::lengthStringVec(void) const { 
	return strVec_.count(); 
} 


int qDatContainer::lock(int i){ 
	if(i == -1) return lock_;
	lock_ = i;
	return lock_;
}


