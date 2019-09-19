//#include <zapp.hpp>
#include "qCheckBoxGroup.h"
#include "qHeadwOutputCBDMedBase.h"

/* This virtual routine is called when a box is checked or unchecked. It handles
 *  any interactions with other check boxes.  The first row is labeled
 *  "all" and they check or uncheck the entire column they are in.
 */
int qHeadwOutputCBDMedBase::doBoxInteractions( int row, int column, bool check ) {
	qDatContainer *dat;
	int i, nc, nr;

	nr = idcListBase_->getNumRow();
	nc = idcListBase_->getNumColumn();
	dat = idcListBase_->GetData();

	/* "all" row?
	 */
	if (row == 0) {
		/* Checked box is in the "all" row.
		 */
		for (i = 1; i < nr; i++)
			dat->setDouble( i * nc + column, check );
	}

	return 0;
}

