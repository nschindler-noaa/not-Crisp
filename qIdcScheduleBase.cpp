/* qIdcScheduleBase.cpp Base class for interface data controllers which control
 *  a set of lists of values, selected by a single index. The list of values
 *  derives from a PERIOD_LIST structure in the underlying database. There
 *  are two types of possible period lists - 366 double values, one per day,
 *  or 366*24 boolean values, one per hour. The first type is a single-level
 *  list and the second type is a two-level list. The way to tell is by the
 *  length of the data arrays in the data container.
 *  The list is selected by two strings ("feature" and "species", although 
 *  in principle they could be anything).
 */

#include "qIdcScheduleBase.h"
#include "crisp.h"
#include "period.h"

qDatContainer * qIdcScheduleBase::NewData() {
	return new qDatContainer( 0, 2, 0 );
}

/* This routine sets up the feature and species lists. The default behavior
 *  is to have a PERIOD_LIST for each dam (species set to "all").
 */
void qIdcScheduleBase::Setup( void ) {
	int i;
	if (getNumSpecies() == 0) {
		insertSpecies( "all" );
		for (i = 0; i < num_dams; i++)
			insertFeature( dams[i]->seg_name );
	}
}

qDatContainer *qIdcScheduleBase::GetData( const char *feature, const char *species ){

	qDatContainer *target;
	//RWCollectable *rc;

	target = NewData();
	if(!target)
		return NULL;

	target->setString(0,feature);
	target->setString(1,species);

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

/*
	if(rc = bt.find((RWCollectable *)target)){
		delete target;
		return (qDatContainer *)rc;
	}

	if(FillData(target)){
		bt.insert((RWCollectable *)target);
		return target;
	}
*/

}

/* For a single-level list of 366 double values. */
int qIdcScheduleBase::ModifyCache(
		const char *feature,
		const char *species,
		const int index,		// Position in the qDatContainer array
		const double dValue,    // The value returned 
		const int gFeature,		// if !=0, apply to all features.
		const int gSpecies )	// if !=0, apply to all species.
{
	int i;
	qDatContainer *dat = NULL;

	if(!(dat = GetData( feature, species ))) 
		return 0;

	dat->setDouble( index, dValue, 0 );

	if (gFeature) {
		for (i = 0; i < getNumFeature(); i++) {
			if (dat = GetData( getFeature(i), species ))
				dat->setDouble( index, dValue, 0 );
		}
	}

	if (gSpecies) {
		for (i = 0; i < getNumSpecies(); i++) {
			if (dat = GetData( feature, getSpecies(i) ))
				dat->setDouble( index, dValue, 0 );
		}
	}

	emit evInvalidate_();
	return 1;
}

/* For a two-level list of 366*24 boolean values. */
int qIdcScheduleBase::ModifyCache(
		const char *feature,
		const char *species,
		const int index,		// Position in the qDatContainer array
		const int iValue,		// The value returned (0 or 1).
		const int gFeature,		// if !=0, apply to all features.
		const int gSpecies )	// if !=0, apply to all species.
{
	int i;
	qDatContainer *dat = NULL;

	if(!(dat = GetData( feature, species ))) 
		return 0;

	dat->setInt( index, iValue );

	if (gFeature) {
		for (i = 0; i < getNumFeature(); i++) {
			if (dat = GetData( getFeature(i), species ))
				dat->setInt( index, iValue );
		}
	}

	if (gSpecies) {
		for (i = 0; i < getNumSpecies(); i++) {
			if (dat = GetData( feature, getSpecies(i) ))
				dat->setInt( index, iValue );
		}
	}

	emit evInvalidate_();
	return 1;
}

/* Translations to/from PERIOD_LIST's in underlying database.
 */
void qIdcScheduleBase::getFromList( qDatContainer *dat, PERIOD_LIST *pl ) {
	int i, j;
	PERIOD_LIST *subpl;
	if (dat->lengthDoubleVec() == DAYS_IN_SEASON) {
		/* Single level list. */
		for (i = 0; i < DAYS_IN_SEASON; i++)
			dat->setDouble( i, 0.0 );
		for (/* pl init */; pl != NULL; pl = pl->next) {
			for (i = pl->start; i <= pl->end; i++)
				dat->setDouble( i, pl->value );
		}
	}
	else if (dat->lengthIntVec() == DAYS_IN_SEASON * 24) {
		/* Two level list. */
		for (i = 0; i < DAYS_IN_SEASON * 24; i++)
			dat->setInt( i, 0 );
		for (/* pl init */; pl != NULL; pl = pl->next) {
			for( subpl = pl->sub_periods; subpl != NULL; subpl = subpl->next) {
				for (i = pl->start; i <= pl->end; i++)
					for (j = subpl->start; j < subpl->end; j++)
						dat->setInt( i*24 + j, 1 );
			}
		}
	}
	else {
		/* Should never get here. */
	}
}

void qIdcScheduleBase::putToList( qDatContainer *dat, PERIOD_LIST **ppl ) {
	PERIOD_LIST *npl;
	int i, j, sum, diff;
	
	destroy_period_struct( *ppl );
	*ppl = NULL;

	if (dat->lengthDoubleVec() == DAYS_IN_SEASON) {
		/* Single level list. */

		/* Create a PERIOD_LIST structure for the last day.
		 */
		npl = new_period_struct();
		npl->start = npl->end = DAYS_IN_SEASON - 1;
		npl->value = dat->getDouble( DAYS_IN_SEASON - 1 );
		npl->sub_periods = npl->next = npl->prev = NULL;
		*ppl = npl;

		/* Crawl backwards through the season, from last day to first,
		 *  constructing period lists of adjacent days with equal values.
		 */
		for (i = DAYS_IN_SEASON - 2; i >= 0; i--) {
			if (dat->getDouble(i) == (*ppl)->value)
				(*ppl)->start = i;
			else {
				/* Value changed: create a new list element and weave it in.
				 */
				npl = new_period_struct();
				npl->start = npl->end = i;
				npl->value = dat->getDouble( i );
				npl->sub_periods = NULL;
				npl->prev = NULL;
				npl->next = *ppl;
				(*ppl)->prev = npl;
				*ppl = npl;
			}
		}
	}
	else if (dat->lengthIntVec() == DAYS_IN_SEASON * 24) {
		/* Two level list. */

		/* Run backwards in time creating new structures for days that
		 *  are different. No structure is needed for a day with all 24
		 *  values "0".
		 */
		for (i = DAYS_IN_SEASON - 1; i >= 0; i--) {
			/* If this day is all "0", just skip it.
			 */
			for (j = sum = 0; j < 24; j++)
				sum += dat->getInt( i*24 + j );
			if (sum == 0)
				continue;

			/* Decide if this day is different from the succeeding one.
			 */
			if (*ppl == NULL)
				diff = 1;
			else
				for (j = diff = 0; j < 24; j++)
					if (dat->getInt( i*24 + j ) != 
										dat->getInt( i*24 + j + 24 )) {
						diff = 1;
						break;
					}

			if (!diff)
				(*ppl)->start = i;
			else {
				/* Create a new two-level structure for this day.
				 */
				npl = new_period_struct();
				npl->prev = npl->sub_periods = NULL;
				npl->next = *ppl;
				npl->start = npl->end = i;
				npl->value = 0.0;
				if (*ppl)
					(*ppl)->prev = npl;
				*ppl = npl;

				/* Each run of "1" values gets an element in the 
				 * sub-period list.
				 */
				j = 24;
				while (j >= 0) {
					for (j--; j >= 0; j--) {
						if (dat->getInt( i*24 + j ))
							break;
					}
					if (j < 0)
						break;
					npl = new_period_struct();
					npl->prev = npl->sub_periods = NULL;
					npl->next = (*ppl)->sub_periods;
					(*ppl)->sub_periods = npl;
					if (npl->next)
						npl->next->prev = npl;
					npl->start = j;
					npl->end = j + 1;
					npl->value = 0.0;
					for (j--; j >= 0; j--)
						if (dat->getInt( i*24 + j ))
							npl->start = j;
						else
							break;
				}
			}
		}
	}
	else {
		/* Should never get here. */
	}
}

