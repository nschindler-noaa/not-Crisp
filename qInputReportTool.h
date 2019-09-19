/* ZasInputReportTool.h
 * This class defines a modal dialog which edits transport structures
 *  via comboboxes, scrollbars, and edit boxes. The dialog is based on 
 *  the IDD_TRANSPORTTOOL resource template.
 *
 * This is a special-purpose dialog which will probably not be subclassed.
 */

#ifndef __QINPUTREPORTTOOL_H__
#define __QINPUTREPORTTOOL_H__

//#include <zapp.hpp>
//#include <rw/cstring.h>
//#include <rw/tpdlist.h>
#include "crisp.h"
#include "qDataReportTool.h"

class qInputReportTool : public qDataReportTool {

protected:


	static qInputReportTool *instance_;

public:

	static qInputReportTool *instance( QWidget *w = NULL );

	qInputReportTool( QWidget *w );
	~qInputReportTool();

};

#endif


