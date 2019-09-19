
#include "qCheckBoxGroup.h"
#include "qReachOutputCBDMedBase.h"

/* This virtual routine is called when a box is checked or unchecked. It handles
 *  any interactions with other check boxes.  The first row and the 
 *  next-to-last column are labeled "all" and they check or uncheck the 
 *  entire row or column theyare in, respectively.
 */
int qReachOutputCBDMedBase::doBoxInteractions( int row, int column, bool check ) {
	qDatContainer *dat;
	int i, j, nc, nr;

	nr = idcListBase_->getNumRow();
	nc = idcListBase_->getNumColumn();
	dat = idcListBase_->GetData();

	/* "all" row or column? (or both)?
	 */
	if (row == 0 && column == nc - 1) {
		/* Checked box is in "all" row and "all" column - check or
		 *  uncheck all other boxes except for last column
		 */
		for (i = 1; i < nr; i++)
			for (j = 0; j < nc - 1; j++)
				dat->setDouble( i * nc + j, check );
	}
	else if (row == 0) {
		/* Checked box is in the "all" row.
		 */
		for (i = 1; i < nr; i++)
			dat->setDouble( i * nc + column, check );
	}
	else if (column == nc - 1) {
		/* Checked box is in the "all" column.
		 */
		for (j = 0; j < nc - 1; j++)
			dat->setDouble( row * nc + j, check );
	}

	return 0;
}

