//This class is used to configure what the mouse does when something
//on the map gets selected.. It is explicitly activated through
//the dialogBuildRequest function from the ZatMapMedDialogBuilder map
//sub mediator.

#ifndef _qMouseTools_h_
#define _qMouseTools_h_


#include <qwidget.h>
#include <qdialog.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qvaluelist.h>
#include "mouse_tool_base.h"

const int NumCategories = 4;
const int NumButtons = 3;

const int onDam = 0;
const int onRelease = 1 ;
const int onReach =  2;
const int onHeadwater = 3;

const int lButton =  0;
const int mButton =  1;
const int rButton = 2;

class QmapObject;

class qMouseTool : public Mouse_Tool_Base {

	Q_OBJECT

	protected:
		QComboBox *OnButton[NumCategories][NumButtons];
		QWidget *w_;
		
		QValueList <int> IdCode[NumCategories][NumButtons];
		static int id[NumCategories][NumButtons];
		static qMouseTool *instance_;

		//This function configures all the combo boxes with the names 
		//and ids of all the various dialogs. 
		virtual void initializeComboBoxes();
		qMouseTool(QWidget * );

	public:
		//class is a singleton.. so we use an instance function
		//and protect the constructor.
		static qMouseTool *instance(QWidget * );
		virtual ~qMouseTool();
		
	
		virtual void dialogBuildRequest(const int type, QmapObject *geoObject1_, QmapObject *geoObject2_);		

		//Add entry provides a quick way of adding a new label and it's coresponding
		//idcode.
		virtual void addEntry(const char *label, int idcode, int button, int category);

	protected slots:
		virtual void cbDone();
		virtual void cbHelp();

};

#endif
