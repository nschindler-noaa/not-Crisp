
#include "qRowColumnBox.h"
#include "log.h"



qRowColumnBox::qRowColumnBox(QWidget *w, QRect r, char *text, int baseId)
{
	consCommon( w );
}

qRowColumnBox::qRowColumnBox(QWidget *w, char *text, int baseId) 
{
	consCommon( w );
}

void qRowColumnBox::consCommon( QWidget *w) {

    wParent_ = w;
	rowPercent = NULL;
	colPercent = NULL;
}

qRowColumnBox::~qRowColumnBox(){

	rwList.clear();
   	if(numRows)
		delete rowPercent;
	if(numCols)
		delete colPercent;
}

void qRowColumnBox::setRows(int r, float *rp){
	rowPercent = rp;
	numRows = r;
}
void qRowColumnBox::setCols(int c, float *cp){
	colPercent = cp;
	numCols = c;
}
qItem *qRowColumnBox::insert(QWidget *w, int r, int c, int id, int groupId){ 
	qItem *zi = new qItem(w,r,c,id,groupId);
	rwList.append(zi);
	return zi;
}

