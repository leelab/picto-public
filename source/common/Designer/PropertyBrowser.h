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

/*! \brief This Widget contains all property widgets for setting/getting Property values of the currently selected Asset in the Designer.
 *	\details The EditorState is passed into the constructor so that this widget can detect exactly
 *	what has been selected in the Designer Canvas and display the correct Property widgets accordingly.  If the main body of the
 *	AssetItem are selected, the Asset Property widgets are shown.  If the start bar of the AssetItem is selected, the Asset's Script
 *	Property widgets are shown.  If a result bar of an asset item is selected, the particular selected result's script is shown.
 *
 *	This widget's job is essentially figuring out which property widgets need to be displayed.  It includes a PropertyGroupWidget
 *	which takes care of actually creating and displaying those widgets.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
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
	QStringList orderedScriptNames_;
	QMap<QString,bool> scriptNamesLookup_;
private slots:
	void propertyEdited(QSharedPointer<Property> prop,QVariant val);
	void arrowPortSelected(QSharedPointer<Asset> asset);
};

#endif
