#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>

#include "dialog.h"
#include "../../common/memleakdetect.h"

/*! \brief Creates a new Dialog widget.
 *	\details This function sets up the Dialog widget's UI layout.
 */
Dialog::Dialog(QWidget *parent) :	
  QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint)
{
	//set up the dialog box
	serverLine = new QLineEdit(tr("tdt405"));

	okButton = new QPushButton(tr("OK"));
	okButton->setDefault(true);
	connect(okButton,SIGNAL(clicked()),this,SLOT(accept()));

	//use layouts to arrange the dialog box
	QFormLayout *formLayout = new QFormLayout;
	formLayout->addRow(tr("&Server"),serverLine);

	QVBoxLayout *dialogLayout = new QVBoxLayout;
	dialogLayout->addLayout(formLayout);
	dialogLayout->addWidget(okButton);

	setLayout(dialogLayout);
}