/****************************************************************************
** Form implementation generated from reading ui file 'mouse_tool_base.ui'
**
** Created: Sat Mar 5 16:04:35 2005
**      by: The User Interface Compiler ($Id: mouse_tool_base.cpp,v 1.1.1.1 2005/07/18 18:35:07 jkrinke Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "mouse_tool_base.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a Mouse_Tool_Base as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Mouse_Tool_Base::Mouse_Tool_Base( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "Mouse_Tool_Base" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 0, 0 ) );
    setMaximumSize( QSize( 750, 270 ) );
    setBaseSize( QSize( 0, 0 ) );

    QWidget* privateLayoutWidget = new QWidget( this, "layout16" );
    privateLayoutWidget->setGeometry( QRect( 0, 10, 650, 180 ) );
    layout16 = new QGridLayout( privateLayoutWidget, 1, 1, 11, 15, "layout16"); 

    textLabel7 = new QLabel( privateLayoutWidget, "textLabel7" );
    textLabel7->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    layout16->addWidget( textLabel7, 4, 0 );

    comboBox1_0 = new QComboBox( FALSE, privateLayoutWidget, "comboBox1_0" );

    layout16->addWidget( comboBox1_0, 1, 1 );

    comboBox4_3 = new QComboBox( FALSE, privateLayoutWidget, "comboBox4_3" );

    layout16->addWidget( comboBox4_3, 4, 3 );

    textLabel2 = new QLabel( privateLayoutWidget, "textLabel2" );
    textLabel2->setAlignment( int( QLabel::AlignCenter ) );

    layout16->addWidget( textLabel2, 0, 2 );

    comboBox4_2 = new QComboBox( FALSE, privateLayoutWidget, "comboBox4_2" );

    layout16->addWidget( comboBox4_2, 4, 2 );

    textLabel1 = new QLabel( privateLayoutWidget, "textLabel1" );
    textLabel1->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)5, (QSizePolicy::SizeType)1, 0, 0, textLabel1->sizePolicy().hasHeightForWidth() ) );
    textLabel1->setAlignment( int( QLabel::AlignCenter ) );

    layout16->addWidget( textLabel1, 0, 1 );

    comboBox3_3 = new QComboBox( FALSE, privateLayoutWidget, "comboBox3_3" );

    layout16->addWidget( comboBox3_3, 3, 3 );

    textLabel6 = new QLabel( privateLayoutWidget, "textLabel6" );
    textLabel6->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    layout16->addWidget( textLabel6, 3, 0 );

    comboBox2_0 = new QComboBox( FALSE, privateLayoutWidget, "comboBox2_0" );

    layout16->addWidget( comboBox2_0, 2, 1 );

    comboBox3_0 = new QComboBox( FALSE, privateLayoutWidget, "comboBox3_0" );

    layout16->addWidget( comboBox3_0, 3, 1 );

    comboBox1_2 = new QComboBox( FALSE, privateLayoutWidget, "comboBox1_2" );

    layout16->addWidget( comboBox1_2, 1, 2 );

    comboBox1_3 = new QComboBox( FALSE, privateLayoutWidget, "comboBox1_3" );

    layout16->addWidget( comboBox1_3, 1, 3 );

    comboBox4_0 = new QComboBox( FALSE, privateLayoutWidget, "comboBox4_0" );

    layout16->addWidget( comboBox4_0, 4, 1 );

    comboBox2_3 = new QComboBox( FALSE, privateLayoutWidget, "comboBox2_3" );

    layout16->addWidget( comboBox2_3, 2, 3 );

    textLabel5 = new QLabel( privateLayoutWidget, "textLabel5" );
    textLabel5->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    layout16->addWidget( textLabel5, 2, 0 );

    textLabel3 = new QLabel( privateLayoutWidget, "textLabel3" );
    textLabel3->setAlignment( int( QLabel::WordBreak | QLabel::AlignCenter ) );

    layout16->addWidget( textLabel3, 0, 3 );

    comboBox3_2 = new QComboBox( FALSE, privateLayoutWidget, "comboBox3_2" );

    layout16->addWidget( comboBox3_2, 3, 2 );

    comboBox2_2 = new QComboBox( FALSE, privateLayoutWidget, "comboBox2_2" );

    layout16->addWidget( comboBox2_2, 2, 2 );

    textLabel4 = new QLabel( privateLayoutWidget, "textLabel4" );
    textLabel4->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );

    layout16->addWidget( textLabel4, 1, 0 );

    pushButton2 = new QPushButton( this, "pushButton2" );
    pushButton2->setGeometry( QRect( 580, 200, 71, 30 ) );

    pushButton1 = new QPushButton( this, "pushButton1" );
    pushButton1->setGeometry( QRect( 470, 200, 91, 30 ) );
    languageChange();
    resize( QSize(661, 244).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
Mouse_Tool_Base::~Mouse_Tool_Base()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Mouse_Tool_Base::languageChange()
{
    setCaption( tr( "Mouse Tool" ) );
    textLabel7->setText( tr( "On Headwater" ) );
    textLabel2->setText( tr( "Middle Button" ) );
    textLabel1->setText( tr( "Left Buttton" ) );
    textLabel6->setText( tr( "On Reach" ) );
    textLabel5->setText( tr( "On Release" ) );
    textLabel3->setText( tr( "Right Button" ) );
    textLabel4->setText( tr( "On Dam" ) );
    pushButton2->setText( tr( "Help" ) );
    pushButton1->setText( tr( "Done" ) );
}

