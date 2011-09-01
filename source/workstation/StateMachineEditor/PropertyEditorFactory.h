#ifndef PROPERTYEDITORFACTORY_H
#define PROPERTYEDITORFACTORY_H
#include <QtVariantEditorFactory>
#include <QSharedPointer>
#include <QtVariantPropertyManager>
#include "EditorState.h"
#include <QMap>
#include "PropertyEditTracker.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
class PropertyEditorFactory : public QtVariantEditorFactory
{
    Q_OBJECT

public:
   PropertyEditorFactory(QWidget *parent=0);
   void setNextProperty(QSharedPointer<Picto::Property> nextProp);
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
