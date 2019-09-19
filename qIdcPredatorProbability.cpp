//Here is an example of an implimentation of a qIdc for a TimeBase dialog. 
//Time base dialogs are the ones with time segments listed 0000-0200 0200-0400 
//with zabel sliders and such.. there were a few of em that looked like this so I
//made a base class for em..

#include "crisp.h"
#include "qIdcPredatorProbability.h"

/* Single global, static object does all interfacing for this data.
 */
qIdcPredatorProbability qIdcPredatorProbability_( ID_PREDATIONPROB );

qIdcPredatorProbability::qIdcPredatorProbability(int iDialogID)
				 : qIdcTimeBase(iDialogID) {
	setHelpPage( 74 );

} 

qDatContainer *qIdcPredatorProbability::NewData(){
	return new qDatContainer(DAM_TIME_SLC,2,0);
}

void qIdcPredatorProbability::ApplyData(qDatContainer *dat){
	int i,j;
	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	if(riverSeg){
		for(i=0;i<num_specs;i++){
			if(!strcmp(spec_names[i], dat->getString(1))){
				for(j=0;j<DAM_TIME_SLC;j++)
					riverSeg->current->dam.species[i].pred[j] =
								dat->getDouble(j);

				dat->clearDirty();
				qIdcTimeBase::ApplyData( dat );
				break;
			}
		}
	}
}

int qIdcPredatorProbability::FillData(qDatContainer *dat){
	int i, j;
  	river_segment *riverSeg=NULL;
	riverSeg = getRiverSegmentHandle(dat->getString(0), DAM_SEG);
	if(riverSeg){
		for(i=0;i<num_specs;i++){
			if(!strcmp(spec_names[i], dat->getString(1))){
				for(j=0;j<DAM_TIME_SLC;j++)
					dat->setDouble(j, riverSeg->current->dam.species[i].pred[j]);

				dat->clearDirty();
				return 1;
			}
		}
	}
	return 0;

}



