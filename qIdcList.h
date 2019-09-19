/* idcList.h   Derived classes for interface data controllers which control
 *  a single list of values, selected by a single index (they may be
 *  logically organized into rows and columns, in which case all of the
 *  first row is stored, then all of the second row, etc).
 */

#ifndef __qIdcList__
#define __qIdcList__

#include "qIdcListBase.h"

class qIdcRiverParameters : public qIdcListBase {

public:
	qIdcRiverParameters();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 0.4; }
};

extern qIdcRiverParameters qIdcRiverParameters_;

class qIdcHeadwaterModulation : public qIdcListBase {

public:
	qIdcHeadwaterModulation();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 2.0; }
};

extern qIdcHeadwaterModulation qIdcHeadwaterModulation_;

class qIdcHdwUnregMaxFlow : public qIdcListBase {

public:
	qIdcHdwUnregMaxFlow();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 400.0; }
};

extern qIdcHdwUnregMaxFlow qIdcHdwUnregMaxFlow_;

class qIdcPredDistCoef : public qIdcListBase {

public:
	qIdcPredDistCoef();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 10.0; }
};

extern qIdcPredDistCoef qIdcPredDistCoef_;

class qIdcTailracePredCoeff : public qIdcListBase {

public:
	qIdcTailracePredCoeff();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 200.0; }
};

extern qIdcTailracePredCoeff qIdcTailracePredCoeff_;

class qIdcForebayPredCoeff : public qIdcListBase {

public:
	qIdcForebayPredCoeff();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 200.0; }
};

extern qIdcForebayPredCoeff qIdcForebayPredCoeff_;

class qIdcInriverLatentMort : public qIdcListBase {

public:
	qIdcInriverLatentMort();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcInriverLatentMort qIdcInriverLatentMort_;

class qIdcTransportLatentMort : public qIdcListBase {

public:
	qIdcTransportLatentMort();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcTransportLatentMort qIdcTransportLatentMort_;

class qIdcDifferentialReturn : public qIdcListBase {

public:
	qIdcDifferentialReturn();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1000000000.0; }
};

extern qIdcDifferentialReturn qIdcDifferentialReturn_;

class qIdcDamSpillCap : public qIdcListBase {

public:
	qIdcDamSpillCap();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1000.0; }
};

extern qIdcDamSpillCap qIdcDamSpillCap_;

class qIdcDamTailraceLength : public qIdcListBase {

public:
	qIdcDamTailraceLength();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 10000.0; }
};

extern qIdcDamTailraceLength qIdcDamTailraceLength_;

class qIdcPhouseCap : public qIdcListBase {

public:
	qIdcPhouseCap();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1000.0; }
};

extern qIdcPhouseCap qIdcPhouseCap_;

class qIdcPhouseFlowMin : public qIdcListBase {

public:
	qIdcPhouseFlowMin();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 250.0; }
};

extern qIdcPhouseFlowMin qIdcPhouseFlowMin_;

// Powerhouse priority is an integer value for each
//  powerhouse (at dams which have more than one). Stored
//  in the double values - round to nearest.
class qIdcPhousePriority : public qIdcListBase {

public:
	qIdcPhousePriority();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcPhousePriority qIdcPhousePriority_;

// These classes handle the output settings for dams and reaches,
//  respectively.
class qIdcDamOutputSetting : public qIdcListBase {

public:
	qIdcDamOutputSetting();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcDamOutputSetting qIdcDamOutputSetting_;

class qIdcReachOutputSetting : public qIdcListBase {

public:
	qIdcReachOutputSetting();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcReachOutputSetting qIdcReachOutputSetting_;

class qIdcReachClassSetting : public qIdcListBase {
protected:
    int nrc;

public:
	qIdcReachClassSetting();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );
    virtual void cbPurge( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcReachClassSetting qIdcReachClassSetting_;

class qIdcHeadwOutputSetting : public qIdcListBase {

public:
	qIdcHeadwOutputSetting();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 1.0; }
};

extern qIdcHeadwOutputSetting qIdcHeadwOutputSetting_;

class qIdcDamGasTheta : public qIdcListBase {

public:
	qIdcDamGasTheta();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 10.0; }
};

extern qIdcDamGasTheta qIdcDamGasTheta_;

class qIdcReachGasTheta : public qIdcListBase {

public:
	qIdcReachGasTheta();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 10.0; }
};

extern qIdcReachGasTheta qIdcReachGasTheta_;

class qIdcDamKEntrain : public qIdcListBase {

public:
	qIdcDamKEntrain();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);

	virtual void Setup( void );

	// Allowed range of any value in the collection.
	virtual double minVal( void ) { return 0.0; }
	virtual double maxVal( void ) { return 10.0; }
};

extern qIdcDamKEntrain qIdcDamKEntrain_;

#endif //  __qIdcList__


