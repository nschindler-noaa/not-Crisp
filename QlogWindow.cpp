#include "QLogWindow.h"
#include <qevent.h>


#ifdef SOLARIS2
#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <qapplication.h>
#include <qvariant.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qeventloop.h>
#include <qtoolbar.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qfont.h>


#include "log.h"
#include "Qcrisp.h"
#include "Qmain.h"
#include <qcheckbox.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include <qclipboard.h>
#include <qtextedit.h>
#include "qHelpManager.h"

#ifdef WIN32
#include <io.h>
#endif

static const char* const image0_data[] = { 
"20 20 8 1",
"b c #000000",
"e c #005284",
"a c #4a7ba5",
"f c #6b6b21",
". c #8494a5",
"c c #ad8400",
"d c #dece29",
"# c #ffffff",
"###################b",
"###################b",
"##.bc#####.b#######b",
"#db#ab###cbb#######b",
"#bc##a####cb#######b",
"#ba#######ab#######b",
"#bc#######cb#######b",
"#cb##b####ab#######b",
"##cbb#####cb#######b",
"###################b",
"###################b",
"###########b#######b",
"#########bbabbc#cb#b",
"#######bec#badabac#b",
"######.fcf.a#ac#.f#b",
"######acf#a####cab#b",
"#######a.bbbcbca.b#b",
"##############a####b",
"###################b",
"abebababfbebababfbab"};


static const char* const image1_data[] = { 
"22 22 7 1",
". c None",
"# c #000000",
"b c #2e2e2e",
"c c #5c5c5c",
"d c #878787",
"e c #c2c2c2",
"a c #ffffff",
"......................",
"....##########........",
"....#aaaaaaa#b#.......",
"....#aaaaaaa#cb#......",
"....#aaaaaaa#dcb#.....",
"....#aaaaaaa#edcb#....",
"....#aaaaaaa#aedcb#...",
"....#aaaaaaa#######...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....#aaaaaaaaaaaaa#...",
"....###############...",
"......................",
"......................"};



static const char* const image4_data[] = { 
"22 22 88 2",
"Qt c None",
".2 c #000000",
".S c #08ff08",
"#v c #100810",
".U c #101010",
"#c c #101018",
".M c #181018",
"#e c #181818",
".A c #181821",
".L c #211821",
"#l c #212121",
".z c #212129",
"#m c #292129",
"#u c #292929",
"#n c #292931",
".R c #29ff29",
"#o c #312931",
".T c #313131",
"#p c #313139",
".Z c #31ff31",
"#q c #393139",
"#t c #393939",
".y c #393942",
"#s c #423942",
".o c #424242",
"#h c #4a4a52",
".n c #5a525a",
"#r c #5a5a63",
".I c #5ace5a",
"#b c #6b636b",
".p c #6b6b6b",
".x c #6b6b73",
".Y c #6bff63",
".l c #736b73",
".t c #7b737b",
".s c #7b7384",
".0 c #7bff7b",
".r c #847b84",
".u c #847b8c",
"#g c #84848c",
".v c #8c7b94",
"#i c #8c848c",
".w c #8c8494",
"#j c #8c8c8c",
".8 c #8c8c94",
".m c #948c94",
"#k c #948c9c",
"#f c #949494",
".q c #94949c",
".J c #94c694",
"#d c #9c949c",
"#a c #9c94a5",
".k c #9c9c9c",
".N c #9c9ca5",
".H c #9ccea5",
".K c #a59ca5",
"#. c #a59cad",
".i c #a5a5a5",
".3 c #a5a5ad",
"## c #ad9cad",
".V c #ada5ad",
".d c #adadad",
".j c #adadb5",
".9 c #b5adb5",
".# c #b5b5b5",
".a c #bdbdbd",
".7 c #bdd6bd",
".c c #c6c6c6",
".5 c #cec6ce",
".b c #cecece",
".4 c #ceced6",
".F c #d6ced6",
".G c #d6cede",
".h c #d6d6d6",
".E c #d6d6de",
".Q c #d6ffd6",
".B c #ded6de",
".1 c #ded6e7",
".g c #dedede",
".D c #dedee7",
".6 c #e7dee7",
".f c #e7e7e7",
".C c #e7e7ef",
".X c #e7ffe7",
".O c #efe7ef",
".e c #efefef",
".W c #f7f7f7",
".P c #ffffff",
"QtQtQtQtQtQt.#.a.b.b.b.b.c.c.a.a.d.aQtQtQtQt",
"QtQtQtQtQtQt.a.e.f.f.f.f.f.e.e.e.g.aQtQtQtQt",
"QtQtQtQtQtQt.a.c.c.c.b.b.c.c.c.c.a.cQtQtQtQt",
"QtQtQtQtQtQt.#.a.a.a.a.#.a.a.#.#.d.aQtQtQtQt",
"QtQtQtQtQt.c.d.c.a.c.c.c.a.a.a.c.#QtQtQtQtQt",
"QtQtQtQtQt.a.a.#.a.a.a.a.a.a.c.c.#QtQtQtQtQt",
"QtQtQtQtQt.a.#.c.a.a.a.a.a.c.a.c.dQtQtQtQtQt",
"QtQtQtQtQt.c.a.a.a.a.a.a.a.a.a.a.#QtQtQtQtQt",
"QtQtQtQtQt.d.b.f.g.g.g.g.g.g.h.g.i.i.jQtQtQt",
"QtQtQt.a.k.l.#.h.b.h.b.h.b.h.g.g.m.n.o.p.#Qt",
"QtQt.a.q.r.s.t.t.t.t.t.t.t.u.v.w.x.y.z.A.o.i",
"Qt.a.k.B.C.D.B.E.E.E.E.F.G.H.I.J.K.o.L.L.M.y",
".a.N.O.P.P.P.P.P.P.P.P.P.Q.R.S.R.b.v.T.A.U.L",
".V.W.P.P.P.P.P.P.P.P.P.P.X.Y.Z.0.P.1.t.A.2.L",
".3.E.4.5.4.h.E.E.g.6.D.B.D.E.7.F.4.5.8.M.2.A",
".m.9.j.V.3#..3.K#.#..i#..K#.###a.q.8#b#c.2.L",
".m.j.j#..3.K.K.K.N.K.N.N.N.N#a#d#d.w#b#c.2#e",
"#f#.#..K.N.K.N.N.N#a.k#a#d#d#d#a.m#g#b.M.2#h",
".m.3.K.K#a.k#a#d#a.k#a#d#a#d.q.m.8#i.x#c#e.d",
"#f#g#i.w#j.w#i.8.w#i.8.8.m.8.m#k.8.w#b#e#fQt",
".#.l.z.A#l.z#m#m#m#n#o#o#p#p#q#q#p#o#p#fQtQt",
"QtQt.d#r#s#s#t#p.T.T.T#u#u.z#e#e#v.o.kQtQtQt"};

static const char* const image8_data[] = { 
"22 22 6 1",
". c None",
"# c #000000",
"b c #000082",
"c c #3c3cfd",
"d c #8b8bfd",
"a c #ffffff",
"......................",
"......................",
"########..............",
"#aaaaaa##.............",
"#a####a#a#............",
"#aaaaaa#aa#...........",
"#a####a#bbbbbbbb......",
"#aaaaaa#baaaaaabb.....",
"#a#####aba####abcb....",
"#aaaaaaabaaaaaabdcb...",
"#a#####aba####abadcb..",
"#aaaaaaabaaaaaabbbbbb.",
"#a#####aba####aaaaaab.",
"#aaaaaaabaaaaaaaaaaab.",
"#a#####aba#########ab.",
"#aaaaaaabaaaaaaaaaaab.",
"########ba#########ab.",
"........baaaaaaaaaaab.",
"........ba#########ab.",
"........baaaaaaaaaaab.",
"........bbbbbbbbbbbbb.",
"......................"};



/*
 *  Constructs a QLogWindow as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
QLogWindow::QLogWindow(const char* name, WFlags fl )
    : QMainWindow( 0, name, fl ),
      image0( (const char **) image0_data ),
      image1( (const char **) image1_data ),
      image4( (const char **) image4_data ),
      image8( (const char **) image8_data )    
{
    (void)statusBar();
    if ( !name )
	setName( "QLogWindow" );
    setIcon( image0 );
	resize(500,250);
	statusBar()->close(); //close status bar
	/* Create logging file if necessary. Don't try more than once. */
	if (logFile_ == NULL) {
	#ifndef WIN32
		strcpy( logFileName_, "/tmp/COMPASSXXXXXX" );
	#else
		strcpy( logFileName_, "COMPASSXXXXXX" );
	#endif
	
	if ((long) mktemp( logFileName_ ) ) {
			strcat( logFileName_, ".logfile" );
			logFile_ = fopen( logFileName_, "w+" );
		}
	}

 
	//GUI stuff
	// actions
    fileNewAction = new QAction( this, "fileNewAction" );
    fileNewAction->setIconSet( QIconSet( image1 ) );
    fileSaveAsAction = new QAction( this, "fileSaveAsAction" );
    filePrintAction = new QAction( this, "filePrintAction" );
    filePrintAction->setIconSet( QIconSet( image4 ) );
    fileExitAction = new QAction( this, "fileExitAction" );
    editCopyAction = new QAction( this, "editCopyAction" );
    editCopyAction->setIconSet( QIconSet( image8 ) );
    editSelect_AllAction = new QAction( this, "editSelect_AllAction" );
    loggingWarningAction = new QAction( this, "loggingWarningAction" );
    loggingMessageAction = new QAction( this, "loggingMessageAction" );
    loggingDebugAction = new QAction( this, "loggingDebugAction" );
    loggingRawAction = new QAction( this, "loggingRawAction" );

    // menubar
    MenuBar = new QMenuBar( this, "MenuBar" );

    MenuBar->setGeometry( QRect( 0, 0, 600, 29 ) );

    fileMenu = new QPopupMenu( this );
    fileNewAction->addTo( fileMenu );
    fileSaveAsAction->addTo( fileMenu );
    filePrintAction->addTo( fileMenu );
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( QString(""), fileMenu, 2 );

    editMenu = new QPopupMenu( this );
    editCopyAction->addTo( editMenu );
    editSelect_AllAction->addTo( editMenu );
    MenuBar->insertItem( QString(""), editMenu, 3 );

    LoggingMenu = new QPopupMenu( this );
    loggingWarningAction->addTo( LoggingMenu );
	loggingWarningAction->setToggleAction ( true ); 
    loggingMessageAction->addTo( LoggingMenu );
	loggingMessageAction->setToggleAction ( true ); 
    loggingDebugAction->addTo( LoggingMenu );
	loggingDebugAction->setToggleAction ( true ); 
    loggingRawAction->addTo( LoggingMenu );
	loggingRawAction->setToggleAction ( true ); 
    MenuBar->insertItem( QString(""), LoggingMenu, 4 );

	//Set appropriate messaging checks
	loggingWarningAction->setOn(log_warning);
	loggingMessageAction->setOn(log_message);
	loggingDebugAction->setOn(log_debug);
	loggingRawAction->setOn(log_raw);
	
	//One line to insert the help menu/button
    MenuBar->insertItem("Help", this, SLOT(helpPressed())  );	

    languageChange();  
	text = QString("");
    clearWState( WState_Polished );
	qte = new QTextEdit(this, "Text Logging Widget");
	setCentralWidget(qte);
	qte->setCurrentFont( QFont("Courier"));
	qte->setText(" ");
	qte->setWordWrap(QTextEdit::NoWrap);
	qte->setUndoRedoEnabled ( false );
	qte->setReadOnly (true);

    // signals and slots connections
    connect( fileNewAction, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    connect( fileSaveAsAction, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
    connect( filePrintAction, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    connect( fileExitAction, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    connect( editCopyAction, SIGNAL( activated() ), this, SLOT( editCopy() ) );
	connect( editSelect_AllAction, SIGNAL( activated() ), this, SLOT( editSelectAll() ) );
	connect( loggingWarningAction, SIGNAL(  toggled(bool) ), this, SLOT( Warning(bool) ) );
	connect( loggingMessageAction, SIGNAL(  toggled(bool) ), this, SLOT( Message(bool) ) );
	connect( loggingRawAction, SIGNAL(  toggled(bool) ), this, SLOT( Raw(bool) ) );
	connect( loggingDebugAction, SIGNAL(  toggled(bool) ), this, SLOT( Debug(bool) ) );

    
}

/*
 *  Destroys the object and frees any allocated resources
 */
QLogWindow::~QLogWindow()
{
	//delete static instance
	m_pInstance = NULL;
    // no need to delete child widgets, Qt does it all for us
}

//Select all log text action
void QLogWindow::editSelectAll(){
	qte->selectAll(true);

}

/* The main app deletes logging files as part of normal termination.
 */
void QLogWindow::deleteLogFile( void ) {
	if (logFile_ != NULL)
		fclose( logFile_ );
	remove( logFileName_ );
}

/* The dialog does not re-create itself as needed, unless that function
 *  is explicitly requested. This is because the "run" thread might auto
 *  create a log window, which would then be destroyed when the thread 
 *  terminated.
 */

QLogWindow * QLogWindow::m_pInstance = NULL;
char QLogWindow::logFileName_[30];
FILE *QLogWindow::logFile_ = NULL;

QLogWindow *QLogWindow::instance(int autoCreate){
	if (!autoCreate)
		return m_pInstance;
	
	/* This is NOT thread safe! But we should only call this from the main thread, so it should be fine */
	if (m_pInstance != NULL)
		return m_pInstance;
	
	m_pInstance = new QLogWindow("LogWindow");

	return m_pInstance;  /* May be returned NULL. */
}


/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void QLogWindow::languageChange()
{
    setCaption( tr( "Log Window" ) );
    fileNewAction->setText( tr( "New" ) );
    fileNewAction->setMenuText( tr( "&New" ) );
    fileNewAction->setAccel( tr( "Ctrl+N" ) );
    fileSaveAsAction->setText( tr( "Save As" ) );
    fileSaveAsAction->setMenuText( tr( "Save &As..." ) );
    fileSaveAsAction->setAccel( QString::null );
    filePrintAction->setText( tr( "Print" ) );
    filePrintAction->setMenuText( tr( "&Print..." ) );
    filePrintAction->setAccel( tr( "Ctrl+P" ) );
    fileExitAction->setText( tr( "Exit" ) );
    fileExitAction->setMenuText( tr( "E&xit" ) );
    fileExitAction->setAccel( QString::null );
    editCopyAction->setText( tr( "Copy" ) );
    editCopyAction->setMenuText( tr( "C&opy" ) );
    editCopyAction->setAccel( tr( "Ctrl+C" ) );
    editSelect_AllAction->setText( tr( "Select All" ) );
    loggingWarningAction->setText( tr( "Warning" ) );
    loggingMessageAction->setText( tr( "Message" ) );
    loggingDebugAction->setText( tr( "Debug" ) );
    loggingRawAction->setText( tr( "Raw" ) );
    MenuBar->findItem( 2 )->setText( tr( "&File" ) );
    MenuBar->findItem( 3 )->setText( tr( "&Edit" ) );
    MenuBar->findItem( 4 )->setText( tr( "Logging" ) );
    
}

void QLogWindow::fileNew()
{
	//clear out old text
	qte->clear();

	//clear out old log file
    if (logFile_) {
    		fclose( logFile_ );
			logFile_ = fopen( logFileName_, "w+" );
        }
}

void QLogWindow::helpPressed(){
	helpManager->showHelp( QString("84.html") );
}



void QLogWindow::fileSaveAs()
{
	/* Copy logging file to another file, then go on logging into
	 *  original.
	 */
	if (logFile_ != NULL) {
		char cBuf[250]; /* Hope this is enough. */
		
		QString types("Text files (*.txt);;"      "Any files (*)");

		QString name = QFileDialog::getSaveFileName (QString::null, types, this, 
			"SaveAs Dialog", "Save Log As...");

		if(name != QString::null){
			
			if(!name.endsWith(".txt"))
				name.append(".txt");
			FILE *fp = fopen( name, "w" );
			if (fp == NULL){
				QMessageBox::warning ( this,"Log Save Error","Unable to open file.", "OK" );
							
			}
			else {
				fseek( logFile_, 0, SEEK_SET );
				while (fgets( cBuf, 249, logFile_ ) != NULL)
					fputs( cBuf, fp );
				fseek( logFile_, 0, SEEK_END );
				fclose( fp );
			}			
		}
	}
}

void QLogWindow::filePrint()
{
    QPrinter printer(QPrinter::PrinterResolution);
	
	if(printer.setup ( this )){
		QPainter p;
        if( !p.begin( &printer ) ) {               // paint on printer
            return;
        }

        QPaintDeviceMetrics metrics( p.device() );
        int dpiy = metrics.logicalDpiY();
        int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
        QRect body( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
        QSimpleRichText richText( QStyleSheet::convertFromPlainText(qte->text()),
                                  QFont(),
                                  qte->context(),
                                  qte->styleSheet(),
                                  qte->mimeSourceFactory(),
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

}

void QLogWindow::fileExit()
{
	close();

}

void QLogWindow::editCopy()
{
	
	if(qte->hasSelectedText()){
		QClipboard *cb = qApp->clipboard();
		cb->setText(qte->selectedText(), QClipboard::Clipboard);
	}
	
}

void QLogWindow::Warning(bool on){
	if(on)
		log_warning = 1;
	else
		log_warning = 0;
}

void QLogWindow::Message(bool on){
	 if(on)
		log_message = 1;
	else
		log_message = 0;
}

void QLogWindow::Debug(bool on){
	if(on)
		log_debug = 1;
	else
		log_debug = 0;
}

void QLogWindow::Raw(bool on){
	if(on)
		log_raw = 1;
	else
		log_raw = 0;
}


void QLogWindow::customEvent( QCustomEvent * e ){

	QLogEvent * event = (QLogEvent*) e;
	
	addMsg( (char*)event->string.ascii() , event->level );
}



//Function that adds text to the log window
void QLogWindow::addMsg( char *msg, int id ) {
	int log_it = 0;
	if(m_pInstance != NULL){
		switch(id){
		case IDM_ERRORS:
			log_it = 1;
			break;

		case IDM_WARNINGS:
			if (log_warning)
				log_it = 1;
			break;

		case IDM_MESSAGES:
			if (log_message)
				log_it = 1;
			break;

		case IDM_RAWDUMP:
			if (log_raw)
				log_it = 1;
			break;

		case IDM_DEBUG:
			if (log_debug)
				log_it = 1;
			break;

		default:
			break;
		}
	if (!log_it)
		return;

	//Do actual logging
	text =  QString(msg);

	/* If this starts a new line, add a timestamp (consider this part of the
	 * line even for repetition purposes since it's useful to see a message
	 * appear a couple times after a minute has elapsed) */
	int tmpP, tmpIndex;
	qte->getCursorPosition(&tmpP, &tmpIndex);
	if (tmpIndex == 0) text = QTime::currentTime().toString("hh:mm\t") + text;

	// If this isn't a full line, then repetition doesn't matter
	if (text.find('\n', -1) == -1) {
		lastLine = "";
	}
	
	if (lastLine != "" && lastLine == text) {
		repCount++;

		if (repCount > 1) {
			qte->removeParagraph(qte->paragraphs()-2);
		}

		qte->setCursorPosition(qte->paragraphs()-1,
			qte->paragraphLength(qte->paragraphs()-1));
		qte->insert(QString().sprintf("\tThe preceding line was repeated %d time%s\n", repCount, repCount > 1 ? "s" : "")); 
	}
	else {
		qte->setCursorPosition(qte->paragraphs()-1,
			qte->paragraphLength(qte->paragraphs()-1));
		qte->insert(text); 
		lastLine = QString(text);
		repCount = 0;
	}
	
	
		
	/* log to file if appropriate.
	 */
	if (logFile_ != NULL) {
		fputs( msg, logFile_ );
		fflush( logFile_ );
	}
	}

}


