#include <QtGui>
#include "PropertyEditorFactory.h"
#include "../../common/storage/DataStore.h"
#include "../../common/property/PropertyContainer.h"
#include "ScriptWidget.h"
#include "DeletableWidget.h"
using namespace Picto;

//! [0]
PropertyEditorFactory::PropertyEditorFactory(QSharedPointer<EditorState> editorState,QWidget *parent) :
	editorState_(editorState),
	QtVariantEditorFactory(parent),
	childPropWasChanged_(false)
{

}

QWidget* PropertyEditorFactory::createEditor (QtVariantPropertyManager* manager, QtProperty* property, QWidget* parent)
{
	QWidget* resultWidget = NULL;
	QString propName = property->propertyName();
	if((propName == "EntryScript")
		|| (propName == "FrameScript")
		|| (propName == "ExitScript"))
	{
		resultWidget = new ScriptWidget(manager,property);
	}
	else
	{
		resultWidget = QtVariantEditorFactory::createEditor(manager,property,NULL);
	}
	if(resultWidget)
		resultWidget->installEventFilter(this);
	return resultWidget;
}

bool PropertyEditorFactory::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::FocusOut)
		emit propertyChanged();
	return false;
}