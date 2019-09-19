/****************************************************************************
** Form interface generated from reading ui file 'mouse_tool_base.ui'
**
** Created: Sat Mar 5 16:04:16 2005
**      by: The User Interface Compiler ($Id: mouse_tool_base.h,v 1.1.1.1 2005/07/18 18:35:07 jkrinke Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MOUSE_TOOL_BASE_H
#define MOUSE_TOOL_BASE_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QComboBox;
class QPushButton;

class Mouse_Tool_Base : public QDialog
{
    

public:
    Mouse_Tool_Base( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~Mouse_Tool_Base();

    QLabel* textLabel7;
    QComboBox* comboBox1_0;
    QComboBox* comboBox4_3;
    QLabel* textLabel2;
    QComboBox* comboBox4_2;
    QLabel* textLabel1;
    QComboBox* comboBox3_3;
    QLabel* textLabel6;
    QComboBox* comboBox2_0;
    QComboBox* comboBox3_0;
    QComboBox* comboBox1_2;
    QComboBox* comboBox1_3;
    QComboBox* comboBox4_0;
    QComboBox* comboBox2_3;
    QLabel* textLabel5;
    QLabel* textLabel3;
    QComboBox* comboBox3_2;
    QComboBox* comboBox2_2;
    QLabel* textLabel4;
    QPushButton* pushButton2;
    QPushButton* pushButton1;

protected:
    QGridLayout* layout16;


    virtual void languageChange();

};

#endif // MOUSE_TOOL_BASE_H
