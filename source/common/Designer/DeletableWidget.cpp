#include <QtWidgets>
#include "DeletableWidget.h"
#include "../../common/storage/datastore.h"
#include "../../common/property/Property.h"
#include "../../common/memleakdetect.h"
using namespace Picto;

//! [0]
DeletableWidget::DeletableWidget(QSharedPointer<EditorState> editorState, QtVariantPropertyManager* manager, QtProperty* property, QWidget* child, QWidget *parent) :
	editorState_(editorState),
	QWidget(parent),
	manager_(manager),
	property_(property),
	child_(child)
{
	if(!child)
		return;
	//Q_ASSERT(child);
	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(child_,0,Qt::AlignRight);
	QToolButton *button = new QToolButton;
	QSharedPointer<QButtonGroup> buttonGroup(new QButtonGroup());
	buttonGroup->addButton(button);
	button->setIcon(QIcon(":/icons/delete.png"));
	button->setIconSize(QSize(10,10));
	layout->addWidget(button);
	layout->setContentsMargins(0,0,0,0);
	setLayout(layout);
	connect(buttonGroup.data(), SIGNAL(buttonClicked(int)),this, SLOT(triggered(int)));
}

void DeletableWidget::triggered(int)
{
	QSharedPointer<DataStore> dataStore = editorState_->getSelectedAsset().staticCast<DataStore>();
	Q_ASSERT(dataStore->getPropertyContainer()->getPropertyManager().data() == manager_);
	QSharedPointer<Property> prop = dataStore->getPropertyContainer()->getPropertyFromQtProperty(property_);
	prop->setDeleted();
	editorState_->triggerExperimentReset();
}