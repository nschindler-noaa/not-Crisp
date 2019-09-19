#ifndef _qIdcEquationBase_
#define _qIdcEquationBase_

//#include <rw/tvdlist.h>
//#include <rw/cstring.h>
#include "qInterfaceDataControl.h"
#include "qDatContainer.h"
#include "eqns.h"
#include "qUtils.h"
#include "qGraphDataTokenTypes.h"

struct river_segment;

class qIdcEquationBase : public qInterfaceDataControl {  

protected:

	int iDialogID_;
	
	QValueList<int> eqTypes_;  /* Allowed types of equations (allowed "id" values). */
	int iMaxParams_; /* max # of params for any allowed equation type */

public:

	qIdcEquationBase(int iDialogID);
	
	/* Accesses list of allowed equation types for this IDC.
	 */
	int nEqTypes( void ) { return (int)(eqTypes_.count()); };
	int eqType( int i ) { return eqTypes_[i]; };
	int maxParams( void ) { return iMaxParams_; };
    int getDividerParam( int i ) { return eqn_dflt_tab[i].divider_param; };
	void addEqType( int i );

	/* Virtual functions to access the underlying database.
	 */
	virtual int GetEquationData(equation *eq, qDatContainer *dat);
	virtual void SetEquationData(equation *eq, qDatContainer *dat);
	virtual int ModifyCache(
		const int index,		// Position in the datContainer array
		const int flagFeature,	// (Ex: If the D check box is on, modify for all dams)
		const int flagSpecies,  // (Ex: If the S check box is on, modify for all species)
		const double dValue,    // The value returned 
		const char *strFeature, // Name of the dam
		const char *strSpecies, // Name of the species
		const int isDelta);		// If we're in Delta mode, dValue is the amount changed.

	virtual qDatContainer *GetData(const char *strFeature, const char *strSpecies);
	virtual qDatContainer *NewData();
	virtual int FillData(qDatContainer *dat);
	virtual void ApplyData(qDatContainer *dat);
	virtual const char *GetCaption(void){ return "Equation"; }

    /* This routine gets underlying data from the database, without reference
     *  to the cache. The caller is responsible for cleanup.
     */
	virtual qDatContainer *GetULData(const char *strFeature, const char *strSpecies);

    void SetDialogID(int id){ iDialogID_ = id; }
	int  GetDialogID(void){ return iDialogID_; }

	/* These virtual functions control whether the dam selection and
	 *  species selection boxes are actually displayed. By default, they
	 *  are. NOTE: in one case, the "dam" combobox actaully selects by
     *  reaches, in two other cases, by reach classes.
	 */
	virtual int selBySpecies( void ) { return 1; };
	virtual int selByFeature( void ) { return 1; };
	virtual int selByDam( void ) { return 1; };
	virtual int selByPowerhouse( void ) { return 0; };
	virtual int selByStock( void ) { return 0; };
	virtual int selByReach( void ) { return 0; };
	virtual int selByReachClass( void ) { return 0; };
};

#endif


