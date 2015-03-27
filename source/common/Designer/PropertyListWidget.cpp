#include <QtWidgets>
#include <QMetaObject>
#include <QSpacerItem>
#include "PropertyListWidget.h"
#include "../../common/storage/datastore.h"
#include "../../common/memleakdetect.h"
using namespace Picto;
;

/*! \brief Constructs a new PropertyListWidget
 *	\details trackInitVals indicates whether Property widgets created for this PropertyListWidget should set/show init
 *	values for the input Properties or if they should set/show saveValues for the input Properties.  editorState is
 *	passed into the PropertyEditorFactory constructor. We connect the PropertyEditorFactory::propertyEdited() signal
 *	to propertyWasEdited() so that we can update Property values when their widget's values are updated.
 */
 PropertyListWidget::PropertyListWidget(bool trackInitVals, QSharedPointer<EditorState> editorState, QWidget *parent) :
	QWidget(parent),
	mainWidget_(NULL),
	propertyFactory_(editorState),
	trackInitVals_(trackInitVals),
	browser_(new QtButtonPropertyBrowser(this))
{
	connect(&propertyFactory_, SIGNAL(propertyEdited(QSharedPointer<Property>,QVariant)),
        this, SLOT(propertyWasEdited(QSharedPointer<Property>,QVariant)));
	setLayout(new QVBoxLayout());
	layout()->addWidget(browser_);

	//This makes sure the resizing of internal widgets is handled correctly
	layout()->setSizeConstraint(QLayout::SetMinAndMaxSize);
	layout()->setContentsMargins(QMargins(0,0,0,0));

	//Set up the property browser by connecting the input propManager with the input propertyFactory such that
	// QtVariantProperties in the input property manager will have their widgets created by the input property factory
	// and displayed in the browser_.
	//See http://docs.huihoo.com/qt/solutions/4/qtpropertybrowser/qtabstractpropertybrowser.html#setFactoryForManager
	browser_->setFactoryForManager(&propManager_, &propertyFactory_);
}

PropertyListWidget::~PropertyListWidget()
{
	browser_->unsetFactoryForManager(&propManager_);
	browser_->clear();
	propManager_.clear();
	propertyFactory_.clear();
}

/*! \brief Creates widgets for the input Properties and displays them.
 *	\details This function separates out regular Property objects from Script Property objects. The details of actually
 *	creating the approriate widgets and displaying them are handled by addScriptProperty() and addProperty().
 */
void PropertyListWidget::addProperties(QString title, QVector<QSharedPointer<Property>> props)
{
	//Add each Property to the property browser one by one
	foreach(QSharedPointer<Property> prop,props)
	{
		QString propName = prop->getName();
		bool scriptProp = (propName == "EntryScript" || propName == "FrameScript" || propName == "ExitScript"
			|| propName == "AnalysisEntryScript" || propName == "AnalysisFrameScript" || propName == "AnalysisExitScript");
		if(scriptProp)
			addScriptProperty(prop,&propManager_,browser_);
		else
			addProperty(prop,&propManager_,browser_);
	}

	//Show the changes
	show();
}

/*! \brief Uses the QtPropertyBrowser infrastructure with the PropertyEditorFactory to create a widget tailored to the
 *	input Property and display it.
 *	\details Since we called
 *	\code browser_->setFactoryForManager(&propManager_, &propertyFactory_); \endcode
 *	in the constructor, QtVariantProperties created by the input manager will have their widgets created by our
 *	PropertyEditorFactory and displayed in the browser when we call
 *	\code browser->addProperty(item); \endcode.
 *	In this function, we first use the input manager to create a QtVariantProperty for the input Property, then set its
 *	value to the current value of the Property and add it to the browser.  This causes the PropertyEditorFactory to create
 *	a widget of the appropraite type for the QtVariantProperty.  We then connect the input Properties
 *	Property::valueChanged() or Property::initValueChanged() signal (according to trackInitVals) to
 *	propertyWasEditedExternally().  This allows us to update the QtVariantProperty value whenever the Property value
 *	changes so that our widget values will always be up to date.  In the opposite direction, just before creating each
 *	widget in the PropertyEditorFactory, we call PropertyEditorFactory::setNextProperty().  This allows it to track which
 *	QtVariantProperty is connected to which Picto Property.  Since we connected the PropertyEditorFactory::propertyEdited()
 *	signal to propertyWasEdited() in the constructor, whenever the PropertyEditorFactory detects that a widget value for a
 *	particular Propery was edited, we will find out about it and can the change the Property value accordingly in that
 *	function. This all seems very convoluded, but doing things this way allows us to make use of the QtPropertyBrowser
 *	system which gives us a lot of nice automatically generated widget functionality for free.  Someday when someone has
 *	time we should probably just implement all of this ourselves and get rid of the complexity.
 *	\note There is another possible source of confusion here which is that when we are not using trackInitVals_ we
 *	set/get the Property value and not the Property saveValue.  This has to do with the fact that when set Properties
 *	into runMode, value() references the Property's runValue but when it isn't in runMode value() references the
 *	Property's saveValue.  For more detail see Property.
 *	\sa http://docs.huihoo.com/qt/solutions/4/qtpropertybrowser/qtabstractpropertybrowser.html#setFactoryForManager
*/
void PropertyListWidget::addProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtAbstractPropertyBrowser* browser)
{
	int propertyType = prop->type();
	if(propertyType == PropertyContainer::enumTypeId())
		propertyType = QtVariantPropertyManager::enumTypeId();
	QString propName = prop->getName();
	QtVariantProperty *item = NULL;
	item = manager->addProperty(propertyType,prop->getName());
	foreach(QString attr,prop->getAttributes())
	{
		item->setAttribute(attr,prop->attributeValue(attr));
	}
	item->setValue(trackInitVals_?prop->initValue():prop->value());
	propertyFactory_.setNextProperty(prop);
	browser->addProperty(item);
	if(trackInitVals_)
		connect(prop.data(),SIGNAL(initValueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	else
		connect(prop.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	propToQtPropHash_[prop.data()] = QPair<QtVariantProperty*,QSharedPointer<Property>>(item,prop);
}

/*! \brief Works like addProperty() except that ScriptProperties come with a button that allows them to be opened or
 *	closed so as to save space in the UI.
 *	\details To implement the open/close button we just create a group QtVariantProperty in the QtVariantPropertyManager
 *	and then add the QtVariantProperty representing the script as a sub-QtVariantProperty of that group QtVariantProperty.
 *	This causes the property to appear "within" a button when we add the group to the browser.  
 *	\note This is probably very difficult to understand if you haven't read the documentation for addProperty() yet.
 */
void PropertyListWidget::addScriptProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtAbstractPropertyBrowser* browser)
{
	QtVariantProperty *groupItem = NULL;
	QtVariantProperty *item = NULL;
	groupItem = manager->addProperty( QtVariantPropertyManager::groupTypeId(), prop->getName());
	item = manager->addProperty(prop->type(),"");
	groupItem->insertSubProperty(item,0);
	item->setValue(trackInitVals_?prop->initValue():prop->value());
	propertyFactory_.setNextProperty(prop);
	QtBrowserItem* browserItem = browser->addProperty(groupItem);

	if(!item->value().toString().trimmed().isEmpty())
	{	//When first opening scripts in a property bar, if they have contents, expand them.
		Q_ASSERT(qobject_cast<QtButtonPropertyBrowser*>(browser));
		qobject_cast<QtButtonPropertyBrowser*>(browser)->setExpanded(browserItem, true);
	}
	if(trackInitVals_)
		connect(prop.data(),SIGNAL(initValueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	else
		connect(prop.data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propertyWasEditedExternally(Property*,QVariant)));
	propToQtPropHash_[prop.data()] = QPair<QtVariantProperty*,QSharedPointer<Property>>(item,prop);

}

/*! \brief Called whenever the PropertyEditorFactory reports that the value in the widget for the input Property changed.  
 *	Sets the new value to the input Property.
 *	\note To actually call the function that sets the value into the input Property, we use QMetaObject::invokeMethod() 
 *	instead of calling the Property::setValue() or Property::setInitValue() methods directly.  This is so that we won't 
 *	encounter any multithreaded write issues.  By using this method, if the properties are in another thread, the function 
 *	call will be scheduled to run in that thread's event loop automatically.  If the property is in the current thread, the 
 *	function will be called immediately.  This is one of the few places where the UI thread and an experiment that might 
 *	possibly be in a separate thread need to communicate.  In the end, I'm not sure that this was absolutely necessary since
 *	we show the PropertyListWidget in the Test Viewer and Remote Viewer and only in the Playback Viewer is the experiment
 *	running a separate thread; however, we may want to put things into different threads in the remote viewer and test viewer
 *	in the future (although, we would probably be better off adjusting the experimental run paradigm to be based on the Qt
 *	event loop).
 */
void PropertyListWidget::propertyWasEdited(QSharedPointer<Property> prop,QVariant val)
{
	//We use QMetaObject::invokeMethod() below instead of calling the methods directly
	//so that we won't encounter any multithreaded write issues.  By using this method,
	//if the properties are in another thread, the function call will be scheduled to
	//run in that thread's event loop automatically.  If the property is in the current
	//thread, the function will be called immediately.  This is one of the few places
	//where the UI thread and an experiment that might possibly in a separate thread need
	//to communicate.
	if(trackInitVals_)
		QMetaObject::invokeMethod(prop.data(),"setInitValue",Q_ARG(QVariant,val));
	else
		QMetaObject::invokeMethod(prop.data(),"setValue",Q_ARG(QVariant,val));
	emit propertyEdited(prop,val);
}

/*! \brief Called when a Property was edited.  Sets the new value of the Property to the corresponding QtVariantProperty
 *	whose value is driving the Property's Widget.
 *	\sa addProperty()
 */
void PropertyListWidget::propertyWasEditedExternally(Property* prop,QVariant val)
{
	Q_ASSERT(propToQtPropHash_.contains(prop));
	QtVariantProperty *qtProp = propToQtPropHash_.value(prop).first;
	Q_ASSERT(browser_);
	browser_->setCurrentItem(NULL);
	qtProp->setValue(val);
}