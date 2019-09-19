/* idcSelect.cpp   Derived classes for interface data controllers which control
 *  a set of lists of values, selected by a single index (they may be
 *  logically organized into rows and columns, in which case all of the
 *  first row is stored, then all of the second row, etc). The sets are
 *  selected by two strings (feature and species, although in principle they
 *  could be anything).
 */

#include "qIdcSelect.h"
#include "qIdcFGEEquation.h"
#include "crisp.h"
#include "resource.h"
#include <stdlib.h>

/* ********************************************************************* */

/* Reach predation coefficients.
 */
qIdcReachPredCoeff::qIdcReachPredCoeff() : qIdcSelectBase( ID_PREDATIONCOEF_REACH ) {
	setHelpPage( 54 );
}

void qIdcReachPredCoeff::cbPurge( void ) {
    clearLists();
    qIdcSelectBase::cbPurge();
}

void qIdcReachPredCoeff::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
        nrc = num_reach_classes;  /* remember this for validation */

        for (i = 0; i < num_reach_classes; i++) {
            insertFeature( reach_class_names[i] );
        }
        insertSpecies( "ALL" );

        insertColumnLabel( "mean (x10^-7)" );
		insertColumnLabel( "low (x10^-7)" );
		insertColumnLabel( "high (x10^-7)" );

		for (i = 0; i < num_specs; i++) {
			insertRowLabel( specs[i].name );
		}
	}
}

int qIdcReachPredCoeff::FillData(qDatContainer *dat) {
	int i, j;
	
    if (nrc != num_reach_classes)
        return 1;  /* Someone changed the rules on us. */

	for(j = 0; j < num_reach_classes; j++)
		if(!strcmp(reach_class_names[j], dat->getString(0)))
			break;
	if (j >= num_reach_classes)
		return 0;

	for (i = 0; i < getNumRow(); i++) {
		FillBrokenStickData(dat, i*3, &specs[i].reach_pcoef[j]);
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcReachPredCoeff::ApplyData(qDatContainer *dat) {
	int i, j;

    if (nrc != num_reach_classes)
        return;  /* Someone changed the rules on us. */

	for(j = 0; j < num_reach_classes; j++)
		if(!strcmp(reach_class_names[j], dat->getString(0)))
			break;
	if (j >= num_reach_classes)
		return;

	for (i = 0; i < getNumRow(); i++) {
		ApplyBrokenStickData(dat, i*3, &specs[i].reach_pcoef[j]);
	}

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcReachPredCoeff qIdcReachPredCoeff_;

/* ********************************************************************* */

/* Predator mortality coefficients. (X-T predation mortality alg.)
 */
qIdcPredMortCoef::qIdcPredMortCoef() : qIdcSelectBase( ID_PREDMORTCOEF ) {
	setHelpPage( 91 );
}

void qIdcPredMortCoef::cbPurge( void ) {
    clearLists();
    qIdcSelectBase::cbPurge();
}

void qIdcPredMortCoef::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
        nrc = num_reach_classes;  /* remember this for validation */

        for (i = 0; i < num_reach_classes; i++) {
            insertFeature( reach_class_names[i] );
        }
        insertSpecies( "ALL" );

		insertColumnLabel( "distance coef" );
		insertColumnLabel( "time coef" );

		for (i = 0; i < num_specs; i++) {
			insertRowLabel( specs[i].name );
		}
		for (i = 0; i < stocks.num; i++) {
			insertRowLabel( stocks.s[i]->name );
		}
	}
}

int qIdcPredMortCoef::FillData(qDatContainer *dat) {
	int i,j,k;
	
    if (nrc != num_reach_classes)
        return 1;  /* Someone changed the rules on us. */
	if (getNumRow() == 0)
        return 1;  /* ditto */

	for(k = 0; k < num_reach_classes; k++)
		if(!strcmp(reach_class_names[k], dat->getString(0)))
			break;
	if (k >= num_reach_classes)
		return 0;

	for (i = 0; i < num_specs; i++) {
		dat->setDouble( i+i, specs[i].stock.distance_coeff[k] );
		dat->setDouble( i+i+1, specs[i].stock.time_coeff[k] );
	}
	for (i = 0, j = num_specs; i < stocks.num; i++, j++) {
		dat->setDouble( j+j, stocks.s[i]->distance_coeff[k] );
		dat->setDouble( j+j+1, stocks.s[i]->time_coeff[k] );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcPredMortCoef::ApplyData(qDatContainer *dat) {
	int i,j,k;

    if (nrc != num_reach_classes)
        return;  /* Someone changed the rules on us. */
	if (getNumRow() == 0)
        return;  /* ditto */

	for(k = 0; k < num_reach_classes; k++)
		if(!strcmp(reach_class_names[k], dat->getString(0)))
			break;
	if (k >= num_reach_classes)
		return;

	for (i = 0; i < num_specs; i++) {
		specs[i].stock.distance_coeff[k] = (float)(dat->getDouble( i+i ));
		specs[i].stock.time_coeff[k] = (float)(dat->getDouble( i+i+1 ));
	}
	for (i = 0, j = num_specs; i < stocks.num; i++, j++) {
		stocks.s[i]->distance_coeff[k] = (float)(dat->getDouble( j+j ));
		stocks.s[i]->time_coeff[k] = (float)(dat->getDouble( j+j+1 ));
	}

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcPredMortCoef qIdcPredMortCoef_;

/* Velocity variance parameter.
 */
qIdcVVAR::qIdcVVAR() : qIdcSelectBase( ID_VVAR ) {
	setHelpPage( 55 );
}

void qIdcVVAR::cbPurge( void ) {
    clearLists();
    qIdcSelectBase::cbPurge();
}

void qIdcVVAR::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
        nrc = num_reach_classes;  /* remember this for validation */

        for (i = 0; i < num_reach_classes; i++) {
            insertFeature( reach_class_names[i] );
        }
        insertSpecies( "ALL" );

		insertColumnLabel( "var" );

		for (i = 0; i < num_specs; i++) {
			insertRowLabel( specs[i].name );
		}
		for (i = 0; i < stocks.num; i++) {
			insertRowLabel( stocks.s[i]->name );
		}
	}
}

int qIdcVVAR::FillData(qDatContainer *dat) {
	int i,j,k;
	
    if (nrc != num_reach_classes)
        return 1;  /* Someone changed the rules on us. */
	if (getNumRow() == 0)
        return 1;  /* ditto */

	for(k = 0; k < num_reach_classes; k++)
		if(!strcmp(reach_class_names[k], dat->getString(0)))
			break;
	if (k >= num_reach_classes)
		return 0;

	for (i = 0; i < num_specs; i++) {
		dat->setDouble( i, specs[i].stock.vvar[k] );
	}
	for (i = 0, j = num_specs; i < stocks.num; i++, j++) {
		dat->setDouble( j, stocks.s[i]->vvar[k] );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcVVAR::ApplyData(qDatContainer *dat) {
	int i,j,k;

    if (nrc != num_reach_classes)
        return;  /* Someone changed the rules on us. */
	if (getNumRow() == 0)
        return;  /* ditto */

	for(k = 0; k < num_reach_classes; k++)
		if(!strcmp(reach_class_names[k], dat->getString(0)))
			break;
	if (k >= num_reach_classes)
		return;

	for (i = 0; i < num_specs; i++) {
		specs[i].stock.vvar[k] = (float)(dat->getDouble( i ));
	}
	for (i = 0, j = num_specs; i < stocks.num; i++, j++) {
		stocks.s[i]->vvar[k] = (float)(dat->getDouble( j ));
	}

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcVVAR qIdcVVAR_;

/* ********************************************************************* */

/* migration rate variance parameters.
 */
qIdcMigRateVar::qIdcMigRateVar() : qIdcSelectBase( ID_MIGRATIONRATEVAR ) {
	setHelpPage( 56 );
}

void qIdcMigRateVar::cbPurge( void ) {
    clearLists();
    qIdcSelectBase::cbPurge();
}

void qIdcMigRateVar::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
        nrc = num_reach_classes;  /* remember this for validation */

        for (i = 0; i < num_reach_classes; i++) {
            insertFeature( reach_class_names[i] );
        }
        insertSpecies( "ALL" );

		insertColumnLabel( "mean" );
		insertColumnLabel( "low" );
		insertColumnLabel( "high" );

		for (i = 0; i < num_specs; i++) {
			insertRowLabel( specs[i].name );
		}
		for (i = 0; i < stocks.num; i++) {
			insertRowLabel( stocks.s[i]->name );
		}
	}
}

int qIdcMigRateVar::FillData(qDatContainer *dat) {
	int i,j,k;
	
    if (nrc != num_reach_classes)
        return 1;  /* Someone changed the rules on us. */
	if (getNumRow() == 0)
        return 1;  /* ditto */

	for(k = 0; k < num_reach_classes; k++)
		if(!strcmp(reach_class_names[k], dat->getString(0)))
			break;
	if (k >= num_reach_classes)
		return 0;

	for (i = 0; i < num_specs; i++) {
		FillBrokenStickData(dat, i*3, &specs[i].stock.mvcoef[k]);
	}
	for (i = 0, j = num_specs; i < stocks.num; i++, j++) {
		FillBrokenStickData(dat, j*3, &stocks.s[i]->mvcoef[k]);
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcMigRateVar::ApplyData(qDatContainer *dat) {
	int i,j,k;

    if (nrc != num_reach_classes)
        return;  /* Someone changed the rules on us. */
	if (getNumRow() == 0)
        return;  /* ditto */

	for(k = 0; k < num_reach_classes; k++)
		if(!strcmp(reach_class_names[k], dat->getString(0)))
			break;
	if (k >= num_reach_classes)
		return;

	for (i = 0; i < num_specs; i++) {
		ApplyBrokenStickData(dat, i*3, &specs[i].stock.mvcoef[k]);
	}
	for (i = 0, j = num_specs; i < stocks.num; i++, j++) {
		ApplyBrokenStickData(dat, j*3, &stocks.s[i]->mvcoef[k]);
	}

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcMigRateVar qIdcMigRateVar_;

/* ********************************************************************* */

/* Reach predator densities.
 */
qIdcReachPredDensity::qIdcReachPredDensity() : qIdcSelectBase( ID_REACHPREDDEN ) {
	setHelpPage( 49 );
}

void qIdcReachPredDensity::Setup(void) {
	river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "mean" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == REACH_SEG) {
				insertRowLabel( seg->seg_name );
			}
		}

		/* Set up feature and species lists. Base class lists dams - since
		 *  we want species (only), we must duplicate the base class code here.
		 */
		int i;
		for (i = 0; i < num_specs; i++)
			insertSpecies( specs[i].name );
		insertFeature( "ALL" );
	}
}

int qIdcReachPredDensity::FillData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	int i;
	
	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return 0;
	
	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), REACH_SEG );
		dat->setDouble( i, seg->current->reach.species[s].pred_pop_mean );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcReachPredDensity::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	int i;

	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), REACH_SEG );
		seg->current->reach.species[s].pred_pop_mean
                        = (float)(dat->getDouble( i ));
	}

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcReachPredDensity qIdcReachPredDensity_;

/* Dam predator densities.
 */
qIdcDamPredDensity::qIdcDamPredDensity() : qIdcSelectBase( ID_DAMPREDDEN ) {
	setHelpPage( 50 );
}

void qIdcDamPredDensity::Setup(void) {
	river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "forebay" );
		insertColumnLabel( "tailrace" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == DAM_SEG) {
				insertRowLabel( seg->seg_name );
			}
		}

		/* Set up feature and species lists. Base class lists dams - since
		 *  we want species (only), we must duplicate the base class code here.
		 */
		int i;
		for (i = 0; i < num_specs; i++)
			insertSpecies( specs[i].name );
		insertFeature( "ALL" );
	}
}

int qIdcDamPredDensity::FillData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	int i;
	
	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return 0;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		dat->setDouble( i+i, seg->current->dam.species[s].pred_pop_mean );
		dat->setDouble( i+i+1, seg->current->dam.species[s].pred_pop_tailrace );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcDamPredDensity::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	int i;

	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		seg->current->dam.species[s].pred_pop_mean
                       = (float)(dat->getDouble( i+i ));
		seg->current->dam.species[s].pred_pop_tailrace 
                       = (float)(dat->getDouble( i+i+1 ));
	}

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcDamPredDensity qIdcDamPredDensity_;

/* Various mortality proportions at dams (really, at powerhouses).
 */
qIdcDamMortality::qIdcDamMortality() : qIdcSelectBase( ID_MORTALITY ) {
	setHelpPage( 69 );
}

void qIdcDamMortality::Setup(void) {
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "mean" );
		insertColumnLabel( "low" );
		insertColumnLabel( "high" );

		insertRowLabel( "RSW" );
		insertRowLabel( "Spillway" );
		insertRowLabel( "Sluiceway" );
		insertRowLabel( "Bypass" );
		insertRowLabel( "Turbine" );
		insertRowLabel( "Transport" );

		/* Set up feature and species lists. Base class lists dams - since
		 *  we want powerhouses, we must duplicate the base class code here.
		 */
		int i;
		for (i = 0; i < num_specs; i++)
			insertSpecies( specs[i].name );
		for (i = 0; i < num_powerhouses; i++)
			insertFeature( phouses[i]->seg_name );
	}
}

int qIdcDamMortality::FillData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		return 0;

	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return 0;

	FillBrokenStickData(dat, IDC_DAM_MORT_OFF_RSW*3, &seg->current->dam.species[s].rsw_mort);
	FillBrokenStickData(dat, IDC_DAM_MORT_OFF_SPILL*3, &seg->current->dam.species[s].spill_mort);
	FillBrokenStickData(dat, IDC_DAM_MORT_OFF_SLUICEWAY*3, &seg->current->dam.species[s].sluiceway_mort);
	FillBrokenStickData(dat, IDC_DAM_MORT_OFF_BYPASS*3, &seg->current->dam.species[s].bypass_mort);
	FillBrokenStickData(dat, IDC_DAM_MORT_OFF_TURBINE*3, &seg->current->dam.species[s].turbine_mort);
	FillBrokenStickData(dat, IDC_DAM_MORT_OFF_TRANSPORT*3, &seg->current->dam.species[s].transport_mort);
	
	dat->clearDirty();
	return 1;
}

void qIdcDamMortality::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		return;

	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return;

	ApplyBrokenStickData(dat, IDC_DAM_MORT_OFF_RSW*3, &seg->current->dam.species[s].rsw_mort);
	ApplyBrokenStickData(dat, IDC_DAM_MORT_OFF_SPILL*3, &seg->current->dam.species[s].spill_mort);
	ApplyBrokenStickData(dat, IDC_DAM_MORT_OFF_SLUICEWAY*3, &seg->current->dam.species[s].sluiceway_mort);
	ApplyBrokenStickData(dat, IDC_DAM_MORT_OFF_BYPASS*3, &seg->current->dam.species[s].bypass_mort);
	ApplyBrokenStickData(dat, IDC_DAM_MORT_OFF_TURBINE*3, &seg->current->dam.species[s].turbine_mort);
	ApplyBrokenStickData(dat, IDC_DAM_MORT_OFF_TRANSPORT*3, &seg->current->dam.species[s].transport_mort);

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcDamMortality qIdcDamMortality_;

/* ******************************************************************** */

/* Separation probabilities at dams (really, at powerhouses).
 */
qIdcSeparationProb::qIdcSeparationProb() : qIdcSelectBase( ID_SEPARATIONPROB ) {
	setHelpPage( 70 );
}

void qIdcSeparationProb::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "separation probability" );

		for (i = 0; i < num_specs; i++)
			insertRowLabel( specs[i].name );

		/* Set up feature and species lists. Base class lists dams - since
		 *  we want powerhouses, we must duplicate the base class code here.
		 */
		insertSpecies( "all" );
		for (i = 0; i < num_powerhouses; i++)
			insertFeature( phouses[i]->seg_name );
	}
}

int qIdcSeparationProb::FillData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		return 0;

	for(s=0;s<num_specs;s++)
		dat->setDouble( s, seg->current->dam.species[s].separation_prob );
	
	dat->clearDirty();
	return 1;
}

void qIdcSeparationProb::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		return;

	for(s=0;s<num_specs;s++)
		seg->current->dam.species[s].separation_prob = dat->getDouble( s );

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcSeparationProb qIdcSeparationProb_;

/* ******************************************************************** */

/* Various fge proportions at dams (really, at powerhouses).
 */
qIdcFGE::qIdcFGE() : qIdcSelectBase( ID_FGE ) {
	setHelpPage( 71 );
}

void qIdcFGE::Setup(void) {
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "mean" );
		insertColumnLabel( "low" );
		insertColumnLabel( "high" );

		insertRowLabel( "day" );
		insertRowLabel( "night" );

		/* Set up feature and species lists. Base class lists dams - since
		 *  we want powerhouses, we must duplicate the base class code here.
		 */
		int i;
		for (i = 0; i < num_specs; i++)
			insertSpecies( specs[i].name );
		for (i = 0; i < num_powerhouses; i++)
			insertFeature( phouses[i]->seg_name );
	}
}

int qIdcFGE::FillData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		return 0;

	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return 0;

	FillBrokenStickData(dat, IDC_DAM_FGE_DAY*3, &seg->current->dam.species[s].day_fge);
	FillBrokenStickData(dat, IDC_DAM_FGE_NIGHT*3, &seg->current->dam.species[s].night_fge);
	
	dat->clearDirty();
	return 1;
}

void qIdcFGE::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		return;

	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return;

	ApplyBrokenStickData(dat, IDC_DAM_FGE_DAY*3, &seg->current->dam.species[s].day_fge);
	ApplyBrokenStickData(dat, IDC_DAM_FGE_NIGHT*3, &seg->current->dam.species[s].night_fge);

	dat->clearDirty();

	qIdcSelectBase::ApplyData( dat );


	/* The night fge value is used to calculate the FGE Equation for display.
     *  So notify the FGE Equation qIdc that something has changed:
	 *  any dialog which depends on it will therefore recalculate whatever
	 *  it displays.
	 */
	qIdcFGEEquation_.invalidate();
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcFGE qIdcFGE_;

/* ******************************************************************** */

/* Proportion of non-spilled fish passing through sluiceway */
qIdcSluicewayProp::qIdcSluicewayProp() : qIdcSelectBase( ID_SLUICEWAYPROP ) {
	setHelpPage( 69 );
}

void qIdcSluicewayProp::Setup(void) {
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "mean" );
		insertColumnLabel( "low" );
		insertColumnLabel( "high" );

		insertRowLabel( "Prop. of non-spilled fish through sluiceway" );

		/* Set up feature and species lists. Base class lists dams - since
		 *  we want powerhouses, we must duplicate the base class code here.
		 */
		int i;
		for (i = 0; i < num_specs; i++)
			insertSpecies( specs[i].name );
		for (i = 0; i < num_powerhouses; i++)
			insertFeature( phouses[i]->seg_name );
	}
}

int qIdcSluicewayProp::FillData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		return 0;

	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return 0;

	FillBrokenStickData(dat, 0, &seg->current->dam.species[s].sluiceway_prop);
	
	dat->clearDirty();
	return 1;
}

void qIdcSluicewayProp::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int s;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		seg = getRiverSegmentHandle( dat->getString(0), POWERHOUSE_SEG );
	if (seg == NULL)
		return;

	for(s=0;s<num_specs;s++)
		if(!strcmp(spec_names[s], dat->getString(1)))
			break;
	if (s >= num_specs)
		return;

	ApplyBrokenStickData(dat, 0, &seg->current->dam.species[s].sluiceway_prop, NULL);

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcSluicewayProp qIdcSluicewayProp_;

/* ******************************************************************** */

/* Transport start/stop dates */
qIdcTransportDates::qIdcTransportDates() : qIdcSelectBase( ID_TRANSPORTDATES ) {
	setHelpPage( 111 );
}

void qIdcTransportDates::Setup(void) {
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "Julian Day" );

		insertRowLabel( "Start Date" );
		insertRowLabel( "Stop Date" );

		/* Set up feature and species lists. Base class lists species - since
		 *  we don't want species, we must duplicate the base class code here.
		 */
		int i;
		insertSpecies( NULL );
		for(i=0;i<num_dams;i++)
			insertFeature(dams[i]->seg_name);
	}
}

int qIdcTransportDates::FillData(qDatContainer *dat) {
	river_segment *seg;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		return 0;

	/* Fill in default values if there is no transport structure at that dam */
	if (seg->current->dam.transport) {
		dat->setDouble(IDC_TRANSPORT_DATES_START, seg->current->dam.transport->start_date);
		dat->setDouble(IDC_TRANSPORT_DATES_STOP, seg->current->dam.transport->end_date);
	}
	else {
		dat->setDouble(IDC_TRANSPORT_DATES_START, 0);
		dat->setDouble(IDC_TRANSPORT_DATES_STOP, 0);
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcTransportDates::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	
	seg = getRiverSegmentHandle( dat->getString(0), DAM_SEG );
	if (seg == NULL)
		return;

	/* If stop <= start, then we're not doing transport */
	if (dat->getDouble(IDC_TRANSPORT_DATES_STOP) <= dat->getDouble(IDC_TRANSPORT_DATES_START)) {
		if (dat->getDouble(IDC_TRANSPORT_DATES_STOP) < dat->getDouble(IDC_TRANSPORT_DATES_START)) {
			log_msg(L_WARNING, "Stop date is earlier than start date for transport at %s. Disabling transport\n", seg->seg_name);
		}

		/* Remove transport if it exists */
		if (seg->current->dam.transport) {
			free(seg->current->dam.transport);
			seg->current->dam.transport = NULL;
		}

		dat->setDouble(IDC_TRANSPORT_DATES_START, 0);
		dat->setDouble(IDC_TRANSPORT_DATES_STOP, 0);
	}
	else {
		/* Doing transport */

		/* Make sure the transport structure exists */
		if (!seg->current->dam.transport) {
			seg->current->dam.transport = (struct transport*) malloc(sizeof(struct transport));
			bzero(seg->current->dam.transport, sizeof(struct transport));

			seg->current->dam.transport->target_seg = default_transport_target;

			/* Set default values for features that we don't want to use */
			/* TODO: find out if these features are no longer needed and remove them */
			seg->current->dam.transport->hfl_spec = &specs[0];
			seg->current->dam.transport->end_count = 1000000000;
			seg->current->dam.transport->num_low_days = DAYS_IN_SEASON;

			/* The speed at which the transports move is here... 100 miles/day is
			 * the default, like in the transport tool. */
			seg->current->dam.transport->rate = 100.0f;
		}

		/* Apply data here */
		struct transport *trans = seg->current->dam.transport;

		trans->start_by_date = 1;
		trans->start_date = (int) dat->getDouble(IDC_TRANSPORT_DATES_START);
		trans->end_date = (int) dat->getDouble(IDC_TRANSPORT_DATES_STOP);
		trans->source_seg = seg;
	}

	dat->clearDirty();
	qIdcSelectBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcTransportDates qIdcTransportDates_;
