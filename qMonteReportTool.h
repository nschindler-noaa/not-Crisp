/* qInputReportTool.h
 * This class defines a modal dialog which edits transport structures
 *  via comboboxes, scrollbars, and edit boxes. The dialog is based on 
 *  the qDataReportTool dialog
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#ifndef __qMONTEREPORTTOOL_H__
#define __qMONTEREPORTTOOL_H__

//#include <zapp.hpp>
//#include <rw/cstring.h>
//#include <rw/tpdlist.h>
#include "crisp.h"
#include "qDataReportTool.h"

class qMonteReportTool : public qDataReportTool {
protected:


	static qMonteReportTool *instance_;

public:

	static qMonteReportTool *instance( QWidget *w = NULL );

	qMonteReportTool( QWidget *w );
	~qMonteReportTool();

    // Monte Carlo tool may prod us when the alternatives change on disk.
    void altsUpdated( void );

};

#endif


