//#include <zapp.hpp>
#include "qCheckBoxGroup.h"
#include "qDamOutputCBDMedBase.h"

/* This virtual routine is called when a box is checked or unchecked. It handles
 *  any interactions with other check boxes. This derived class makes the
 *  last column act like a modified radio button group (modified in that
 *  no buttons checked is also a legitimate state). The first three columns
 *  do not interact. The first row and the next-to-last column are
 *  labeled "all" and they check or uncheck the entire row or column they
 *  are in, respectively (except for the last column).
 */
int qDamOutputCBDMedBase::doBoxInteractions( int row, int column, bool check ) {
	qDatContainer *dat;
	int i, j, nc, nr;

	nr = idcListBase_->getNumRow();
	nc = idcListBase_->getNumColumn();
	dat = idcListBase_->GetData();

	if (column == nc - 1) {
		/* Box has been checked - uncheck all other boxes in column.
		 */
		if (check)
			for (i = 0; i < nr; i++)
				if (i != row)
					dat->setDouble( i * nc + column, 0 );
	}
	else {
		/* "all" row or column? (or both)?
		 */
		if (row == 0 && column == nc - 2) {
			/* Checked box is in "all" row and "all" column - check or
			 *  uncheck all other boxes except for last column
			 */
			for (i = 1; i < nr; i++)
				for (j = 0; j < nc - 2; j++)
					dat->setDouble( i * nc + j, check );
		}
		else if (row == 0) {
			/* Checked box is in the "all" row.
			 */
			for (i = 1; i < nr; i++)
				dat->setDouble( i * nc + column, check );
		}
		else if (column == nc - 2) {
			/* Checked box is in the "all" column.
			 */
			for (j = 0; j < nc - 2; j++)
				dat->setDouble( row * nc + j, check );
		}
	}

	return 0;
}

