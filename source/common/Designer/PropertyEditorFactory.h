#ifndef PROPERTYEDITORFACTORY_H
#define PROPERTYEDITORFACTORY_H
#include <QSharedPointer>
#include <QtVariantProperty.h>
#include "EditorState.h"
#include <QMap>
#include "PropertyEditTracker.h"
#include "../common.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyEditorFactory : public QtVariantEditorFactory
#else
class PropertyEditorFactory : public QtVariantEditorFactory
#endif
{
    Q_OBJECT

public:
   PropertyEditorFactory(QWidget *parent=0);
   virtual ~PropertyEditorFactory(){};
   void setNextProperty(QSharedPointer<Property> nextProp);
   void clear();
signals:
	void propertyEdited(QSharedPointer<Property> prop);

protected:
	virtual QWidget* createEditor (QtVariantPropertyManager* manager, QtProperty* property, QWidget* parent);
private:
	QList<QSharedPointer<PropertyEditTracker>> editTrackers_;


};
//! [0]
#endif
