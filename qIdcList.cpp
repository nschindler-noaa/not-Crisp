/* qIdcList.cpp   Derived classes for interface data controllers which control
 *  a single list of values, selected by a single index (they may be
 *  logically organized into rows and columns, in which case all of the
 *  first row is stored, then all of the second row, etc).
 */

#include "qIdcList.h"
#include "crisp.h"
#include "resource.h"
#include "output_flags.h"
#include "qIdcDamDelayEquation.h"
#include "qIdcEditGraph.h"

/* River parameters (a grab bag of global, single values).
 */
qIdcRiverParameters::qIdcRiverParameters() : qIdcListBase( ID_UNREGMAX ) {
	setHelpPage( 46 );
}

void qIdcRiverParameters::Setup(void) {
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "river params" );

		insertRowLabel( "gas dissp exp (*10^-4 cm^2/sec)" );
		insertRowLabel( "Ufree (kfs *10^-1)" );
        insertRowLabel( "fork threshold" );
	}
}

int qIdcRiverParameters::FillData(qDatContainer *dat) {
	
	dat->setDouble( 0, current->gas_dissp_exp, 0 );
	dat->setDouble( 1, current->Ufree, 0 );
    dat->setDouble( 2, current->fork_threshold, 0 );

	dat->clearDirty();
	return 1;
}

void qIdcRiverParameters::ApplyData(qDatContainer *dat) {

	current->gas_dissp_exp = dat->getDouble( 0 );
	current->Ufree = dat->getDouble( 1 );
    current->fork_threshold = dat->getDouble( 2 );

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcRiverParameters qIdcRiverParameters_;

/* ********************************************************************* */

/* A single value which modulates all headwater flows (!).
 */
qIdcHeadwaterModulation::qIdcHeadwaterModulation() : qIdcListBase( ID_HEADWATERMOD ) {
	setHelpPage( 47 );
}

void qIdcHeadwaterModulation::Setup(void) {
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "headwater modulation" );
		insertRowLabel( "proportion of mean flow" );
	}
}

int qIdcHeadwaterModulation::FillData(qDatContainer *dat) {
	
	dat->setDouble( 0, current->flow_proportion );
	
	dat->clearDirty();
	return 1;
}

extern "C" {
        void modulate_unreg_headwaters( void );
};

void qIdcHeadwaterModulation::ApplyData(qDatContainer *dat) {

    if (dat->isDirty()) {
        current->flow_proportion = dat->getDouble( 0 );
	    modulate_unreg_headwaters();
	    qIdcegHeadwaterFlow_.ClearCache();
	    dat->clearDirty();
	    qIdcListBase::ApplyData( dat );
    }
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcHeadwaterModulation qIdcHeadwaterModulation_;

/* ********************************************************************* */

/* Unregulated headwater flow maximums
 */
qIdcHdwUnregMaxFlow::qIdcHdwUnregMaxFlow() : qIdcListBase( ID_UNREGMAX ) {
	setHelpPage( 48 );
}

void qIdcHdwUnregMaxFlow::Setup(void) {
	river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "maximum unregulated flow (kcfs)" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == HEADWATER_SEG &&
						seg->current->headwater.flow_coefficient != 0) {
				insertRowLabel( seg->seg_name );
			}
		}
	}
}

int qIdcHdwUnregMaxFlow::FillData(qDatContainer *dat) {
	river_segment *seg;
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), HEADWATER_SEG );
		dat->setDouble( i, seg->current->headwater.flow_max );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcHdwUnregMaxFlow::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int i;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), HEADWATER_SEG );
		seg->current->headwater.flow_max = dat->getDouble( i );
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcHdwUnregMaxFlow qIdcHdwUnregMaxFlow_;

/* ********************************************************************* */

/* Reach predator distribution coefficients (volume interaction).
 */
qIdcPredDistCoef::qIdcPredDistCoef() : qIdcListBase( ID_PREDDISTCOEF ) {
	setHelpPage( 90 );
}

void qIdcPredDistCoef::Setup(void) {
	river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "coefficient" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == REACH_SEG) {
				insertRowLabel( seg->seg_name );
			}
		}
	}
}

int qIdcPredDistCoef::FillData(qDatContainer *dat) {
	river_segment *seg;
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), REACH_SEG );
		dat->setDouble( i, seg->current->reach.pred_dist_coef );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcPredDistCoef::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int i;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), REACH_SEG );
		seg->current->reach.pred_dist_coef = (float)(dat->getDouble( i ));
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcPredDistCoef qIdcPredDistCoef_;

/* ********************************************************************* */

/* Tailrace predation coefficients.
 */
qIdcTailracePredCoeff::qIdcTailracePredCoeff() : qIdcListBase( ID_PREDATIONCOEF_TAILRACE ) {
	setHelpPage( 53 );
}

void qIdcTailracePredCoeff::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "mean (x10^-7)" );
		insertColumnLabel( "low (x10^-7)" );
		insertColumnLabel( "high (x10^-7)" );

		for (i = 0; i < num_specs; i++) {
			insertRowLabel( specs[i].name );
		}
	}
}

int qIdcTailracePredCoeff::FillData(qDatContainer *dat) {
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		FillBrokenStickData(dat, i*3, &specs[i].tailrace_pcoef);
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcTailracePredCoeff::ApplyData(qDatContainer *dat) {
	int i;

	for (i = 0; i < getNumRow(); i++) {
		ApplyBrokenStickData(dat, i*3, &specs[i].tailrace_pcoef);
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcTailracePredCoeff qIdcTailracePredCoeff_;

/* ********************************************************************* */

/* Forebay predation coefficients.
 */
qIdcForebayPredCoeff::qIdcForebayPredCoeff() : qIdcListBase( ID_PREDATIONCOEF_FOREBAY ) {
	setHelpPage( 54 );
}

void qIdcForebayPredCoeff::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "mean (x10^-7)" );
		insertColumnLabel( "low (x10^-7)" );
		insertColumnLabel( "high (x10^-7)" );

		for (i = 0; i < num_specs; i++) {
			insertRowLabel( specs[i].name );
		}
	}
}

int qIdcForebayPredCoeff::FillData(qDatContainer *dat) {
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		FillBrokenStickData(dat, i*3, &specs[i].forebay_pcoef);
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcForebayPredCoeff::ApplyData(qDatContainer *dat) {
	int i;

	for (i = 0; i < getNumRow(); i++) {
		ApplyBrokenStickData(dat, i*3, &specs[i].forebay_pcoef);
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcForebayPredCoeff qIdcForebayPredCoeff_;

/* ********************************************************************* */

/* Inriver latent mort.
 */
qIdcInriverLatentMort::qIdcInriverLatentMort() : qIdcListBase( ID_INRIVER_LATENT_MORT ) {
	setHelpPage( 54 );
}

void qIdcInriverLatentMort::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "mean (x10^-7)" );
		insertColumnLabel( "low (x10^-7)" );
		insertColumnLabel( "high (x10^-7)" );

		for (i = 0; i < num_specs; i++) {
			insertRowLabel( specs[i].name );
		}
	}
}

int qIdcInriverLatentMort::FillData(qDatContainer *dat) {
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		FillBrokenStickData(dat, i*3, &specs[i].inriver_latent_mort);
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcInriverLatentMort::ApplyData(qDatContainer *dat) {
	int i;

	for (i = 0; i < getNumRow(); i++) {
		ApplyBrokenStickData(dat, i*3, &specs[i].inriver_latent_mort);
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcInriverLatentMort qIdcInriverLatentMort_;

/* ********************************************************************* */

/* Transport latent mort.
 */
qIdcTransportLatentMort::qIdcTransportLatentMort() : qIdcListBase( ID_TRANSPORT_LATENT_MORT ) {
	setHelpPage( 54 );
}

void qIdcTransportLatentMort::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "mean (x10^-7)" );
		insertColumnLabel( "low (x10^-7)" );
		insertColumnLabel( "high (x10^-7)" );

		for (i = 0; i < num_specs; i++) {
			insertRowLabel( specs[i].name );
		}
	}
}

int qIdcTransportLatentMort::FillData(qDatContainer *dat) {
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		FillBrokenStickData(dat, i*3, &specs[i].transport_latent_mort);
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcTransportLatentMort::ApplyData(qDatContainer *dat) {
	int i;

	for (i = 0; i < getNumRow(); i++) {
		ApplyBrokenStickData(dat, i*3, &specs[i].transport_latent_mort);
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcTransportLatentMort qIdcTransportLatentMort_;

/* ********************************************************************* */

/* Differential return
 */
qIdcDifferentialReturn::qIdcDifferentialReturn() : qIdcListBase( ID_DIFFERENTIAL_RETURN ) {
	setHelpPage( 54 );
}

void qIdcDifferentialReturn::Setup(void) {
	int i;

	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "mean (x10^-7)" );
		insertColumnLabel( "low (x10^-7)" );
		insertColumnLabel( "high (x10^-7)" );

		for (i = 0; i < num_specs; i++) {
			insertRowLabel( specs[i].name );
		}
	}
}

int qIdcDifferentialReturn::FillData(qDatContainer *dat) {
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		FillBrokenStickData(dat, i*3, &specs[i].differential_return);
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcDifferentialReturn::ApplyData(qDatContainer *dat) {
	int i;

	for (i = 0; i < getNumRow(); i++) {
		ApplyBrokenStickData(dat, i*3, &specs[i].differential_return);
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcDifferentialReturn qIdcDifferentialReturn_;


/* ********************************************************************* */

/* Dam spill capacity.
 */
qIdcDamSpillCap::qIdcDamSpillCap() : qIdcListBase( ID_SPILLCAP ) {
	setHelpPage( 57 );
}

void qIdcDamSpillCap::Setup(void) {
	river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "spill cap (kcfs)" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == DAM_SEG) {
				insertRowLabel( seg->seg_name );
			}
		}
	}
}

int qIdcDamSpillCap::FillData(qDatContainer *dat) {
	river_segment *seg;
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		dat->setDouble( i, seg->current->dam.spill_cap );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcDamSpillCap::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int i;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		seg->current->dam.spill_cap = (float)(dat->getDouble( i ));
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcDamSpillCap qIdcDamSpillCap_;

/* ********************************************************************* */

/* Dam tailrace lentgh.
 */
qIdcDamTailraceLength::qIdcDamTailraceLength() : qIdcListBase( ID_TAILRACELENGTH ) {
	setHelpPage( 59 );
}

void qIdcDamTailraceLength::Setup(void) {
	river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "length of tailrace (ft)" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == DAM_SEG) {
				insertRowLabel( seg->seg_name );
			}
		}
	}
}

int qIdcDamTailraceLength::FillData(qDatContainer *dat) {
	river_segment *seg;
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		dat->setDouble( i, seg->current->dam.tailrace_length );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcDamTailraceLength::ApplyData(qDatContainer *dat) {
	river_segment *seg;
	int i;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		seg->current->dam.tailrace_length = (float)(dat->getDouble( i ));
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcDamTailraceLength qIdcDamTailraceLength_;

/* ********************************************************************* */

/* Powerhouse capacities.
 */
qIdcPhouseCap::qIdcPhouseCap() : qIdcListBase( ID_PH_CAPACITY ) {
	setHelpPage( 60 );
}

void qIdcPhouseCap::Setup(void) {
	int i;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "powerhouse capacity (kcfs)" );

		for (i = 0; i < num_powerhouses; i++)		
			insertRowLabel( phouses[i]->seg_name );
	}
}

int qIdcPhouseCap::FillData(qDatContainer *dat) {
	int i;
	
	for (i = 0; i < getNumRow(); i++) {
		dat->setDouble( i, phouses[i]->current->dam.powerhouse_capacity );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcPhouseCap::ApplyData(qDatContainer *dat) {
	int i;

	for (i = 0; i < getNumRow(); i++) {
		phouses[i]->current->dam.powerhouse_capacity
								= (float)(dat->getDouble( i ));
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcPhouseCap qIdcPhouseCap_;

/* ********************************************************************* */

/* Powerhouse spill thresholds.
 */
qIdcPhouseFlowMin::qIdcPhouseFlowMin() : qIdcListBase( ID_PH_FLOW_MIN ) {
	setHelpPage( 61 );
}

void qIdcPhouseFlowMin::Setup(void) {
	struct river_segment *seg, *ph;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "powerhouse flow minimum (kcfs)" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == DAM_SEG) {
				for (ph = seg; ph; 
								ph = ph->current->dam.additional_pwrhs)
					insertRowLabel( ph->seg_name );
			}
		}
	}
}

int qIdcPhouseFlowMin::FillData(qDatContainer *dat) {
	int i = 0;
	struct river_segment *seg, *ph;
	
	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == DAM_SEG) {
			for (ph = seg; ph; 
							ph = ph->current->dam.additional_pwrhs)
				dat->setDouble( i++, ph->current->dam.flow_min_project );
		}
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcPhouseFlowMin::ApplyData(qDatContainer *dat) {
	int i = 0;
	struct river_segment *seg, *ph;

	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == DAM_SEG) {
			for (ph = seg; ph; 
							ph = ph->current->dam.additional_pwrhs)
				ph->current->dam.flow_min_project
								= (float)(dat->getDouble( i++ ));
		}
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcPhouseFlowMin qIdcPhouseFlowMin_;

/* ********************************************************************* */

/* Powerhouse priority is an integer value for each
 *  powerhouse (at dams which have more than one). Stored
 *  in the double values - round to nearest.
 * NOTE we assume that two powerhouses per dam is the maximum.
 */
qIdcPhousePriority::qIdcPhousePriority() : qIdcListBase( ID_PH_PRIORITY ) {
	setHelpPage( 62 );
}

void qIdcPhousePriority::Setup(void) {
	struct river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "house 1" );
		insertColumnLabel( "house 2" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == DAM_SEG 
				      && seg->current->dam.additional_pwrhs != NULL) 
				insertRowLabel( seg->seg_name );
		}
	}
}

/* NOTE: sense of priority is reversed for display to user - "0" is highest
 *  priority and results in a checked box in the dialog. "1" is lowest priority
 *  and results in an unchecked box. In the qDatContainer, a 0 value is an
 *  unchecked box and a 1 value is a checked box.
 */
int qIdcPhousePriority::FillData(qDatContainer *dat) {
	int i = 0;
	struct river_segment *seg, *ph;
	
	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == DAM_SEG 
					&& seg->current->dam.additional_pwrhs != NULL) {
			ph = seg->current->dam.additional_pwrhs;
			dat->setDouble( i*2, 1 - seg->current->dam.phouse_priority );
			dat->setDouble( i*2+1, 1 - ph->current->dam.phouse_priority );
			i++;
		}
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcPhousePriority::ApplyData(qDatContainer *dat) {
	int i = 0;
	struct river_segment *seg, *ph;

	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == DAM_SEG 
					&& seg->current->dam.additional_pwrhs != NULL) {
			ph = seg->current->dam.additional_pwrhs;
			seg->current->dam.phouse_priority
								= 1 - (int)(dat->getDouble( i*2 ) + 0.5);
			ph->current->dam.phouse_priority
								= 1 - (int)(dat->getDouble( i*2+1 ) + 0.5);
			i++;
		}
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcPhousePriority qIdcPhousePriority_;

/* ********************************************************************* */

/* Dam output settings include whether to track passage, transport, and
 *  routing for each dam, whether to dump flow, water temp, NSat, and
 *  spill for each dam, and which dam will serve as the flow indicator.
 *  There is also a row and a column labeled "all" - checking or unchecking
 *  an "all" box checks or unchecks that entire row or column. */
qIdcDamOutputSetting::qIdcDamOutputSetting() : qIdcListBase( ID_DAM_OUTPUTSETTING ) {
	setHelpPage( 65 );
}

void qIdcDamOutputSetting::Setup(void) {
	struct river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "Passage" );
		insertColumnLabel( "Transport" );
		insertColumnLabel( "Routing" );
		insertColumnLabel( "Flow" );
		insertColumnLabel( "WTemp" );
		insertColumnLabel( "TDG Sat" );
		insertColumnLabel( "Spill" );
		insertColumnLabel( "Day Spill" );
		insertColumnLabel( "Night Spill" );
		insertColumnLabel( "all" );
		insertColumnLabel( "Flow Ind." );

		insertRowLabel( "all" );
		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == DAM_SEG) 
				insertRowLabel( seg->seg_name );
		}
	}
}

int qIdcDamOutputSetting::FillData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;
	
	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == DAM_SEG) {
			dat->setDouble( i*11, (seg->output_settings
											& PASSAGE_FLAG) != 0 );
			dat->setDouble( i*11+1, (seg->output_settings
											& TRANSPORT_FLAG) != 0 );
			dat->setDouble( i*11+2, (seg->output_settings
											& ROUTING_FLAG) != 0 );
			dat->setDouble( i*11+3, (seg->output_settings
											& FLOW_FLAG) != 0 );
			dat->setDouble( i*11+4, (seg->output_settings
											& WATERTEMP_FLAG) != 0 );
			dat->setDouble( i*11+5, (seg->output_settings
											& NSAT_FLAG) != 0 );
			dat->setDouble( i*11+6, (seg->output_settings
											& SPILL_FLAG) != 0 );
			dat->setDouble( i*11+7, (seg->output_settings
											& DAY_SPILL_FLAG) != 0 );
			dat->setDouble( i*11+8, (seg->output_settings
											& NIGHT_SPILL_FLAG) != 0 );

			dat->setDouble( i*11+10, current->flow_seg == seg );
			i++;
		}
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcDamOutputSetting::ApplyData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;

	current->flow_seg = NULL;
	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == DAM_SEG) {
			seg->output_settings = 
				(dat->getDouble( i*11 ) > 0.5 ? PASSAGE_FLAG : 0 ) |
				(dat->getDouble( i*11+1 ) > 0.5 ? TRANSPORT_FLAG : 0 ) |
				(dat->getDouble( i*11+2 ) > 0.5 ? ROUTING_FLAG : 0 ) |
				(dat->getDouble( i*11+3 ) > 0.5 ? FLOW_FLAG : 0 ) |
				(dat->getDouble( i*11+4 ) > 0.5 ? WATERTEMP_FLAG : 0 ) |
				(dat->getDouble( i*11+5 ) > 0.5 ? NSAT_FLAG : 0 ) |
				(dat->getDouble( i*11+6 ) > 0.5 ? SPILL_FLAG : 0 ) |
				(dat->getDouble( i*11+7 ) > 0.5 ? DAY_SPILL_FLAG : 0 ) |
				(dat->getDouble( i*11+8 ) > 0.5 ? NIGHT_SPILL_FLAG : 0 );
			if (dat->getDouble( i*11+10) > 0.5)
				current->flow_seg = seg;
			i++;
		}
	}

	dat->clearDirty();
	// KLUDGE - don't mark outputs stale:  qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcDamOutputSetting qIdcDamOutputSetting_;

/* ********************************************************************* */

/* Reach output settings.
 */
qIdcReachOutputSetting::qIdcReachOutputSetting() : qIdcListBase( ID_REACH_OUTPUTSETTING ) {
	setHelpPage( 65 );
}

void qIdcReachOutputSetting::Setup(void) {
	struct river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "Passage" );
        insertColumnLabel( "Length" );
		insertColumnLabel( "Flow" );
		insertColumnLabel( "WTemp" );
		insertColumnLabel( "TDG Sat" );
		insertColumnLabel( "Velocity" );
		insertColumnLabel( "Loss" );
		insertColumnLabel( "Elevation" );
		insertColumnLabel( "all" );

		insertRowLabel( "all" );
		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == REACH_SEG) 
				insertRowLabel( seg->seg_name );
		}
	}
}

int qIdcReachOutputSetting::FillData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;
	
	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == REACH_SEG) {
			dat->setDouble( i*9, (seg->output_settings
											& PASSAGE_FLAG) != 0 );
			dat->setDouble( i*9+1, (seg->output_settings
											& LENGTH_FLAG) != 0 );
			dat->setDouble( i*9+2, (seg->output_settings
											& FLOW_FLAG) != 0 );
			dat->setDouble( i*9+3, (seg->output_settings
											& WATERTEMP_FLAG) != 0 );
			dat->setDouble( i*9+4, (seg->output_settings
											& NSAT_FLAG) != 0 );
			dat->setDouble( i*9+5, (seg->output_settings
											& VELOCITY_FLAG) != 0 );
			dat->setDouble( i*9+6, (seg->output_settings
											& LOSS_FLAG) != 0 );
			dat->setDouble( i*9+7, (seg->output_settings
											& ELEVATION_FLAG) != 0 );
			i++;
		}
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcReachOutputSetting::ApplyData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;

	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == REACH_SEG) {
			seg->output_settings = 
				(dat->getDouble( i*9 ) > 0.5 ? PASSAGE_FLAG : 0 ) |
				(dat->getDouble( i*9+1 ) > 0.5 ? LENGTH_FLAG : 0 ) |
				(dat->getDouble( i*9+2 ) > 0.5 ? FLOW_FLAG : 0 ) |
				(dat->getDouble( i*9+3 ) > 0.5 ? WATERTEMP_FLAG : 0 ) |
				(dat->getDouble( i*9+4 ) > 0.5 ? NSAT_FLAG : 0 ) |
				(dat->getDouble( i*9+5 ) > 0.5 ? VELOCITY_FLAG : 0 ) |
				(dat->getDouble( i*9+6 ) > 0.5 ? LOSS_FLAG : 0 ) |
				(dat->getDouble( i*9+7 ) > 0.5 ? ELEVATION_FLAG : 0 );
			i++;
		}
	}

	dat->clearDirty();
	// KLUDGE don't mark outputs stale:  qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcReachOutputSetting qIdcReachOutputSetting_;

/* ********************************************************************* */

/* Reach class settings.
 */
qIdcReachClassSetting::qIdcReachClassSetting() : qIdcListBase( ID_REACH_CLASSES ) {
	setHelpPage( 96 );
}

void qIdcReachClassSetting::cbPurge( void ) {
    clearLabels();
    qIdcListBase::cbPurge();
}

void qIdcReachClassSetting::Setup(void) {
	struct river_segment *seg;
    int i;
	
	/* Since the number of labels can change, we must reset the labels each time! */
	clearLabels();

    nrc = num_reach_classes;  /* remember this for validation */

    for (i = 0; i < num_reach_classes; i++) {
        insertColumnLabel( reach_class_names[i] );
    }

	insertRowLabel( "all" );
	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == REACH_SEG) 
			insertRowLabel( seg->seg_name );
	}
}

int qIdcReachClassSetting::FillData(qDatContainer *dat) {
	int j, i = 1;
	struct river_segment *seg;
	
    if (nrc != num_reach_classes)
        return 1;  /* someone pulled the rug out from under us. */
	if (getNumRow() == 0)
        return 1;  /* ditto */

	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == REACH_SEG) {
            for (j = 0; j < num_reach_classes; j++) {
    			dat->setDouble( i * num_reach_classes + j,
                                 seg->current->reach.rclass == j );
            }
			i++;
		}
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcReachClassSetting::ApplyData(qDatContainer *dat) {
	int j, i = 1;
	struct river_segment *seg;

    if (nrc != num_reach_classes)
        return;  /* someone pulled the rug out from under us. */
	if (getNumRow() == 0)
        return;  /* ditto */

	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == REACH_SEG) {
            for (j = 0; j < num_reach_classes; j++) {
                if (dat->getDouble( i * num_reach_classes + j ) > 0.5)
                    seg->current->reach.rclass = j;
            }
			i++;
		}
	}

	dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcReachClassSetting qIdcReachClassSetting_;

/* ********************************************************************* */

/* Headwater output settings (flow and water temperature).
 */
qIdcHeadwOutputSetting::qIdcHeadwOutputSetting() : qIdcListBase( ID_HEADW_OUTPUTSETTING ) {
	setHelpPage( 65 );
}

void qIdcHeadwOutputSetting::Setup(void) {
	struct river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "Flow" );
		insertColumnLabel( "WTemp" );

		insertRowLabel( "all" );
		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == HEADWATER_SEG) 
				insertRowLabel( seg->seg_name );
		}
	}
}

int qIdcHeadwOutputSetting::FillData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;
	
	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == HEADWATER_SEG) {
			dat->setDouble( i*2, (seg->output_settings
											& FLOW_FLAG) != 0 );
			dat->setDouble( i*2+1, (seg->output_settings
											& WATERTEMP_FLAG) != 0 );
			i++;
		}
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcHeadwOutputSetting::ApplyData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;

	for (seg = river_mouth; seg != NULL; seg = seg->next) {
		if (seg->seg_type == HEADWATER_SEG) {
			seg->output_settings = 
				(dat->getDouble( i*2 ) > 0.5 ? FLOW_FLAG : 0 ) |
				(dat->getDouble( i*2+1 ) > 0.5 ? WATERTEMP_FLAG : 0 );
			i++;
		}
	}

	dat->clearDirty();
	// KLUDGE don't mark outputs stale:  qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcHeadwOutputSetting qIdcHeadwOutputSetting_;

/* ********************************************************************* */

/* Gas Theta parameter for dams
 */
qIdcDamGasTheta::qIdcDamGasTheta() : qIdcListBase( ID_DAM_GAS_THETA ) {
	setHelpPage( 92 );
}

void qIdcDamGasTheta::Setup(void) {
	struct river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "Gas Theta" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == DAM_SEG) 
				insertRowLabel( seg->seg_name );
		}
	}
}

int qIdcDamGasTheta::FillData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;
	
	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		dat->setDouble( i, seg->current->dam.gas_theta );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcDamGasTheta::ApplyData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		seg->current->dam.gas_theta = (float)(dat->getDouble(i));
	}

    dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcDamGasTheta qIdcDamGasTheta_;

/* ********************************************************************* */

/* K Entrain parameter for dams
 */
qIdcDamKEntrain::qIdcDamKEntrain() : qIdcListBase( ID_K_ENTRAIN ) {
	setHelpPage( 86 );
}

void qIdcDamKEntrain::Setup(void) {
	struct river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "K Entrain" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == DAM_SEG) 
				insertRowLabel( seg->seg_name );
		}
	}
}

int qIdcDamKEntrain::FillData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;
	
	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		dat->setDouble( i, seg->current->dam.k_entrain );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcDamKEntrain::ApplyData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), DAM_SEG );
		seg->current->dam.k_entrain = (float)(dat->getDouble(i));
	}

    dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcDamKEntrain qIdcDamKEntrain_;

/* ********************************************************************* */

/* Gas Theta parameter for reaches
 */
qIdcReachGasTheta::qIdcReachGasTheta() : qIdcListBase( ID_REACH_GAS_THETA ) {
	setHelpPage( 85 );
}

void qIdcReachGasTheta::Setup(void) {
	struct river_segment *seg;
	
	if (getNumRow() == 0) {
		/* We must set up the row and column labels on the first call.
		 *  This is because the underlying database will not be set up
		 *  properly when static objects are constructed.
		 */
		insertColumnLabel( "Gas Theta" );

		for (seg = river_mouth; seg != NULL; seg = seg->next) {
			if (seg->seg_type == REACH_SEG) 
				insertRowLabel( seg->seg_name );
		}
	}
}

int qIdcReachGasTheta::FillData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;
	
	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), REACH_SEG );
		dat->setDouble( i, seg->current->reach.gas_theta );
	}
	
	dat->clearDirty();
	return 1;
}

void qIdcReachGasTheta::ApplyData(qDatContainer *dat) {
	int i = 1;
	struct river_segment *seg;

	for (i = 0; i < getNumRow(); i++) {
		seg = getRiverSegmentHandle( getRowLabel( i ), REACH_SEG );
		seg->current->reach.gas_theta = (float)(dat->getDouble(i));
	}

    dat->clearDirty();
	qIdcListBase::ApplyData( dat );
}

/* A single global, static object does the interfacing for this type of
 *  data.
 */
qIdcReachGasTheta qIdcReachGasTheta_;

/* ********************************************************************* */

