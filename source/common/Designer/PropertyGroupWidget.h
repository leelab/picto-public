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

/*! \brief This Widget creates groups of Properties widgets.  It groups together widgets for setting/getting values of a Properties 
 *	and labels each group with a title.
 *	\details Properties for display are passed in using the addProperties() function along with a title for each Property group.
 *
 *	This widget's job is essentially just creating PropertyListWidgets which take care of actually creating property widgets and displaying them 
 *	and then placing each of those widget within a group layout.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class PropertyGroupWidget : public QWidget
{
    Q_OBJECT

public:
	PropertyGroupWidget(bool trackInitVals, QSharedPointer<EditorState> editorState = QSharedPointer<EditorState>(), QWidget *parent=0);
	virtual ~PropertyGroupWidget();
	void addProperties(QString title, QVector<QSharedPointer<Property>> props);
	void clear();
signals:
	/*! Emitted when any Property whose widget is included in this PropertyGroupWidget is edited.*/
	void propertyEdited(QSharedPointer<Property> prop,QVariant val);

private:
	QSharedPointer<EditorState> editorState_;
	QVBoxLayout* layout_;
	QWidget* mainWidget_;
	bool trackInitVals_;
};

#endif
