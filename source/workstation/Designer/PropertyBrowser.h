#ifndef PROPERTYBROWSER_H
#define PROPERTYBROWSER_H
#include <QtAbstractPropertyBrowser>
#include <QtVariantPropertyManager>
#include <QtGroupBoxPropertyBrowser>
#include <qtbuttonpropertybrowser>
#include <qttreepropertybrowser>
#include "PropertyEditorFactory.h"
#include "EditorState.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
class PropertyBrowser : public QtButtonPropertyBrowser
{
    Q_OBJECT

public:
   PropertyBrowser(QSharedPointer<EditorState> editorState,QWidget *parent=0);
   virtual ~PropertyBrowser(){};
public slots:
	void assetSelected(QSharedPointer<Asset> asset);

private:
	QSharedPointer<EditorState> editorState_;
	QSharedPointer<PropertyEditorFactory> propertyFactory_;
private slots:
	void propertyEdited(QSharedPointer<Property> prop);
};
//! [0]
#endif
