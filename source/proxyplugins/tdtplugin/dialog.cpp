#include <QtGui>

#include "dialog.h"
#include "../../common/memleakdetect.h"

Dialog::Dialog(QWidget *parent) :	
  QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
	//set up the dialog box
	serverLine = new QLineEdit(tr("local"));
	tankLine = new QLineEdit("Quincy_OFC");
	blockLine = new QLineEdit("Block-225");

	okButton = new QPushButton(tr("OK"));
	okButton->setDefault(true);
	connect(okButton,SIGNAL(clicked()),this,SLOT(accept()));

	//use layouts to arrange the dialog box
	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&Server"),serverLine);
	formLayout->addRow(tr("&Tank"),tankLine);
	formLayout->addRow(tr("&Block"),blockLine);

	QVBoxLayout *dialogLayout = new QVBoxLayout;
	dialogLayout->addLayout(formLayout);
	dialogLayout->addWidget(okButton);

	setLayout(dialogLayout);
}