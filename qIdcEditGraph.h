/*
 * qIdcEditGraph.h
 * This file contains several classes derived from qIdcEditGraphBase.
 *  Each class defines an interface data container for a particular
 *  type of data in the data base. These are grouped together because
 *  each class is likely to be small, but there are many classes.
 */

#ifndef __QIDCEDITGRAPH_H__
#define __QIDCEDITGRAPH_H__

#include "qIdcEditGraphBase.h"

/* Class for headwater flows. */

class qIdcegHeadwaterFlow : public qIdcEditGraphBase {
public:
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
};

class qIdcegHeadwaterTemp : public qIdcEditGraphBase {
public:
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
};

class qIdcegHeadwaterTurbidity : public qIdcEditGraphBase {
public:
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
};


/* Class for storage basin volumes at dams which have such. */
class qIdcegBasinVolume : public qIdcEditGraphBase {
public:
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	void makeLegal( const char *id = NULL );
};

/* Class for flow at dams with storage basins (editable for UI reasons,
 *  but doesn't really effect model run results).
 */
class qIdcegEditDamFlow : public qIdcEditGraphBase {
public:
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	void calcFlow( const char *id = NULL );
	void calcVolume( const char *id = NULL );
};

/* Class for reach losses. */
class qIdcegFlowLoss : public qIdcEditGraphBase {
public:
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
};

/* Class for reach elevation changes. */
class qIdcegElevationChange : public qIdcEditGraphBase {
public:
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
};


/* Each type of IDC has a single, global, static instantiation. This
 *  way there is only a single cache for each type of data being interfaced.
 */
extern qIdcegHeadwaterFlow qIdcegHeadwaterFlow_;
extern qIdcegHeadwaterTemp qIdcegHeadwaterTemp_;
extern qIdcegHeadwaterTurbidity qIdcegHeadwaterTurbidity_;
extern qIdcegBasinVolume qIdcegBasinVolume_;
extern qIdcegEditDamFlow qIdcegEditDamFlow_;
extern qIdcegFlowLoss qIdcegFlowLoss_;
extern qIdcegElevationChange qIdcegElevationChange_;
#endif

