
#include "mapPoint.h"
#include "QPointTransform.h"


qPointTransform::qPointTransform(QRect rv, double x1, double y1, double x2, double y2, int PO)
	: _ox(x1), _oy(y1), _ProcessOrder(PO), _rv(rv) {
}

qPointTransform::qPointTransform(QRect rv, mapCordset *mpc, int PO)
	: _ox(mpc->X1()), _oy(mpc->Y1()), _ProcessOrder(PO), _rv(rv), _mpc(mpc) {
}

