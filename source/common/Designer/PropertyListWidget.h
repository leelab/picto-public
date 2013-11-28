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

/*! \brief This Widget represents a list of Properties.  It creates widgets for these Properties and connects the widgets and Properties together such
 *	that changing the value in one changes the value in the other.
 *	\details This widget makes use of the PropertyEditorFactory which takes advantage of the QtPropertyBrowser infrastructure to create an appropriate type
 *	of widgets for each property.  Using the QtPropertyBrowser makes the logic in this object fairly complicated.  In return we got lots of free pretty
 *	widgets that generated themselves automatically based on Property types.  At some point we may want to reduce complexity by just making the widgets
 *	ourselves.
 *	
 *	To prepare yourself for all of the complicated patchwork of code necessary to get the QtPropertyBrowser to work with Picto, try taking a deep breath and
 *	reading up at: http://docs.huihoo.com/qt/solutions/4/qtpropertybrowser/qtabstractpropertybrowser.html
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PropertyListWidget : public QWidget
{
    Q_OBJECT

public:
	PropertyListWidget(bool trackInitVals, QSharedPointer<EditorState> editorState = QSharedPointer<EditorState>(), QWidget *parent=0);
	virtual ~PropertyListWidget();
	void addProperties(QString title, QVector<QSharedPointer<Property>> props);

signals:
	/*! \brief Emitted when the prop Property's value was edited to become the val value.*/
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

#endif
