//This is the base class for the Equation Interface Data Control stuff. 
//Basically, the idc classes handle getting data from the crisp.h file and storing it
//in qDatContainer objects.  The idc caches these qDatContainer objects so that the data 
//is only read from the global data once, and so that you can easily support reset and apply 
//options in your dialogs.
//
//To make your own equation dialog you will need to derive from this class and make a new idcFOOBAREquation.


#include "qIdcEquationBase.h"
#include "crisp.h"

qIdcEquationBase::qIdcEquationBase(int iDialogID) {
	iMaxParams_ = -1;
	iDialogID_ = iDialogID;
}

/* Add an equation type to the list of equation types, and update the max
 *  number of parameters for all the equations we are managing.
 */
void qIdcEquationBase::addEqType( int iType ) {
	equation *peq = find_default_eq( iType );

	if (peq != NULL) {
		if (iMaxParams_ < peq->num_params)
			iMaxParams_ = peq->num_params;
		eqTypes_.append( iType );
	}
}

qDatContainer *qIdcEquationBase::NewData(){

	/* "ds" is 3 doubles for each parameter, plus 5 for general storage.
	 * "ss" is 1 string for each parameter, plus 3 for general storage.
	 * "is" is 3 integers for general storage.
	 */
	return new qDatContainer(EQN_NPARAMS*3+5,3+EQN_NPARAMS,3);

}

//This is where you take a qDatContainer and apply it's data to the global data in the crisp.h file.
void qIdcEquationBase::ApplyData(qDatContainer *dat){
	qInterfaceDataControl::ApplyData( dat );
}

//Here you fill a container with data from the global data using some key information already
//present in the dat file such as the name of a dam and a species.
int qIdcEquationBase::FillData(qDatContainer *dat){
	return 0;
}

//This basically takes any equation and puts all the data about it into a qDatContainer.
int qIdcEquationBase::GetEquationData(equation *eq, qDatContainer *dat){

	if(!eq || !dat)
		return 0;
	dat->setInt(0, eq->id);
	dat->setInt(1, eq->total_params);
	dat->setInt(2, eq->num_params);
	dat->setString(2, eq->formula);
	for(int j=0;j<eq->total_params;j++){
		dat->setDouble(j*3,eq->param[j].val);
		dat->setDouble(j*3+1,eq->param[j].min);
		dat->setDouble(j*3+2,eq->param[j].max);
		dat->setString(j+3,eq->param[j].name);
	}
	dat->clearDirty();

	return 1;
}

/* This puts equation data from the cache back into the underlying data
 *  structures. We assume that we just "got" this data, so we don't
 *  anticipate mismatches.
 */
void qIdcEquationBase::SetEquationData(equation *eq, qDatContainer *dat){

	if(!eq || !dat)
		return;
	eq->id = dat->getInt(0);
	eq->total_params = dat->getInt(1);
	eq->num_params = dat->getInt(2);
	eq->formula = find_default_eq(eq->id)->formula;
	for(int j=0;j<eq->total_params;j++){
		eq->param[j].val = dat->getDouble(j*3);
		eq->param[j].min = dat->getDouble(j*3+1);
		eq->param[j].max = dat->getDouble(j*3+2);
	}
	dat->clearDirty();

	return;
}

//This gets a piece of data from the global stuff in the crisp.h file if it's not already cached
//and returns it. 
qDatContainer *qIdcEquationBase::GetData(const char *strFeature, const char *strSpecies){

	qDatContainer *target ;
	int test;
	//RWCollectable *rc;

	target = NewData();
	if(!target)
		return NULL;

	target->setString(0,strFeature);
	target->setString(1,strSpecies);
	target->nStrComp( 2 );

	if((test = bt.find(target)) != -1){
		delete target;
		return bt.at(test);
	}

	if(FillData(target)){
		bt.append(target);
		return target;
	}
    delete target; /* something went wrong - clean up. */
	return NULL;
}

/* This routine gets underlying data from the database, without reference
 *  to the cache. The caller is responsible for cleanup.
 *  This is needed by the equation dialog mediator to handle desired
 *  functionality when changing equation types.
 */
qDatContainer *qIdcEquationBase::GetULData(const char *strFeature, 
                                         const char *strSpecies) {
	qDatContainer *target;

	target = NewData();
	if(!target)
		return NULL;

	target->setString(0,strFeature);
	target->setString(1,strSpecies);
	target->nStrComp( 2 );

	if(FillData(target))
		return target;

    delete target; /* something went wrong - clean up. */
    return NULL;
}

//This is how you modify data elements from your dialogs.. some of this stuff is pointless such as the
//the isDelta flag but I left it in in case you decide to add the check box control stuff in later.

int qIdcEquationBase::ModifyCache(const int index,		//position in the qDatContainer array
	const int flagFeature,	//If the D check box is on, modify for all dams
	const int flagSpecies,  //If the S check box is on, modify for all species
	const double dValue,    //The value returned 
	const char *strFeature, //Name of the dam, reach, etc
	const char *strSpecies, //Name of the species
	const int isDelta){		//If we're in Delta mode, dValue is the amount changed.

	qDatContainer *dat;
	if(!(dat = GetData(strFeature,strSpecies))) 
		return 0;
	if (index >= 0) {
		/* Normal case: index denotes a value in the double's array. 
		 */
		dat->setDouble(index*3,dValue);
	}
	else {
		/* Weird case. dValue is actually the equation form we want to use.
		 *  (" + 0.1" guards against round-off errors).
		 */
		dat->setInt( 0, (int)(dValue + 0.1) );
		dat->setDirty();
	}

	emit evInvalidate_();
	return 1;
}

