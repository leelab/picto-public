#ifndef _VIEW_POSITION_WIDGET_H_
#define _VIEW_POSITION_WIDGET_H_

#include <QWidget>
#include <QtVariantProperty.h>
#include "ScriptTextEdit.h"
#include "EditorState.h"
#include "../operator/DataViewSpecs.h"


QT_BEGIN_NAMESPACE
class QLayout;
class QCheckBox;
class QComboBox;
QT_END_NAMESPACE

/*! \brief A widget for managing and presenting the View Position editor as well as interfacing them with an underlying
 *	QtProperty.
 *	\details This widget is used with the PropertyEditorFactory to create a widget for updating the DataViewElement
 *	ViewPosition Property.  As such, this widget needs to include interfacing with the QtPropertBrowser framework which we
 *	use to get some free automatic type dependant widget creation and layout.  Changes to the widget are set to an
 *	underlyiung QtProperty using its QtVariantPropertyManager.  Similarly, changes to the underlying QtProperty are
 *	reflected in the ViewPositionWidget's checkboxes.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */	
class ViewPositionWidget : public QWidget
{
    Q_OBJECT

public:
	ViewPositionWidget(QtVariantPropertyManager* manager, QtProperty* property, QSharedPointer<EditorState> editorState, QWidget *parent = 0);
	virtual ~ViewPositionWidget(){};

	const Picto::ViewProperties &getViewProperties() const;

private:
	void rectifyNewProperties();
	void pushChange();

	QLayout *layout_;
	QtVariantPropertyManager *manager_;
	QtProperty *property_;
	QSharedPointer<EditorState> editorState_;
	QCheckBox *positionCheckBoxes_[VIEWGRIDWIDTH][VIEWGRIDHEIGHT];
	QComboBox *sizeSelection_;
	//! A Map of Checkboxes to their grid coordinates.  This way we don't need to subclass CheckBox or search the array.
	QMap<QObject*, QSharedPointer<WidgetCoords>> coordsMap;

	Picto::ViewProperties currentViewProperties_;

private slots:
	void checkboxClicked (bool checked);
	void selectedSizeIndexChanged(int);
};


#endif
