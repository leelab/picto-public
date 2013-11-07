#include <QtWidgets>
#include <QMetaObject>
#include "PropertyGroupWidget.h"
#include "PropertyListWidget.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;
;
//! [0]
PropertyGroupWidget::PropertyGroupWidget(bool trackInitVals, QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	mainWidget_(NULL),
	editorState_(editorState),
	trackInitVals_(trackInitVals)
{
	setLayout(new QVBoxLayout());
	layout()->setContentsMargins(QMargins(0,0,0,0));
}

PropertyGroupWidget::~PropertyGroupWidget()
{
	clear();
}

void PropertyGroupWidget::addProperties(QString title, QVector<QSharedPointer<Property>> props)
{
	//If the main widget wasn't yet created, create it and set it up to recieve sub widgets
	if(!mainWidget_)
	{
		mainWidget_ = new QWidget();
		layout_ = new QVBoxLayout();
		mainWidget_->setLayout(layout_);
		layout()->addWidget(mainWidget_);
	}

	PropertyListWidget* propList = new PropertyListWidget(trackInitVals_,editorState_);
	connect(propList,SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),this,SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)));

	//Add the newly created browser to a layout
	QVBoxLayout* browserLayout = new QVBoxLayout();
	browserLayout->addWidget(propList,1);
	//browserLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);//This makes sure resizing of internal widgets is handled correctly
	browserLayout->setContentsMargins(QMargins(0,0,0,0));
	
	//Create a box to contain the browser, titled with the input title
	QGroupBox* browserBox = new QGroupBox(title);
	browserBox->setLayout(browserLayout);
	browserBox->setFlat(true);	//This causes only the top line of the box to be shown (saving space)
	
	//Add the browser box to the end of the mainWidget layout
	layout_->addWidget(browserBox);
	layout_->addStretch(10000);
	mainWidget_->setLayout(layout_);

	//Add properties to the props list
	propList->addProperties(title,props);

	mainWidget_->show();
}

void PropertyGroupWidget::clear()
{
	if(mainWidget_)
	{
		mainWidget_->setParent(NULL);
		delete mainWidget_;
		mainWidget_ = NULL;
	}

}