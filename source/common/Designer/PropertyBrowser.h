#ifndef PROPERTYBROWSER_H
#define PROPERTYBROWSER_H
#include <QWidget>
#include "PropertyGroupWidget.h"
#include "EditorState.h"
using namespace Picto;

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

//! [0]
class PropertyBrowser : public QWidget
{
    Q_OBJECT

public:
   PropertyBrowser(QSharedPointer<EditorState> editorState,QWidget *parent=0);
   virtual ~PropertyBrowser(){};
public slots:
	void assetSelected(QSharedPointer<Asset> asset);

private:
	QSharedPointer<EditorState> editorState_;
	PropertyGroupWidget* propGroupWidget_;
private slots:
	void propertyEdited(QSharedPointer<Property> prop,QVariant val);
};
//! [0]
#endif
