#include <fstream>

#include "qGraphDialog.h"
#include "qGlobalEvents.h"
#include "run_mode.h"
#include "qGraphConstants.h"
#include "resource.h"
#include "log.h"
#include "Qcrisp.h"
#include <qsizepolicy.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>

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
"25 22 5 1",
"# c #000000",
"c c #0000ff",
"b c #808080",
"a c #c0c0c0",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaa#############aaaab#",
".aaaaa#c#.......#b#aaaab#",
".aaaaa#c#.......#c#aaaab#",
".aaaaa#c#.......#c#aaaab#",
".aaaaa#c#.......#c#aaaab#",
".aaaaa#c#.......#c#aaaab#",
".aaaaa#cc#######cc#aaaab#",
".aaaaa#ccccccccccc#aaaab#",
".aaaaa#ccbbbbbbbcc#aaaab#",
".aaaaa#ccbccbbbbcc#aaaab#",
".aaaaa#ccbccbbbbcc#aaaab#",
".aaaaa#ccbccbbbbcc#aaaab#",
".aaaaaa############aaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image2_data[] = { 
"25 22 5 1",
"# c #000000",
"c c #0000ff",
"b c #808080",
"a c #c0c0c0",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaa########aaaaaab#",
".aaaaaaa#.......#aaaaaab#",
".aaaaaaa#.####..#aaaaaab#",
".aaaaaa#.......#aaaaaaab#",
".aaaaaa#.####..####aaaab#",
".aaaaa#.......#aa#.#aaab#",
".aaaa##########a#.##aaab#",
".aaa#aaaaaaaaaa#.#a#aaab#",
".aa##############aa#aaab#",
".aa#aaaaaaaaaaaa#a#aaaab#",
".aa#aaaaaaa###aa###aaaab#",
".aa#aaaaaaacccaa#.#aaaab#",
".aa#############.#aaaaab#",
".aaa#.........#.#aaaaaab#",
".aaaa###########aaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};


static const char* const image3_data[] = { 
"25 22 5 1",
"# c #000000",
"b c #808080",
"a c #c0c0c0",
"c c #ff0000",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaa#####aaaaaaaaab#",
".aaaaaaa#ccccc#aaaaaaaab#",
".aaaaaa#cc###cc#aaaaaaab#",
".aaaaaa#cc#a#cc#aaaaaaab#",
".aaaaaaa##aa#cc#aaaaaaab#",
".aaaaaaaaaa#cc#aaaaaaaab#",
".aaaaaaaaa#cc#aaaaaaaaab#",
".aaaaaaaaa#cc#aaaaaaaaab#",
".aaaaaaaaa#cc#aaaaaaaaab#",
".aaaaaaaaa#cc#aaaaaaaaab#",
".aaaaaaaaaa##aaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaa##aaaaaaaaaab#",
".aaaaaaaaa#cc#aaaaaaaaab#",
".aaaaaaaaa#cc#aaaaaaaaab#",
".aaaaaaaaaa##aaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image4_data[] = { 
"25 22 6 1",
"# c #000000",
"c c #800000",
"b c #808080",
"a c #c0c0c0",
"d c #cbaf14",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaacaaaaab#",
".aaaaaaaaaaaacaaaaaaaaab#",
".aaaaaaaaa#aaaaaaaaaaaab#",
".aaaaaaaa#d#aaaaaaaaaaab#",
".aaaaaaaaa#d#aacaaaaaaab#",
".aaaaaaaaaa###aaaaacaaab#",
".aaaaaaaaaaa###aaaaaaaab#",
".aaa############aaaaaaab#",
".aaa#bbbbbbbbb###aaaaaab#",
".aaa#aacaaaaca####aaaaab#",
".aaa#acaccacac#a###aaaab#",
".aaa#caaaacaaa#aa###aaab#",
".aaa#aaaaaaaaa#aaa##aaab#",
".aaa###########aaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};


static const char* const image5_data[] = { 
"25 22 5 1",
"# c #000000",
"b c #808080",
"a c #c0c0c0",
"c c #ff0000",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaacaab#",
".aaaaaaaaaaaaaaaaaaacaab#",
".aaaaaaaaaaaaaaaaaaccaab#",
".aaaaaaaacaaaaaaaaccaaab#",
".aaaaaaaacaaaaaaaacaaaab#",
".aaaaaaacccaaaaaaccaaaab#",
".aaaaaaacacaaaaaccaaaaab#",
".aaaaaaccaccaaaccaaaaaab#",
".aaaaaacaaaccaacaaaaaaab#",
".aaaaaccaaaacaccaaaaaaab#",
".aaaaacaaaaacccaaaaaaaab#",
".aaaaccaaaaaacaaaaaaaaab#",
".aaaacaaaaaaacaaaaaaaaab#",
".aaaccaaaaaaaaaaaaaaaaab#",
".aaacaaaaaaaaaaaaaaaaaab#",
".aaacaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image6_data[] = { 
"25 22 5 1",
"# c #000000",
"b c #808080",
"a c #c0c0c0",
"c c #ff0000",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaacccccaaab#",
".aaaaaaaaaaaaaacccccaaab#",
".aaaaaaaaaaaaaacccccaaab#",
".aaaacccccaaaaacccccaaab#",
".aaaacccccaaaaacccccaaab#",
".aaaacccccaaaaacccccaaab#",
".aaaacccccaaaaacccccaaab#",
".aaaacccccaaaaacccccaaab#",
".aaaacccccccccccccccaaab#",
".aaaacccccccccccccccaaab#",
".aaaacccccccccccccccaaab#",
".aaaacccccccccccccccaaab#",
".aaaacccccccccccccccaaab#",
".aaaacccccccccccccccaaab#",
".aaaacccccccccccccccaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image7_data[] = { 
"25 22 5 1",
"# c #000000",
"b c #808080",
"a c #c0c0c0",
"c c #ff0000",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aacccccccaaaaaaaaaaaaab#",
".aacccccccaaaaaaaaaaaaab#",
".aaccaaaccaaaccccccccaab#",
".aaccaaaccaaaccccccccaab#",
".aaccaaacccccccaaaaccaab#",
".aaccaaacccccccaaaaccaab#",
".aaccaaaccaaaccaaaaccaab#",
".aaccaaaccaaaccaaaaccaab#",
".aaccaaaccaaaccaaaaccaab#",
".aaccaaaccaaaccaaaaccaab#",
".aaccaaaccaaaccaaaaccaab#",
".aaccaaaccaaaccaaaaccaab#",
".aaccaaaccaaaccaaaaccaab#",
".aaccaaaccaaaccaaaaccaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image8_data[] = { 
"25 22 5 1",
"# c #000000",
"b c #808080",
"a c #c0c0c0",
"c c #ff0000",
". c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaacccccccaaab#",
".aaaaaaaaaaaacccccccaaab#",
".aacccccccaaaccaaaccaaab#",
".aacccccccaaaccaaaccaaab#",
".aaccaaaccaaaccaaaccaaab#",
".aaccaaaccaaaccaaaccaaab#",
".aaccaaaccaaaccaaaccaaab#",
".aaccaaacccccccaaaccaaab#",
".aaccaaacccccccaaaccaaab#",
".aaccaaaaaaaaaaaaaccaaab#",
".aaccaaaaaaaaaaaaaccaaab#",
".aaccaaaaaaaaaaaaaccaaab#",
".aaccaaaaaaaaaaaaaccccab#",
".aaccaaaaaaaaaaaaaccccab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image9_data[] = { 
"25 22 5 1",
". c #000000",
"a c #808080",
"b c #c0c0c0",
"c c #ff0000",
"# c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".a#b#b#b#b#b#b#b#b#b#b#b#",
".ab#b#b#b#b#b#b#b#b#b#bb#",
".a#b#b#b#b#b#b#b#b#b#b#b#",
".ab#b#b#b#b#b#ccccccc#bb#",
".a#b#b#b#b#b#bcccccccb#b#",
".ab#ccccccc#b#ccb#bcc#bb#",
".a#bcccccccb#bcc#b#ccb#b#",
".ab#ccb#bcc#b#ccb#bcc#bb#",
".a#bcc#b#ccb#bcc#b#ccb#b#",
".ab#ccb#bcc#b#ccb#bcc#bb#",
".a#bcc#b#ccccccc#b#ccb#b#",
".ab#ccb#bcccccccb#bcc#bb#",
".a#bcc#b#b#b#b#b#b#ccb#b#",
".ab#ccb#b#b#b#b#b#bcc#bb#",
".a#bcc#b#b#b#b#b#b#ccb#b#",
".ab#ccb#b#b#b#b#b#bccccb#",
".a#bcc#b#b#b#b#b#b#ccccb#",
".ab#b#b#b#b#b#b#b#b#b#bb#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image10_data[] = { 
"25 22 5 1",
". c #000000",
"a c #808080",
"b c #c0c0c0",
"c c #ff0000",
"# c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".a#b#b#b#b#b#b#b#b#b#b#b#",
".ab#b#b#b#b#b#b#b#b#b#bb#",
".a###################b#b#",
".ab#ccccccc#b#b#b#b#b#bb#",
".a##cccccccb#b#b#b#b#b#b#",
".ab#ccb#bcc#b#ccccccccbb#",
".a##cc#b#ccb#bcccccccc#b#",
".ab#ccb#bcccccccb#b#ccbb#",
".a##cc#b#ccccccc#b#bcc#b#",
".ab#ccb#bcc#b#ccb#b#ccbb#",
".a##cc#b#ccb#bcc#b#bcc#b#",
".ab#ccb#bcc#b#ccb#b#ccbb#",
".a##cc#b#ccb#bcc#b#bcc#b#",
".ab#ccb#bcc#b#ccb#b#ccbb#",
".a##cc#b#ccb#bcc#b#bcc#b#",
".ab#ccb#bcc#b#ccb#b#ccbb#",
".a#bcc#b#ccb#bcc#b#bcc#b#",
".ab#b#b#b#b#b#b#b#b#b#bb#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image11_data[] = { 
"25 22 5 1",
". c #000000",
"a c #808080",
"b c #c0c0c0",
"c c #ff0000",
"# c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".a#b#b#b#b#b#b#b#b#b#b#b#",
".ab#b#b#b#b#b#b#b#b#b#bb#",
".a#b#b#b#b#b#b#b#b#b#b#b#",
".ab#b#b#b#b#b#b#b#b#b#cb#",
".a#b#b#b#b#b#b#b#b#b#bcb#",
".ab#b#b#b#b#b#b#b#b#bccb#",
".a#b#b#b#b#c#b#b#b#bcc#b#",
".ab#b#b#b#bcb#b#b#b#c#bb#",
".a#b#b#b#bcccb#b#b#ccb#b#",
".ab#b#b#b#c#c#b#b#ccb#bb#",
".a#b#b#b#ccbcc#b#ccb#b#b#",
".ab#b#b#bcb#bcc#bcb#b#bb#",
".a#b#b#bcc#b#bcbcc#b#b#b#",
".ab#b#b#c#b#b#ccc#b#b#bb#",
".a#b#b#ccb#b#b#c#b#b#b#b#",
".ab#b#bcb#b#b#bcb#b#b#bb#",
".a#b#bcc#b#b#b#b#b#b#b#b#",
".ab#b#c#b#b#b#b#b#b#b#bb#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};

static const char* const image12_data[] = { 
"25 22 5 1",
". c #000000",
"a c #808080",
"b c #c0c0c0",
"c c #ff0000",
"# c #ffffff",
"........................#",
".aaaaaaaaaaaaaaaaaaaaaab#",
".a#b#b#b#b#b#b#b#b#b#b#b#",
".ab#b#b#b#b#b#b#b#b#b#bb#",
".a#b#b#b#b#b#b#b#b#b#b#b#",
".ab#b#b#b#b#b#b#bcccccbb#",
".a#b#b#b#b#b#b#b#ccccc#b#",
".ab#b#b#b#b#b#b#bcccccbb#",
".a#b#b#ccccc#b#b#ccccc#b#",
".ab#b#bcccccb#b#bcccccbb#",
".a#b#b#ccccc#b#b#ccccc#b#",
".ab#b#bcccccb#b#bcccccbb#",
".a#b#b#ccccc#b#b#ccccc#b#",
".ab#b#bcccccccccccccccbb#",
".a#b#b#ccccccccccccccc#b#",
".ab#b#bcccccccccccccccbb#",
".a#b#b#ccccccccccccccc#b#",
".ab#b#bcccccccccccccccbb#",
".a#b#b#ccccccccccccccc#b#",
".ab#b#bcccccccccccccccbb#",
".bbbbbbbbbbbbbbbbbbbbbbb#",
"#########################"};




qGraphDialog::qGraphDialog(QWidget *w, int iChildOpt, WFlags dw  ) 
: QMainWindow(NULL, "A graph Window", dw | Qt::WDestructiveClose),       
		saveImage( (const char **) image1_data ), 
		printImage( (const char **) image2_data ), 
		helpImage( (const char **) image3_data ),
		wandImage( (const char **) image4_data ),
		hollowDown( (const char **) image10_data ),
		hollowUp( (const char **) image7_data ),
		filledDown( (const char **) image12_data ),
		filledUp( (const char **) image6_data ),
		cityUp( (const char **) image8_data ),
		cityDown( (const char **) image9_data ),
		connectUp( (const char **) image5_data ),
		connectDown( (const char **) image11_data )
{
	buildGraphDialog( iChildOpt );
	connect( qEvents::Instance(), SIGNAL(GERunStatusChanged_()), this, SLOT(cbRunStatusChanged()));
}

qGraphDialog::qGraphDialog(QWidget *w, int iChildOpt,WFlags dw , const char *nt) 
		: QMainWindow(NULL, "A graph Window", dw),       
		saveImage( (const char **) image1_data ), 
		printImage( (const char **) image2_data ), 
		helpImage( (const char **) image3_data ),
		wandImage( (const char **) image4_data ),
		hollowDown( (const char **) image10_data ),
		hollowUp( (const char **) image7_data ),
		filledDown( (const char **) image12_data ),
		filledUp( (const char **) image6_data ),
		cityUp( (const char **) image8_data ),
		cityDown( (const char **) image9_data ),
		connectUp( (const char **) image5_data ),
		connectDown( (const char **) image11_data )
{
	buildGraphDialog( iChildOpt );
	cbRunStatusChanged();
	connect( qEvents::Instance(), SIGNAL(GERunStatusChanged_()), this, SLOT(cbRunStatusChanged()));	
}

/* evRemove_.notify() is called here, and by the IDC_OK command() case,
    and by kill(), to make sure that the other objects which interact
	with this one are always notified that this one is going away. 
	wasRemoveNotified_ is used to avoid multiple notifications,
    which could also cause problems. */
qGraphDialog::~qGraphDialog(){
	delete graphType;
}

/* Receive notification that run status has changed.
 */

void qGraphDialog::cbRunStatusChanged( ){
	if (pbOK_ != NULL) {
		if (run_mode.running) {
			pbOK_->setEnabled(FALSE);
			pbApply_->setEnabled(FALSE);
			pbApplyAll_->setEnabled(FALSE);
		}
		else {
			pbOK_->setEnabled(TRUE);
			pbApply_->setEnabled(TRUE);
			pbApplyAll_->setEnabled(TRUE);
		}
	}
}

/* Toolbar height. */	
#ifdef WIN32
	static int tHeight = 22;
#else
	static int tHeight = 0;
#endif	

void qGraphDialog::buildGraphDialog( int iChildOpt ){

	QLabel *st;
	int i;
	QSpacerItem *spacei;

	/* Initializations are here so they can be done once (two contstructors
	 *  both call this. */
	lock_ = 0;
	isActive_ = 0;
	wasRemoveNotified_ = 0;
	TitleSet_ = 0;
	isAutoscale_ = 1;
	isRounded_ = 0;
	isGrouped_ = 0;
	stDateX_ = NULL;
	rightMouseButtonPressed_ = 0;
	cmbStatic_ = NULL;
	pbOK_ = NULL;
	dataToken_ = NULL;
	edYMax_ = NULL;
	edYMin_ = NULL;
	
	//Make a Toolbar, add the buttons.
	i=1;
	tbar_ = new QToolBar(this, "Main ToolBar");
	tbar_->setMovingEnabled(FALSE);
	tbar_->setResizeEnabled (FALSE);
	
	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar_->boxLayout()->addItem( spacei );
	
	if (!(iChildOpt & HIDE_DONE)){
		pbDone_ = new QPushButton("Done",tbar_);		
		connect(pbDone_, SIGNAL(clicked()), this, SLOT(cancelPressed()));
		QApplication::sendPostedEvents( tbar_, QEvent::ChildInserted );
		spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
		tbar_->boxLayout()->addItem( spacei );
	}

	stPositionX_ = new QLabel("X: 100", tbar_ );
	stPositionX_->setFixedSize( 100, 30 );
	QApplication::sendPostedEvents( tbar_, QEvent::ChildInserted );

	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar_->boxLayout()->addItem( spacei );
	
	stPositionY_ = new QLabel("Y: 100", tbar_ );
	stPositionY_->setFixedSize( 100, 30 );

	QApplication::sendPostedEvents( tbar_, QEvent::ChildInserted );
	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar_->boxLayout()->addItem( spacei );

	if (iChildOpt & SHOW_DATE) {
//		log_msg(1, "showing date");
		stDateX_ = new QLabel("", tbar_ ); //make it blank by default
		stDateX_->setFixedSize( 80, 30 );
		QApplication::sendPostedEvents( tbar_, QEvent::ChildInserted );
		spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
		tbar_->boxLayout()->addItem( spacei );
	}
	
	pbSave_ = new QToolButton( tbar_);	
	pbSave_->setPixmap (saveImage);
	connect(pbSave_, SIGNAL(clicked()), SLOT(savePressed()));
	pbPrint_ = new QToolButton( tbar_);	
	pbPrint_->setPixmap (printImage);
	connect(pbPrint_, SIGNAL( clicked()), SLOT(printPressed()));
	pbHelp_ = new QToolButton( tbar_);
	pbHelp_->setPixmap (helpImage);
	connect(pbHelp_, SIGNAL(clicked()), SLOT(helpPressed()));
	QApplication::sendPostedEvents( tbar_, QEvent::ChildInserted );
	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar_->boxLayout()->addItem( spacei );

	if (!(iChildOpt & HIDE_ACTIVE)) {
		
		pbWand_ = new QToolButton(tbar_);
		pbWand_->setPixmap (wandImage);
		pbWand_->setToggleButton(TRUE);
		connect(pbWand_, SIGNAL(clicked()), SLOT(wandPressed()));
				
	}

	if (iChildOpt & SHOW_COMBO) {
		QApplication::sendPostedEvents( tbar_, QEvent::ChildInserted );
		spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
		tbar_->boxLayout()->addItem( spacei );
		cmbStatic_ = new QComboBox(tbar_);
//		log_msg(1, "added the combo box"); 
		connect(cmbStatic_, SIGNAL(activated(int)), this, SLOT(cbCombobox(int)));	
	}

	tbar_->show();

	i = 1;
	tbar2_ = new QToolBar( this, "second toolbar");
	tbar2_->setMovingEnabled(FALSE);
	tbar2_->setResizeEnabled (FALSE);
	
	st = new QLabel( tbar2_,"ymax label");
	st->setText( "Ymax:" );
	st->show();

	edYMax_ = new QLineEdit( tbar2_, "ymax text box");
	edYMax_->setFixedSize(90, 20);
 
	edYMax_->show();
	QApplication::sendPostedEvents( tbar2_, QEvent::ChildInserted );
	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar2_->boxLayout()->addItem( spacei );

	st = new QLabel( tbar2_, "ymin label");
	st->setText( "Ymin:" );
	st->show();
	edYMin_ = new QLineEdit( tbar2_,"ymin text box");
	edYMin_->setFixedSize(90, 20);
  
	QApplication::sendPostedEvents( tbar2_, QEvent::ChildInserted );
	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar2_->boxLayout()->addItem( spacei );
	
	bbFix_ = new QPushButton("Fix", tbar2_);
	bbFix_->setFixedSize(30,22);
	bbFix_->setToggleButton(TRUE);
	QApplication::sendPostedEvents( tbar2_, QEvent::ChildInserted );
	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar2_->boxLayout()->addItem( spacei );

	bbUpArrow_ = new QToolButton(UpArrow, tbar2_);
	bbUpArrow_->setFixedSize(30,22);
	connect(bbUpArrow_, SIGNAL(clicked()), this, SLOT( increaseScale()));

	bbDownArrow_ = new QToolButton(DownArrow, tbar2_);
	bbDownArrow_->setFixedSize(30,22);
	connect(bbDownArrow_, SIGNAL(clicked()), this, SLOT( decreaseScale()));

	QApplication::sendPostedEvents( tbar2_, QEvent::ChildInserted );
	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar2_->boxLayout()->addItem( spacei );
	
	bbRound_ = new QPushButton("Rnd", tbar2_);
	bbRound_->setFixedSize(30,22);
	bbRound_->setToggleButton(TRUE);

	QApplication::sendPostedEvents( tbar2_, QEvent::ChildInserted );
	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar2_->boxLayout()->addItem( spacei );

	graphType = new QButtonGroup();
	graphType->setExclusive(TRUE);
		
	QIconSet qis = QIconSet();
	qis.setPixmap ( connectUp,QIconSet::Automatic,QIconSet::Normal, QIconSet::Off );
	qis.setPixmap ( connectDown,QIconSet::Automatic,QIconSet::Normal, QIconSet::On );
	bbConDots_ = new QToolButton( tbar2_  );
	bbConDots_->setIconSet(qis);

	qis.setPixmap ( cityUp,QIconSet::Automatic,QIconSet::Normal, QIconSet::Off );
	qis.setPixmap ( cityDown,QIconSet::Automatic,QIconSet::Normal, QIconSet::On );
	bbCityScape_ = new QToolButton( tbar2_);
	bbCityScape_->setIconSet(qis);

	qis.setPixmap ( hollowUp,QIconSet::Automatic,QIconSet::Normal, QIconSet::Off );
	qis.setPixmap ( hollowDown,QIconSet::Automatic,QIconSet::Normal, QIconSet::On );
	bbHollowBars_ = new QToolButton( tbar2_);
	bbHollowBars_->setIconSet(qis);
	
	qis.setPixmap ( filledUp,QIconSet::Automatic,QIconSet::Normal, QIconSet::Off );
	qis.setPixmap ( filledDown,QIconSet::Automatic,QIconSet::Normal, QIconSet::On );
	bbFilledBars_ = new QToolButton( tbar2_);
	bbFilledBars_->setIconSet(qis);

	graphType->insert(bbConDots_, ID_CONDOTS);
	graphType->insert(bbCityScape_, ID_CITYSCAPE);
	graphType->insert(bbHollowBars_, ID_HOLLOWBARS);
	graphType->insert(bbFilledBars_, ID_FILLEDBARS);


	bbConDots_->setToggleButton ( TRUE );
	bbHollowBars_->setToggleButton ( TRUE );
	bbCityScape_->setToggleButton ( TRUE );
	bbFilledBars_->setToggleButton ( TRUE );
	
	bbConDots_-> toggle (); //set as the default

	QApplication::sendPostedEvents( tbar2_, QEvent::ChildInserted );
	spacei = new QSpacerItem( 10, 22, QSizePolicy::Fixed, QSizePolicy::Fixed );
	tbar2_->boxLayout()->addItem( spacei );

	bbGroup_ = new QPushButton("Grp", tbar2_);
	bbGroup_->setFixedSize(30,22);
	bbGroup_->setToggleButton ( TRUE );

	tbar2_->show();


	
	if (iChildOpt & SHOW_BOTTOM) {
		/* A whole bottom row of optional buttons. */
		tbar3_ = new QToolBar("Bottom Tool Bar",this );
		moveDockWindow(tbar3_, Qt::DockBottom );
		tbar3_->setResizeEnabled(FALSE);
		tbar3_->setMovingEnabled(FALSE);
		
		QWidget * spacer = new QWidget(tbar3_);
		tbar3_->setStretchableWidget ( spacer );

		pbSchedule_ = new QPushButton("Schedule", tbar3_);
		pbResetAll_ = new QPushButton("Reset All", tbar3_);
		pbApplyAll_ =  new QPushButton("Apply All", tbar3_);
		pbReset_  =  new QPushButton("Reset", tbar3_);
		pbApply_  =  new QPushButton("Apply",tbar3_ );
		pbCancel_ = new QPushButton("Cancel" , tbar3_ );
		pbOK_ = new QPushButton("OK",tbar3_);

		connect(pbOK_, SIGNAL(clicked()), this, SLOT(okPressed()));
		connect(pbCancel_, SIGNAL(clicked()), this, SLOT(cancelPressed()));
		connect(pbApply_, SIGNAL(clicked()), this, SLOT(applyPressed()));
		connect(pbReset_, SIGNAL(clicked()), this, SLOT(resetPressed()));
		connect(pbApplyAll_, SIGNAL(clicked()), this, SLOT(applyAllPressed()));
		connect(pbResetAll_, SIGNAL(clicked()), this, SLOT(resetAllPressed()));
		connect(pbSchedule_, SIGNAL(clicked()), this, SLOT(schedulePressed()));

		tbar3_->show();
	}
	
	mainWidget = new QWidget(this);
	setCentralWidget(mainWidget);
	
	//Make main widgets layout
	mainLayout = new QVBoxLayout(mainWidget,0, 5);
	mainLayout->setResizeMode(QLayout::Minimum);	
	
	stTitle_ = new QLabel(mainWidget);
	QFont f = stTitle_->font();
	f.setBold(true);
	stTitle_->setFont(f);

	stTitle_->setAlignment(AlignHCenter);
	stTitle_->resize( 50, 30 );
	stTitle_->setSizePolicy(  QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed ));

	//add to the layout manager
	mainLayout->addWidget(stTitle_);
	
	QFrame * frame = new QFrame(mainWidget);
	frame->setFrameStyle( QFrame::GroupBoxPanel | QFrame::Plain );
	frame->setLineWidth ( 1 );

	QHBoxLayout * boxLayout  = new QHBoxLayout(frame,1,1);
		
	//add to layout manager
	mainLayout->addWidget(frame);
	
	gp_ = new qGraphPane(frame);
	boxLayout->addWidget(gp_,1);
	gp_->setSizePolicy( QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred) );
	
	//Ourlegend is at the bottom of the layout
	legendLayout = new QHBoxLayout(mainLayout);
		
	//Set the area rectangle
	graphArea_ = gp_->rect();
	connect(gp_, SIGNAL(evDraw()), SLOT(evDraw()));
	
	QPixmap pm((const char **) image0_data);
	setIcon(pm);
	connect( edYMax_, SIGNAL(lostFocus ()), this , SLOT(YMinMaxKillFocus()));
	connect( edYMin_, SIGNAL(lostFocus ()), this , SLOT(YMinMaxKillFocus()));
	connect( edYMax_, SIGNAL(returnPressed ()), this, SLOT(typedYMinMax()));
	connect( edYMin_, SIGNAL(returnPressed ()), this, SLOT(typedYMinMax()));
	connect( bbFix_, SIGNAL(clicked()), this , SLOT(FixChanged()));
	connect( graphType, SIGNAL(clicked(int)), this , SLOT(graphViewChange(int )));
	connect( bbGroup_, SIGNAL(clicked()), this, SLOT(groupPressed()));
	connect( bbRound_, SIGNAL(clicked()), this, SLOT(roundButton()));
	
	resize(670,480);
	
	stTitle_->show();
}

/* Printing support. The graph is printed as a single page. The code is
 *  partly preexisting (Steve Umbehocker) leftovers, part reimplemented
 *  with reliance on the ZApp Programmer's Guide.
 */

void qGraphDialog::printMe( void )
{

	
	QPrinter printer(QPrinter::PrinterResolution);
	printer.setOrientation(QPrinter::Landscape) ;
	printer.setPageSize(QPrinter::Letter) ;
	printer.setMinMax ( 1,1 );
	printer.setPrintRange(QPrinter::AllPages);

	if(printer.setup ( 0)){
		QPainter p;
        if( !p.begin( &printer ) ) {               // paint on printer
            return;
        }
	
        QPaintDeviceMetrics metrics( p.device() );
        int dpiy = metrics.logicalDpiY();
        int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
        
		QRect body( 0,0, metrics.width(), metrics.height() );
	
		gp_->print(&p, body);
		p.end();
	
	}

}

/* We get a draw event from the graph pane because the legend strings and
 *  the title string are in seperate child windows when displaying on the
 *  screen, but are in the graph pane when printing.
 */
void qGraphDialog::evDraw( void )
{
	if(gp_->isPrintMode()){		
		drawLegend();
	}
}


/* This will be called if the window is closed by some external force,
    e.g. the Close command on the window's system menu. */

void qGraphDialog::closeEvent ( QCloseEvent * e ){
	if (!wasRemoveNotified_) {
		emit evCancel_();
		emit evRemove_();
		wasRemoveNotified_ = 1;
	}
	QMainWindow::closeEvent(e); //take care of actual closing
	return;  /* "Proceed with closing window." */
}

void qGraphDialog::scaleUpdated( void ) {
	/* This routine is called whenever some external event has changed the
	 *  Y scale values and other such stuff in the graph data token.
	 *  This routine then updates the scale toolbar (tbar2_) to reflect
	 *  the new state of the data token.
	 * NOTE: this routine also enforces "rounding": if isRounded_ is set,
	 *  this routine coerces YMin and YMax to "natural-seeming" values.
	 *  The definition of "natural-seeming" is inherently esthetic.
	 */
	char buf[30];
	double ymin, ymax, yabs;

	// Get YMax, round it if necessary, and format it as a string.
	if (edYMax_ != NULL && edYMin_ != NULL) {
		ymax = dataToken_->getYMax();
		yabs = fabs(ymax);
		if (isRounded_ && !lock_ && yabs > 1.0 && yabs < 1e20) {
			if (ymax > 0)
				ymax = roundUp( ymax );
			else
				ymax = -roundDown( -ymax );
			dataToken_->setYMax( ymax );
			gp_->repaint();
		}
		if (fabs(ymax) > 1e10)
			sprintf( buf, "%.5e", ymax );
		else
			sprintf( buf, "%.2f", ymax );
		edYMax_->setText( buf );
        
	// Get YMin, round it if necessary, and format it as a string.

		ymin = dataToken_->getYMin();
		yabs = fabs(ymin);
		if (isRounded_ && !lock_ && yabs > 1.0 && yabs < 1e20) {
			if (ymin > 0)
				ymin = roundDown( ymin );
			else
				ymin = -roundUp( -ymin );
			dataToken_->setYMin( ymin );
			gp_->repaint();
		}
		if (fabs(ymin) > 1e10)
			sprintf( buf, "%.5e", ymin );
		else
			sprintf( buf, "%.2f", ymin );
		edYMin_->setText( buf );

		/* If this dialog is displaying a "groupable" data token, and the
		 *  group button is on, and we are not recursing on a grouped setting
		 *  from some other dialog, then set the group scale values and signal
		 *  the event that causes all other dialogs in the group to use this
		 *  scale also.
		 */
		if (isGrouped_ && !lock_ ) {
			lock_ = TRUE;
			dataToken_->setGroupYMax( ymax );
			dataToken_->setGroupYMin( ymin );
			dataToken_->cbGroupYScaleChanged();
			
			lock_ = FALSE;
		}
	}
}

double qGraphDialog::roundUp( double y ) {
	double decade, ynew;
	int idec, i;

	decade = log10( y );
	idec = (int)decade;
	for (i = 0, decade = 1.0; i < idec; i++)
		decade *= 10.0;
	for (ynew = decade; ynew < y; ynew += decade)
		continue;
	return ynew;

}

double qGraphDialog::roundDown( double y ) {
	double decade, ynew;
	int idec, i;

	decade = log10( y );
	idec = (int)decade;
	for (i = 0, decade = 1.0; i < idec; i++)
		decade *= 10.0;
	for (ynew = decade; ynew + decade < y; ynew += decade)
		continue;
	return ynew;
}

void qGraphDialog::decreaseScale( void ) {
	double y;

	isAutoscale_ = 0;
	bbFix_->setOn( TRUE );
	dataToken_->setAllowAutoscale( 0 );

	y = dataToken_->getYMax();
	y /= 2;
	dataToken_->setYMax( y );
	y = dataToken_->getYMin();
	y /= 2;
	dataToken_->setYMin( y );

	scaleUpdated();
	gp_->repaint();
}

void qGraphDialog::increaseScale( void ) {
	double y;

	isAutoscale_ = 0;
	bbFix_->setOn( TRUE );
	dataToken_->setAllowAutoscale( 0 );

	y = dataToken_->getYMax();
	y *= 2;
	dataToken_->setYMax( y );
	y = dataToken_->getYMin();
	y *= 2;
	dataToken_->setYMin( y );

	scaleUpdated();
	gp_->repaint();
}
 
void qGraphDialog::YMinMaxKillFocus(){
	typedYMinMax();	

}


void qGraphDialog::typedYMinMax( void ) {
	double ymin, ymax;
	QString cstr;

    // The edit fields have seen an ENTER key or lost focus, or something.
    if (dataToken_ == NULL)
		return;

    if (!edYMax_->isModified() && !edYMin_->isModified())
        return;

	// The user typed in a new value for Ymin or Ymax.
	isAutoscale_ = 0;
	bbFix_->setOn( TRUE );
	dataToken_->setAllowAutoscale( 0 );

	cstr = edYMax_->text();
	ymax = atof( cstr.ascii() );
	cstr = edYMin_->text();
	ymin = atof( cstr.ascii() );

	if (ymin >= ymax)
		ymax = ymin + 10.0;  // KLUDGE

	dataToken_->setYMax( ymax );
	dataToken_->setYMin( ymin );
	scaleUpdated();
	gp_->repaint();
}

//A slot now that is activated when the groups y scale changes
void qGraphDialog::cbGroupYScaleChanged(  ){

	if (isGrouped_ && !lock_ && dataToken_ != NULL) {
		lock_ = TRUE;
		dataToken_->setYMax( dataToken_->getGroupYMax() );
		dataToken_->setYMin( dataToken_->getGroupYMin() );
		gp_->repaint();
		scaleUpdated();
		lock_ = FALSE;
	}
}

void qGraphDialog::setDataToken( qGraphDataToken *dt ) {
	dataToken_ = dt;
	if (dataToken_ != NULL) {
		connect(dt, SIGNAL(groupScaleUpdated()),this, SLOT(groupDataChanged()));
	}
}

void qGraphDialog::groupDataChanged(){
	cbGroupYScaleChanged();
}



void qGraphDialog::setTitleLabel(const char *label){
	if(label){
		setCaption(label);
		tLabel_ = label;
		stTitle_->setText(label);		
	}
}

int qGraphDialog::addLegendLabel(const char *cs, QColor *zc, int labelNum){

	QWidget * label = new QWidget(mainWidget);
	QHBoxLayout * layout = new QHBoxLayout(label,0,5);
	legendLayout->addWidget(label);
	label->setSizePolicy(  QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed ));
	
	label->show();

	QWidget *bb= new QWidget(label );	
	bb->resize( 15,15 );
	bb->setSizePolicy(  QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed ));				
	bb->setMinimumSize( QSize(15,15) );
	layout->addWidget(bb);
		
	bb->setPaletteBackgroundColor(*zc);
	bb->show();
	
	QLabel *ll = new QLabel(cs, label); 
	QFont f = ll->font();
	f.setBold(true);
	ll->setFont(f);

	ll->updateGeometry();
	ll->setSizePolicy(  QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));				
	layout->addWidget(ll);
	
	ll->show();
	
	label->updateGeometry();
	
	if(labelNum==-1){
		colorList_.append(zc);
		legendList_.append(new QString(cs));
	}
	else{
		colorList_.insert(labelNum,zc);
		legendList_.insert(labelNum, new QString(cs));
	}
	return 1;
}

void qGraphDialog::drawLegend(void){

	
	if(!colorList_.count()) return;

	QBrush b;
	int xShift=50;
	QSize textArea, vCharSize;
	gp_->painter->setPen(QPen(QColor(black),1, SolidLine));

	vCharSize = (gp_->painter->fontMetrics()).size(SingleLine, "X",1);
	QRect r = gp_->getGraphRegion(
                    vCharSize.width(), vCharSize.height() );

	//Iterate through each label in the legend
	for(size_t i=0;i<colorList_.count();i++){
	
		
		b = QBrush(*colorList_.at(i));
		gp_->painter->fillRect(r.left()+xShift, r.bottom()+2*vCharSize.height(), vCharSize.height(),vCharSize.height(),b);
		
		xShift += vCharSize.height()*2 + 5;
		textArea = (gp_->painter->fontMetrics()).size(SingleLine, *legendList_.at(i), legendList_.at(i)->length());
		gp_->painter->drawText(r.left()+xShift, (int)(r.bottom()+2.8*textArea.height()), *legendList_.at(i));
		xShift += textArea.width() + xShift;  //add xshift to the shift width to separate labels

	}
	//Draw the Graphs Title:
	textArea = (gp_->painter->fontMetrics()).size(SingleLine, tLabel_, strlen(tLabel_) );
	//Center gaph label, add some border between it and the grpah drawing area  (1/2 line height)
	gp_->painter->drawText((r.left()+r.right())/2-textArea.width()/2 , r.top()-textArea.height()/2, tLabel_);
	
	return; 
	
}

double qGraphDialog::getYpercent(int y){

	if(y<graphArea_.top()) 
		return 1.0;
	if(y>graphArea_.bottom()) 
		return 0.0;
	return 1.0-(double(y-graphArea_.top())/double(graphArea_.bottom()-graphArea_.top()));
	
}

double qGraphDialog::getXpercent(int x){
	
	if(x<graphArea_.left()) 
		return 1.0;
	if(x>graphArea_.right()) 
		return 0.0;
	return 1.0-(double(x-graphArea_.left())/double(graphArea_.right()-graphArea_.left()));
		
}

int qGraphDialog::setCurrentPen(int colorNum){
	return 1;
}

//These methods are deprecated, but left in for compatibility
void qGraphDialog::lockCanvas(){
}

void qGraphDialog::unlockCanvas(){

}



int
qGraphDialog::setPositionYLabel(const char *pstr){
	stPositionY_->setText(pstr);
	return 0;
}

int
qGraphDialog::setPositionXLabel(const char *pstr){
	stPositionX_->setText(pstr);
	return 0;
}

int
qGraphDialog::setPositionDateLabel(const char *pstr){
	if (stDateX_ != NULL)
		stDateX_->setText(pstr);
	return 0;
}

/* Handle the Static Combobox which may or may not be visible. */

void qGraphDialog::addComboBoxText(const char *text){
	if (cmbStatic_ != NULL)
		cmbStatic_->insertItem(text);
}

int qGraphDialog::getComboBoxText( QString &dest, int i ) {
	if (cmbStatic_ == NULL)
		return -1;
	cmbStatic_->setCurrentItem ( i );
	dest = cmbStatic_->currentText();
	return (dest != QString::null);
}

void qGraphDialog::insertComboBoxText( const char *text, int pos ) {
	if (cmbStatic_ != NULL)
	    cmbStatic_->insertItem( text, pos );
}

int qGraphDialog::selectComboBox(int i){ 
	if (cmbStatic_ == NULL)
		return -1;
	if((i!=-1) && (!( cmbStatic_->count()) || (i >= cmbStatic_->count()))) return 0;
	if(i!=-1){
		 cmbStatic_-> setCurrentItem(i);
		 return i ;
	}
	return cmbStatic_->currentItem();
}

void qGraphDialog::cbCombobox(int selection ){
	//evCombobox_.notify();
	emit evCombobox_();

	return ;
}


void qGraphDialog::FixChanged(){
	isAutoscale_ = !bbFix_->state ();
	dataToken_->setAllowAutoscale( isAutoscale_ );
	if (isAutoscale_) {
		dataToken_->setYBounds( 1 );
		scaleUpdated();
		gp_->repaint();
	}
}	

void qGraphDialog::upArrow(){
	increaseScale();
}
void qGraphDialog::downArrow(){
	decreaseScale();
}

void qGraphDialog::roundButton(){
	
	isRounded_ = bbRound_->state ();
	if (isRounded_)
		scaleUpdated();
	else if (isAutoscale_) {
		dataToken_->setYBounds( 1 );
		gp_->repaint();
		scaleUpdated();
	}
}

void qGraphDialog::graphViewChange(int id){

	switch(id){
		case ID_CONDOTS:
			gp_->drawMethod( CONNECTDOTS );
			gp_->repaint();
			break;

		case ID_CITYSCAPE:
			gp_->drawMethod( CITYSCAPE );
			gp_->repaint();
			break;

		case ID_HOLLOWBARS:
			gp_->drawMethod( HOLLOWBARS );
			gp_->repaint();
			break;

		case ID_FILLEDBARS:
			gp_->drawMethod( FILLEDBARS );
			gp_->repaint();
			break;
	}
}

void qGraphDialog::okPressed(){
		emit evOK_();
		emit evRemove_();
		wasRemoveNotified_ = 1;
		close();				
}

void qGraphDialog::savePressed(){
	emit evSave_();  /* Someone else knows how to save data to file. */
}
 
void qGraphDialog::helpPressed(){
	emit evHelp_();
}

void qGraphDialog::wandPressed(){
	isActive_ = pbWand_->isOn();
}

void qGraphDialog::printPressed(){
		printMe();
}

void qGraphDialog::cancelPressed(){
	close();	
}

void qGraphDialog::applyPressed(){
	emit evApply_();		
}

void qGraphDialog::resetPressed(){
	emit evReset_();
}

void qGraphDialog::applyAllPressed(){
	emit evApplyAll_();
}

void qGraphDialog::resetAllPressed(){
	emit evResetAll_();
}

void qGraphDialog::schedulePressed(){
	emit evSchedule_();
}


void qGraphDialog::groupPressed(){

	isGrouped_ = bbGroup_->state ();
	if (isGrouped_) {
			/* If this is the first time this group's group scale function
			 *  has been used, impose a group scale. Otherwise, use the
			 *  existing group scale.
			 */
			double gymin, gymax;
			gymin = dataToken_->getGroupYMin();
			gymax = dataToken_->getGroupYMax();
			if (gymin == gymax) {
				dataToken_->setGroupYMax( dataToken_->getYMax() );
				dataToken_->setGroupYMin( dataToken_->getYMin() );
			}
			else 
				cbGroupYScaleChanged();
	
	}
	else {
		/* If the group button is unchecked, the graph dialog goes about
		 *  it's own business again.
		 */
		if (isAutoscale_) {
			dataToken_->setYBounds( 1 );
			scaleUpdated();
			gp_->repaint();
		}
	}
}

