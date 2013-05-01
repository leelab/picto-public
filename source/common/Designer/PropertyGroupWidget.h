#ifndef PROPERTYGROUPWIDGET_H
#define PROPERTYGROUPWIDGET_H
#include <QVBoxLayout>
#include <QPair>
#include <QtPropertyBrowser.h>
#include <QtVariantProperty.h>
#include <QtGroupBoxPropertyBrowser.h>
#include <qtbuttonpropertybrowser.h>
#include <qttreepropertybrowser.h>
#include "PropertyEditorFactory.h"
#include "EditorState.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
class PropertyGroupWidget : public QWidget
{
    Q_OBJECT

public:
	PropertyGroupWidget(bool trackInitVals, QSharedPointer<EditorState> editorState = QSharedPointer<EditorState>(), QWidget *parent=0);
	virtual ~PropertyGroupWidget();
	void addProperties(QString title, QVector<QSharedPointer<Property>> props);
	void clear();
signals:
	void propertyEdited(QSharedPointer<Property> prop,QVariant val);

private:
	void addProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtButtonPropertyBrowser* browser);
	void addScriptProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtButtonPropertyBrowser* browser);
	QSharedPointer<PropertyEditorFactory> propertyFactory_;
	QVBoxLayout* layout_;
	QWidget* mainWidget_;
	bool trackInitVals_;
	QHash<Property*,QPair<QtVariantProperty*,QSharedPointer<Property>>> propToQtPropHash_;
private slots:
	void propertyWasEdited(QSharedPointer<Property> prop,QVariant val);
	void propertyWasEditedExternally(Property* prop,QVariant val);
};
//! [0]
#endif