#include "qHelpWindow.h"
#include <qstatusbar.h>
#include <qpixmap.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qiconset.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstylesheet.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qobjectlist.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qprinter.h>
#include <qsimplerichtext.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qimage.h>
#include <qdragobject.h>
#include <qsplitter.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

#include <ctype.h>

static char*back[]={
"16 16 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"................",
".......#........",
"......##........",
".....#a#........",
"....#aa########.",
"...#aabaaaaaaa#.",
"..#aabbbbbbbbb#.",
"...#abbbbbbbbb#.",
"...c#ab########.",
"....c#a#ccccccc.",
".....c##c.......",
"......c#c.......",
".......cc.......",
"........c.......",
"................",
"......................"};

static char*forward[]={
"16 16 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"................",
"................",
".........#......",
".........##.....",
".........#a#....",
"..########aa#...",
"..#aaaaaaabaa#..",
"..#bbbbbbbbbaa#.",
"..#bbbbbbbbba#..",
"..########ba#c..",
"..ccccccc#a#c...",
"........c##c....",
"........c#c.....",
"........cc......",
"........c.......",
"................",
"................"};

// Include compiled help files
extern "C" {
#include "help/output/help_include.h"
}

/* Subclass QMimeSourceFactory so that it loads from compiled-in
 * help only. Additionally, the files are not loaded until they
 * are requested */
class qHelpMimeSourceFactory : public QMimeSourceFactory {
public:
	qHelpMimeSourceFactory();
	~qHelpMimeSourceFactory();
	const QMimeSource* data(const QString& abs_name) const; // retrieve data
	void setData(const QString& abs_name, QMimeSource* data) const; // store data
private:
	class qHelpMimeSourceFactoryPrivate {
	public:
		QMap<QString, QMimeSource*> store; /* Stores loaded data */
	};
	qHelpMimeSourceFactoryPrivate* d; /* This seems necessary to subclass QMimeSourceFactory in Qt */
};
qHelpMimeSourceFactory::qHelpMimeSourceFactory() {
	d = new qHelpMimeSourceFactoryPrivate();
}
qHelpMimeSourceFactory::~qHelpMimeSourceFactory() {
	delete d;
}
const QMimeSource* qHelpMimeSourceFactory::data(const QString& abs_name) const {
	QMimeSource* src = d->store[abs_name];
	if (src) return src;

	for (int i=0; help_index_names[i] != NULL; i++) {
		if (abs_name == help_index_names[i]) {
			// Read file into buffer
			/* This byte array is never deleted, but this makes sense because
			 * the help contents are kept in memory until the program is
			 * closed, at which point all used memory is freed */
			QByteArray* buffer = new QByteArray(help_index_lengths[i]);
			buffer->setRawData(help_index[i], help_index_lengths[i]);

			QString filename = QString(help_index_names[i]);
			QImage image = QImage(*buffer);
			// Store data in mime
			if (image.isNull()) {
				// Text
				src = new QTextDrag(QString(*buffer));
			}
			else {
				// Image
				src = new QImageDrag(image);
			}

			setData(abs_name, src);
			return src;
		}
	}
	return NULL;
}
void qHelpMimeSourceFactory::setData(const QString& abs_name, QMimeSource* data) const {
    if (d->store.contains(abs_name)) delete d->store[abs_name];
    d->store.replace(abs_name,data);
}

// Table of Contents reader
#define	TOC_FILENAME	"crisp.toc"
#define TOC_ROOT_NODE_NAME	"Table of Contents"
#define	TOC_OUTSIDE	0
#define	TOC_TOC	1
/* Keywords are actually ignored for the time being;
 * The search merely matches the entered words with
 * the page titles */
#define	TOC_KEYWORD	2

/* Subclass QXmlDefaultHandler to read in the contents/index
 * This is a pretty simple SAX-like parser using Qt's XML API
 * which is supposedly based on Java's XML API
 *
 * The XML format is very simple, and looks like this:
 * <toc version="1.0">
 * <section ref="1.html" title="Page Title">
 *  <section ref="2.html" title="Subsection"/>
 * </section>
 * <section ref="3.html" title"Another top-level section"/>
 * </toc>
 * Note that there are <keyword> elements in help/crisp.toc but
 * these are actually ignored for now */
class qHelpContentsReader : public QXmlDefaultHandler {
public:
	qHelpContentsReader(qHelpWindow* parent);
	bool startElement (const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts);
	bool endElement (const QString & namespaceURI, const QString & localName, const QString & qName);
private:
	qHelpWindow* parent;
	QPtrStack<QListViewItem> sections; // Table of Contents Sections
	QPtrStack<QListViewItem> last; // Last item in section (this is needed because Qt adds items to the TOP of a QListView by default)
	int state; // internal state
};

qHelpContentsReader::qHelpContentsReader(qHelpWindow* parent) {
	this->parent = parent;
}
bool qHelpContentsReader::startElement (const QString & namespaceURI, const QString & localName, const QString & qName, const QXmlAttributes & atts) {
	switch (state) {
	case TOC_TOC:
		if (localName == "section") {
			// Start of a new section
			QListViewItem* section = new QListViewItem(sections.top(), last.top(), atts.value("title"));
			// Set last element
			last.pop();
			last.push(section);
			// Push another level onto the stacks
			last.push(section);
			sections.push(section);

			// Store connection between filename and title here, without overwrite
			parent->fileToSection.insert(atts.value("ref"), atts.value("title"), FALSE);
			parent->sectionToFile.insert(atts.value("title"), atts.value("ref"), FALSE);
		}
	break;
	case TOC_KEYWORD: /* Not currently used */
	break;
	default:
		if (localName == "toc") {
			state = TOC_TOC;

			// Push root node
			sections.push(new QListViewItem(parent->tree, TOC_ROOT_NODE_NAME));
			sections.top()->setOpen(true);
			parent->fileToSection.insert("1.html", TOC_ROOT_NODE_NAME);
			parent->sectionToFile.insert(TOC_ROOT_NODE_NAME, "1.html");
			last.push(sections.top());
		}
	}
	return true;
}
bool qHelpContentsReader::endElement (const QString & namespaceURI, const QString & localName, const QString & qName) {
	switch (state) {
	case TOC_TOC:
		if (localName == "toc") {
			state = TOC_OUTSIDE;

			// Pop root node
			sections.pop();
			last.pop();
		}
		else if (localName == "section") {
			// End of section
			sections.pop();
			last.pop();
		}
	break;
	case TOC_KEYWORD:
	break;
	default:
	break;
	}
	return true;
}


qHelpWindow::qHelpWindow( const QString& home_,
			QWidget* parent, const char *name )
    : QMainWindow( parent, name )/*,
      pathCombo( 0 )*/
{
	// Load help data into mime
	mime = new qHelpMimeSourceFactory();

	/* Set up draggable boundary between tree and browser */
	QSplitter *hbox = new QSplitter(this);
	setCentralWidget(hbox);

	/* Create tree for contents */
	tree = new QListView(hbox);
	tree->addColumn("Topic");
	tree->setSorting(-1); // disable sorting

	// Set up sizing
	tree->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, 2, 1));
	tree->setResizeMode(QListView::AllColumns);

	// Get TOC
	const QMimeSource* toc = mime->data(TOC_FILENAME);
	if (toc) {
		// Read in contents using XML parser
		QXmlSimpleReader* xml = new QXmlSimpleReader();
		qHelpContentsReader reader(this);
		xml->setContentHandler(&reader);
		QXmlInputSource xmlInput;
		xmlInput.setData(toc->encodedData("text/plain"));
		xml->parse(&xmlInput);
	}
	else {
		QMessageBox::critical(this, "Error", "Could not load table of contents!", QMessageBox::Ok, QMessageBox::NoButton);
		hbox->removeChild(tree);
	}

	/* Create browser */
    browser = new QTextBrowser(hbox);
	browser->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, 3, 1));
	browser->setMimeSourceFactory(mime);
    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    connect( browser, SIGNAL( sourceChanged(const QString& ) ),
	     this, SLOT( sourceChanged( const QString&) ) );

    if ( !home_.isEmpty() )
	browser->setSource( home_ );

    connect( browser, SIGNAL( highlighted( const QString&) ),
	     statusBar(), SLOT( message( const QString&)) );

    resize( 750,600 );

    QPopupMenu* file = new QPopupMenu( this );
    file->insertItem( tr("&Print"), this, SLOT( print() ), CTRL+Key_P );
    file->insertSeparator();
    file->insertItem( tr("&Close"), this, SLOT( close() ), CTRL+Key_Q );

    // The same two icons are used twice each.
    QIconSet icon_back( QPixmap((const char**) back) );
    QIconSet icon_forward( QPixmap((const char**) forward) );

    QPopupMenu* go = new QPopupMenu( this );
    backwardId = go->insertItem( icon_back,
				 tr("&Backward"), browser, SLOT( backward() ),
				 CTRL+Key_Left );
    forwardId = go->insertItem( icon_forward,
				tr("&Forward"), browser, SLOT( forward() ),
				CTRL+Key_Right );
    go->insertItem( tr("&Contents"), browser, SLOT( home() ) );
    go->insertItem( tr("&Search"), this, SLOT( search() ) );

    menuBar()->insertItem( tr("&File"), file );
    menuBar()->insertItem( tr("&Go"), go );

    menuBar()->setItemEnabled( forwardId, FALSE);
    menuBar()->setItemEnabled( backwardId, FALSE);
    connect( browser, SIGNAL( backwardAvailable( bool ) ),
	     this, SLOT( setBackwardAvailable( bool ) ) );
    connect( browser, SIGNAL( forwardAvailable( bool ) ),
	     this, SLOT( setForwardAvailable( bool ) ) );
    connect(browser, SIGNAL(sourceChanged(const QString&)), this, SLOT(browserSourceChanged(const QString&)));
	connect(tree, SIGNAL(currentChanged(QListViewItem*)), this, SLOT(treeCurrentChanged(QListViewItem*)));


    QToolBar* toolbar = new QToolBar( this );
    addToolBar( toolbar, "Toolbar");
    QToolButton* button;

    button = new QToolButton( icon_back, tr("Backward"), "", browser, SLOT(backward()), toolbar );
    connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( icon_forward, tr("Forward"), "", browser, SLOT(forward()), toolbar );
    connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( QPixmap(), tr("Contents"), "", browser, SLOT(home()), toolbar );
	button->setUsesTextLabel(true);
    button = new QToolButton( QPixmap(), tr("Search"), "", this, SLOT(search()), toolbar );
	button->setUsesTextLabel(true);

    browser->setFocus();

}

void qHelpWindow::setBackwardAvailable( bool b)
{
    menuBar()->setItemEnabled( backwardId, b);
}

void qHelpWindow::setForwardAvailable( bool b)
{
    menuBar()->setItemEnabled( forwardId, b);
}

void qHelpWindow::sourceChanged( const QString& url )
{
    if ( browser->documentTitle().isNull() )
	setCaption( "COMPASS Online Documentation" );
    else
	setCaption( "COMPASS Online Documentation: " + browser->documentTitle() ) ;
}

qHelpWindow::~qHelpWindow()
{
	if (mime != NULL) delete mime;
}

// This is copied from Trolltech's example code, so it should work. Print to file works
void qHelpWindow::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer( QPrinter::HighResolution );
    printer.setFullPage(TRUE);
    if ( printer.setup( this ) ) {
	QPainter p( &printer );
	if( !p.isActive() ) // starting printing failed
	    return;
	QPaintDeviceMetrics metrics(p.device());
	int dpiy = metrics.logicalDpiY();
	int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
	QRect body( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
	QSimpleRichText richText( browser->text(),
				  QFont(),
				  browser->context(),
				  browser->styleSheet(),
				  browser->mimeSourceFactory(),
				  body.height() );
	richText.setWidth( &p, body.width() );
	QRect view( body );
	int page = 1;
	do {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
	    if ( view.top()  >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (TRUE);
    }
#endif
}

void qHelpWindow::showPage(QString& page) {
	browser->setSource(page);
	show(); // If the page can't be found it will show up blank which is probably fine
}

// browser and tree notify each other of updates
void qHelpWindow::treeCurrentChanged(QListViewItem* item) {
	tree->setCurrentItem(item);
	tree->ensureItemVisible(item);
	browser->setSource(sectionToFile[item->text(0)]);
}
void qHelpWindow::browserSourceChanged(const QString & src) {
	QListViewItem* item = tree->findItem(fileToSection[src], 0);
	if (sectionToFile[tree->currentItem()->text(0)] != src) {
		tree->setCurrentItem(item);
		tree->ensureItemVisible(item);
	}
}

/* Implementation of searching */
// Search dialog creation
qHelpSearchDialog::qHelpSearchDialog(qHelpWindow* parent_)
: QDialog(parent_) {
	parent = parent_;

	QVBoxLayout *vbox = new QVBoxLayout(this, 6, 6);
	QLabel* label = new QLabel("Enter search terms to see related topics", this, "prompt_label");
	vbox->addWidget(label);

	lineEdit = new QLineEdit(this, "prompt_edit");
	vbox->addWidget(lineEdit);

	list = new QListView(this);
	list->addColumn("Related Topics");
	list->setResizeMode(QListView::AllColumns);
	vbox->addWidget(list);

	QHBoxLayout *hbox = new QHBoxLayout(6);
    vbox->addLayout(hbox, AlignRight);

    QPushButton *ok = new QPushButton("OK", this, "prompt_ok");
    ok->setDefault(TRUE);
    QPushButton *cancel = new QPushButton("Cancel", this, "prompt_cancel");

    QSize bs = ok->sizeHint().expandedTo(cancel->sizeHint());
    ok->setFixedSize(bs);
    cancel->setFixedSize(bs);

    hbox->addStretch();
    hbox->addWidget(cancel);
    hbox->addWidget(ok);

    connect(lineEdit, SIGNAL(returnPressed()),
	     this, SLOT(tryAccept()));
    connect(lineEdit, SIGNAL(textChanged(const QString&)),
	     this, SLOT(textChanged(const QString&)));
	connect(list, SIGNAL(doubleClicked ( QListViewItem *, const QPoint &, int )), this, SLOT(accept()));

    connect(ok, SIGNAL(clicked()), this, SLOT( accept()));
    connect(cancel, SIGNAL(clicked()), this, SLOT( reject()));

	// Resize and focus
    QSize sh = sizeHint().expandedTo(QSize(400, 10));
    resize(sh.width(), 300);
	lineEdit->setFocus();
	
	// Set up dialog
    setCaption("Search Online Documentation");

	textChanged("");
}
// Set up list
void qHelpSearchDialog::textChanged(const QString & str) {
	list->clear();
	for (QMap<QString, QString>::Iterator it = parent->sectionToFile.begin(); it != parent->sectionToFile.end(); it++) {
		if (it.key().contains(str, false)) new QListViewItem(list, it.key());
	}
	list->setSelected(list->firstChild(), true);
}
// Return selected topic
QString qHelpSearchDialog::getSelectedTopic() {
	QListViewItem* item = list->selectedItem();
	if (item) return item->text(0);
	else return "";
}

// Run a search dialog
void qHelpWindow::search() {
	qHelpSearchDialog dialog(this);
	if (dialog.exec() == QDialog::Accepted) {
		browser->setSource(sectionToFile[dialog.getSelectedTopic()]);
	}
}
