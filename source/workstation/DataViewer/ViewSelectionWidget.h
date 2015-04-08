#ifndef _VIEW_SELECTION_WIDGET_H_
#define _VIEW_SELECTION_WIDGET_H_

#include <QString>
#include <QVector>
#include <QWidget>
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QMap>

#include "../viewer.h"

using namespace Picto;


//! The Number of view slots horizontally on the Data View Tab
#define VIEWGRIDWIDTH 4
//! The Number of view slots vertically on the Data View Tab
#define VIEWGRIDHEIGHT 4

class DataViewWidget;
class DataViewLayout;

/*!	\brief Enumeration of the various sizes Views can take.
*/
enum ViewSize : int
{
	VIEW_SIZE_1x1 = 1,	//<! A 1x1 view
	VIEW_SIZE_2x2,		//<! A 2x2 view
	VIEW_SIZE_3x3,		//<! A 3x3 view
	VIEW_SIZE_4x4,		//<! A 4x4 view
	VIEW_SIZE_MAX		//<! An invalid value
};

/*!	\brief A struct to keep track of the location of widgets within the viewer.
*/
struct WidgetCoords
{
	WidgetCoords(int x, int y)
	{
		x_ = x;
		y_ = y;
	}
	int x_;
	int y_;
};

/*!	\brief A widget to contain objects meant to be displayed in the Experiment Viewer
*	\details Details forthcoming
*	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2015
*/
class ViewSelectionWidget : public QWidget
{
	Q_OBJECT
public:
	ViewSelectionWidget();
	virtual ~ViewSelectionWidget();

	bool setDefaultView(DataViewWidget *pDefaultView, int x, int y, ViewSize eSize);
	void registerView(DataViewWidget *pNewView);
	void addContainer(QWidget *pNewView);
	void connectToViewerLayout(DataViewLayout *pLayout);
	void connectToTaskConfig(QSharedPointer<TaskConfig> pTaskConfig);

	void rebuild();
	void clear();
signals:
	void widgetRemoved(QWidget *pWidget);
	void widgetAdded(QWidget *pWidget, int x, int y, ViewSize size);
public slots:
	void updateWidgetContainer(QWidget *pWidget);
	void removeWidgetContainer(QWidget *pWidget);
	void addWidgetContainer(QWidget *pWidget);
private:
	QGridLayout* selectionLayout_;
	QComboBox *plotSelection_;
	QComboBox *sizeSelection_;
	QCheckBox *positionCheckBoxes_[VIEWGRIDWIDTH][VIEWGRIDHEIGHT];
	QSharedPointer<TaskConfig> currentTaskConfig_;

	QMap<QObject*, QSharedPointer<WidgetCoords>> coordsMap;

	void updateSizeAndPositionOptions();
	void checkboxChanged(bool bNewValue, int x, int y);

	DataViewWidget *createDataViewWidget(QWidget *pWidget);
	void removeDataViewWidget(DataViewWidget *pWidget, bool bRemoveFromVector = true);
private slots:
	void selectedPlotIndexChanged(int selectedIndex);
	void selectedSizeIndexChanged(int selectedIndex);
	void checkClicked(bool);
	bool isWidgetPosValid(DataViewWidget *pWidget, int testX, int testY);
protected:
	QVector<DataViewWidget*> dataViewWidgets_;
	QMap<int, DataViewWidget*> widgetIndexMap_;
	int nextIndex_;
	QMap<QWidget*, DataViewWidget*> viewContainerMap_;

};

#endif
