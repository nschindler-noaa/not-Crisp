/* idcSelect.h   Derived classes for interface data controllers which control
 *  a set of lists of values, selected by a single index (they may be
 *  logically organized into rows and columns, in which case all of the
 *  first row is stored, then all of the second row, etc). The sets are
 *  selected by two strings (feature and species, although in principle they
 *  could be anything).
 */

#ifndef __qIdcSelect__
#define __qIdcSelect__

#include "qIdcSelectBase.h"

class qIdcReachPredCoeff : public qIdcSelectBase {
protected:
    int nrc;

public:
	qIdcReachPredCoeff();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );
    virtual void cbPurge( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 200.0; }
};

extern qIdcReachPredCoeff qIdcReachPredCoeff_;


class qIdcReachPredDensity: public qIdcSelectBase {

public:
	qIdcReachPredDensity();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 10000.0; }
};

extern qIdcReachPredDensity qIdcReachPredDensity_;

class qIdcPredMortCoef : public qIdcSelectBase {
protected:
    int nrc;

public:
	qIdcPredMortCoef();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );
    virtual void cbPurge( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 100000.0; }
};

extern qIdcPredMortCoef qIdcPredMortCoef_;

class qIdcVVAR : public qIdcSelectBase {
protected:
    int nrc;

public:
	qIdcVVAR();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );
    virtual void cbPurge( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 500.0; }
};

extern qIdcVVAR qIdcVVAR_;

class qIdcMigRateVar : public qIdcSelectBase {
protected:
    int nrc;

public:
	qIdcMigRateVar();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );
    virtual void cbPurge( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.1; }
	virtual double maxVal( void ) { return 2.0; }
};

extern qIdcMigRateVar qIdcMigRateVar_;

class qIdcDamPredDensity : public qIdcSelectBase {

public:
	qIdcDamPredDensity();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 40000.0; }
};

extern qIdcDamPredDensity qIdcDamPredDensity_;

/* Offsets into container / 3
 * It's divided by 3 since there are mean, low, and high values for each */
enum {
	IDC_DAM_MORT_OFF_RSW=0,
	IDC_DAM_MORT_OFF_SPILL,
	IDC_DAM_MORT_OFF_SLUICEWAY,
	IDC_DAM_MORT_OFF_BYPASS,
	IDC_DAM_MORT_OFF_TURBINE,
	IDC_DAM_MORT_OFF_TRANSPORT
};

class qIdcDamMortality : public qIdcSelectBase {

public:
	qIdcDamMortality();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcDamMortality qIdcDamMortality_;


class qIdcSeparationProb : public qIdcSelectBase {

public:
	qIdcSeparationProb();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcSeparationProb qIdcSeparationProb_;


/* Offsets into container / 3
 * It's divided by 3 since there are mean, low, and high values for each */
enum {
	IDC_DAM_FGE_DAY=0,
	IDC_DAM_FGE_NIGHT
};

class qIdcFGE : public qIdcSelectBase {

public:
	qIdcFGE();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcFGE qIdcFGE_;

class qIdcRSWProp : public qIdcSelectBase {
public:
	qIdcRSWProp();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcRSWProp qIdcRSWProp_;

class qIdcSluicewayProp : public qIdcSelectBase {

public:
	qIdcSluicewayProp();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcSluicewayProp qIdcSluicewayProp_;

/* Offsets into dat container */
enum {
	IDC_TRANSPORT_DATES_START = 0,
	IDC_TRANSPORT_DATES_STOP
};

class qIdcTransportDates : public qIdcSelectBase {

public:
	qIdcTransportDates();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 365.0; }
};

extern qIdcTransportDates qIdcTransportDates_;

#endif

