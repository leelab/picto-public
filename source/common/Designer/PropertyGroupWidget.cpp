#include <QtWidgets>
#include <QMetaObject>
#include "PropertyGroupWidget.h"
#include "PropertyListWidget.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;
;

/*! \brief Constructs a new PropertyGroupWidget.
 *	\details trackInitVals and editorState are passed into the contained PropertyListWidgets.
*/
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

/*! \brief Adds the input list of Properties to this widget for display under the input title.
 *	\details Internally, the props list is passed into a PropertyListWidget which handles creation
 *	and layout of Property Widgets.  These are placed inside a titled group box that is added to this
 *	widget's layout.
*/
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
	browserLayout->addWidget(propList);
	browserLayout->setContentsMargins(QMargins(0,0,0,0));
	
	//Create a box to contain the browser, titled with the input title
	QGroupBox* browserBox = new QGroupBox(title);
	browserBox->setLayout(browserLayout);
	browserBox->setFlat(true);	//This causes only the top line of the box to be shown (saving space)
	
	//Add the browser box to the end of the mainWidget layout
	layout_->addWidget(browserBox);
	mainWidget_->setLayout(layout_);

	//Add properties to the props list
	propList->addProperties(title,props);

	mainWidget_->show();
}

/*! \brief Clears all contents of this widget, so that all Property widget groups will dissappear and calling 
*	addProperties() again will result in starting a new Properties widget group.
*/
void PropertyGroupWidget::clear()
{
	if(mainWidget_)
	{
		mainWidget_->setParent(NULL);
		delete mainWidget_;
		mainWidget_ = NULL;
	}

}