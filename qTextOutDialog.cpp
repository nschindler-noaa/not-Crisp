#include <stdio.h>
#include "qTextOutDialog.h"
#include "Qcrisp.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qprinter.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qfiledialog.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include "log.h"
#include "qHelpManager.h"

/* The dialog frame whose sole job is to contain a ZatTextOutPane.
 */
qTextOutDialog::qTextOutDialog(QWidget *w,	char *szLabel, int compress)
	: QMainWindow( w, szLabel, WDestructiveClose ) 
{

	setHelpPage( 83 );

	/* Set up menu. */
	QMenuBar * menu =  menuBar();
	QPopupMenu *fileMenu = new QPopupMenu;
	QPopupMenu *editMenu = new QPopupMenu;
	QPopupMenu *helpMenu = new QPopupMenu;
	fileMenu->insertItem ( "Save As...", this, SLOT(cbSaveAs()  )  );
	fileMenu->insertItem ( "Print...", this, SLOT(cbPrint()  ));
	fileMenu->insertItem ( "Close", this, SLOT(cbClose()  ));
	editMenu->insertItem ( "Copy", this, SLOT( cbCopy() ));
	editMenu->insertItem ( "Select All", this, SLOT( cbSelectAll() ));
	menu->insertItem("File", fileMenu);
	menu->insertItem("Edit", editMenu);
	menu->insertItem("Help", helpMenu);	
	

	/* Create logging pane. */
	lp_ = new QTextBrowser(this);
	lp_->setFont(QFont("Courier", 10)  ); //make it a monospaced for better reports
	setCentralWidget(lp_);
	resize(645,330);

	/* reveal self. */
	show();
}

qTextOutDialog::~qTextOutDialog() {

}


void qTextOutDialog::cbSaveAs(){
	/* Send current contents to file.
	 */

	//char cBuf[250]; /* Hope this is enough. */
	
	QString types("Text files (*.txt);;"      "Any files (*)");

	QString name = QFileDialog::getSaveFileName (QString::null, types, this, 
		"SaveAs Dialog", "Save Log As...");

	if(name != QString::null){
		
		if(!name.endsWith(".txt"))
			name.append(".txt");
		QFile file( name ); // Write the text to a file
		if ( file.open( IO_WriteOnly ) ) {
			QTextStream stream( &file );
			stream << lp_->text();
			//textEdit->setModified( FALSE );
		}
		
	}

	
	return ;
}
void qTextOutDialog::cbHelp(){
	
	helpManager->showHelp( QString("%1.html").arg(getHelpPage()) );
	return ;
}
void qTextOutDialog::cbPrint(){
	QPrinter printer( QPrinter::HighResolution );
	
	if(printer.setup( this ) ){
		//log_msg(0,"print ok'd");
		QPainter p;
        if( !p.begin( &printer ) ) {               // paint on printer
		    return;
        }

        QPaintDeviceMetrics metrics( p.device() );
        int dpiy = metrics.logicalDpiY();
        int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
        QRect body( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
        QSimpleRichText richText( QStyleSheet::convertFromPlainText(lp_->text()),
                                  lp_->font(),
                                  lp_->context(),
                                  lp_->styleSheet(),
                                  lp_->mimeSourceFactory(),
                                  body.height() );
        richText.setWidth( &p, body.width() );
        QRect view( body );
        int page = 1;
        do {
            richText.draw( &p, body.left(), body.top(), view, colorGroup() );
            view.moveBy( 0, body.height() );
            p.translate( 0 , -body.height() );
            p.drawText( view.right() - p.fontMetrics().width( QString::number( page ) ),
                        view.bottom() + p.fontMetrics().ascent() + 5, QString::number( page ) );
            if ( view.top()  >= richText.height() )
                break;
            printer.newPage();
            page++;
        } while (TRUE);
	}
	return;
}

void qTextOutDialog::cbClose(){	
	close();	
}

void qTextOutDialog::cbCopy(){	
	lp_->copy();	
}

void qTextOutDialog::cbSelectAll(){	
	lp_->selectAll();		
}

/* Entry from "outside" the qTextOutDialog & qTextOutPane classes.
 *  Log string to window.
 */
void qTextOutDialog::addString(char *str){
	lp_->append(  str );
}

/* Entry from "outside" - open file and log all text therein to window.
 */
void qTextOutDialog::addFile( char *fname ) {
	FILE *fp;
	char cBuf[256];

	fp = fopen( fname, "r" );
	while (fgets( cBuf, 255, fp ) != NULL)
		addString( cBuf );
	fclose( fp );
}

/* Bring window to top without activating. It appears that ZApp doesn't have
 *  a good way to do this on the PC.
 */
void qTextOutDialog::bringForward( void ) {
	raise();
}
