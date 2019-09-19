/* qIdcSelectBase.h   Base class for interface data controllers which control
 *  a set of lists of values, selected by a single index (they may be
 *  logically organized into rows and columns, in which case all of the first
 *  row is stored, followed by all of the second row, etc - the column
 *  subscript varies fastest, as it were). The list is selected by two strings
 *  ("feature" and "species", although in principle they could be anything).
 */

#include "qIdcSelectBase.h"
#include "crisp.h"

qDatContainer *qIdcSelectBase::NewData() {
	// Return a single array of double values.
	return new qDatContainer( getNumRow() * getNumColumn(), 2, 0 );
}

/* This routine sets up the feature and species lists.
 * NOTE that derived classes must still define void Setup( void ) to define
 * the row and column labels, and may want to call this routine as well.
 */
void qIdcSelectBase::Setup( void ) {
	int i;
	for (i = 0; i < num_specs; i++)
		insertSpecies( specs[i].name );
	for (i = 0; i < num_dams; i++)
		insertFeature( dams[i]->seg_name );
}

qDatContainer *qIdcSelectBase::GetData( const char *feature, const char *species ){

	qDatContainer *target;
//	RWCollectable *rc;

	target = NewData();
	if(!target)
		return NULL;

	target->setString(0,feature);
	target->setString(1,species);

	int index = bt.find(target);

	if( index != -1 ){
		delete target;
		return bt.at(index);		
	}

	if(FillData(target)){
		bt.inSort(target);
		return target;
	}
	return NULL;
}

int qIdcSelectBase::ModifyCache(
		const char *feature,
		const char *species,
		const int index,		// Position in the qDatContainer array
		const double dValue,    // The value returned 
		const int gFeature,		// if !=0, apply to all features.
		const int gSpecies )	// if !=0, apply to all species.
{
	int i,j;
	qDatContainer *dat = NULL;

	if(!(dat = GetData( feature, species ))) 
		return 0;

	dat->setDouble( index, dValue, 0 );

    // Apply value to all features for the current species.
	if (gFeature && !gSpecies) {
		for (i = 0; i < getNumFeature(); i++) {
			if (dat = GetData( getFeature(i), species ))
				dat->setDouble( index, dValue, 0 );
		}
	}

    // Apply value to all species for the current feature.
	else if (gSpecies && !gFeature) {
		for (i = 0; i < getNumSpecies(); i++) {
			if (dat = GetData( feature, getSpecies(i) ))
				dat->setDouble( index, dValue, 0 );
		}
	}

    // Apply value to all combinations of species and features.
	else if (gSpecies && gFeature) {
		for (i = 0; i < getNumSpecies(); i++) {
            for (j = 0; j < getNumFeature(); j++) {
			    if (dat = GetData( getFeature(j), getSpecies(i) ))
				    dat->setDouble( index, dValue, 0 );
            }
		}
	}

	emit evInvalidate_();
	return 1;
}

