#include <QCheckBox>
#include <QVBoxLayout>
#include <QAbstractButton>
#include "TaskSelectorWidget.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
TaskSelectorWidget::TaskSelectorWidget(QWidget *parent) :
	QWidget(parent)
{
	buttonGroup_ = new QButtonGroup(this);
	connect(buttonGroup_,SIGNAL(buttonClicked(int)),this,SIGNAL(runSelected(int)));
	clear();
}

void TaskSelectorWidget::addRun(QIcon& icon,QString runName,int index)
{
	//Add new buttons to layout
	QCheckBox* taskCheckbox(new QCheckBox(runName));
	taskCheckbox->setIcon(icon);
	buttonGroup_->addButton(taskCheckbox,index);
	qobject_cast<QVBoxLayout*>(layout())->addWidget(taskCheckbox,0,Qt::AlignTop);
	int numButtons = buttonGroup_->buttons().size();
	qobject_cast<QVBoxLayout*>(layout())->setStretch(numButtons-1,1);
	if(numButtons > 1)
		qobject_cast<QVBoxLayout*>(layout())->setStretch(numButtons-2,0);
	//qobject_cast<QVBoxLayout*>(layout())->addStretch(1);
}

void TaskSelectorWidget::clear()
{
	//Remove all old buttons from layout.  We do this in a roundabout but simple way.  Moving a widget's
	//layout to another widget and then destroying that widget destroys the layout and all the children that
	//were attached to it.
	if(layout())
		QWidget().setLayout(layout());
	setLayout(new QVBoxLayout());
	QList<QAbstractButton*> managedButtons = buttonGroup_->buttons();
	foreach(QAbstractButton* button,managedButtons)
	{
		buttonGroup_->removeButton(button);
	}
}

int TaskSelectorWidget::getSelectedRun()
{
	return buttonGroup_->checkedId();
}