#ifndef PropertyListWidget_H
#define PropertyListWidget_H
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
class PropertyListWidget : public QWidget
{
    Q_OBJECT

public:
	PropertyListWidget(bool trackInitVals, QSharedPointer<EditorState> editorState = QSharedPointer<EditorState>(), QWidget *parent=0);
	virtual ~PropertyListWidget();
	void addProperties(QString title, QVector<QSharedPointer<Property>> props);

signals:
	void propertyEdited(QSharedPointer<Property> prop,QVariant val);

private:
	void addProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtAbstractPropertyBrowser* browser);
	void addScriptProperty(QSharedPointer<Property> prop,QtVariantPropertyManager* manager,QtAbstractPropertyBrowser* browser);
	QVBoxLayout* layout_;
	QWidget* mainWidget_;
	bool trackInitVals_;
	QHash<Property*,QPair<QtVariantProperty*,QSharedPointer<Property>>> propToQtPropHash_;
	PropertyEditorFactory propertyFactory_;
	QtAbstractPropertyBrowser* browser_;
	QtVariantPropertyManager propManager_;
private slots:
	void propertyWasEdited(QSharedPointer<Property> prop,QVariant val);
	void propertyWasEditedExternally(Property* prop,QVariant val);
};
//! [0]
#endif
