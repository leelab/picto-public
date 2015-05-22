#ifndef _VIEW_SELECTION_WIDGET_H_
#define _VIEW_SELECTION_WIDGET_H_

#include <QString>
#include <QVector>
#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include <QMap>

#include "../viewer.h"
#include "../../common/operator/DataViewSpecs.h"

using namespace Picto;

class DataViewWidget;
class DataViewOrganizer;
class Picto::OperatorPlotHandler;


/*!	\brief A widget to contain objects meant to be displayed in the Experiment Viewer
*	\details Details forthcoming
*	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2015
*/
class ViewSelectionFrame : public QFrame
{
	Q_OBJECT
public:
	ViewSelectionFrame();
	virtual ~ViewSelectionFrame();

	void registerView(DataViewWidget *pNewView);
	void addContainer(QWidget *pNewView, bool signal = true);
	void tryDefault(QWidget *pNewView);
	void connectToViewerLayout(DataViewOrganizer *pOrganizer);
	void connectToTaskConfig(QSharedPointer<TaskConfig> pTaskConfig);
	void setVisualTargetHost(QWidget *pWidget);

	void rebuild();
	void clear();

signals:
	//! A signal broadcast when a widget is to be removed from the connected ViewSelectionLayout.
	void widgetRemoved(QWidget *pWidget);
	//! A signal broadcast when a widget is added to the connected ViewSelectionLayout.
	void widgetAdded(QWidget *pWidget, int x, int y, DataViewSize::ViewSize size);

	//! Message used to send the cached plotHandler (or a null pointer) back to the TaskConfig, for OperatorPlots
	void returnPlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath);
	//! Message used to indicate to TaskConfig when the manager is ready to receive 
	void managerConnectionEstablished(bool connected);

public slots:
	void updateWidgetContainer(QWidget *pWidget);
	void removeWidgetContainer(QWidget *pWidget);
	void addWidgetContainer(QWidget *pWidget);
	void notifyAnalysisSelection(const QString &name, bool selected, bool local);
	void clearAnalysisSelection(bool local);

	void clearPlotHandlers();

private:
	void updateSizeAndPositionOptions();
	void checkboxChanged(bool bNewValue, int x, int y);

	DataViewWidget *createDataViewWidget(QWidget *pWidget);
	void removeDataViewWidget(DataViewWidget *pWidget, bool bRemoveFromVector = true);

private slots:
	void selectedPlotIndexChanged(int selectedIndex);
	void selectedSizeIndexChanged(int selectedIndex);
	void checkClicked(bool);

	void plotHandlerRequested(const QString plotPath);
	void cachePlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath);

protected:
	//! A Vector holding all registered DataViewWidgets
	QVector<DataViewWidget*> dataViewWidgets_;
	//! A Map of all widget indices (The combobox meta-data) to DataViewWidgets
	QMap<int, DataViewWidget*> widgetIndexMap_;
	//! A Map of ViewWidgets to their corresponding DataViewWidgets (just containing DataViewWidgets created here)
	QMap<QWidget*, DataViewWidget*> viewContainerMap_;
	//! The next widget index to pass out
	int nextIndex_;
	//! Current Local Analysis Dispaly states.  Map of Analysis Name vs current selection.
	QMap<QString, bool> localAnalysisDisplayState;
	//! Current ImportedAnalysis Dispaly states.  Map of Analysis Name vs current selection.
	QMap<QString, bool> importedAnalysisDisplayState;

private:
	bool isWidgetPosValid(DataViewWidget *pWidget, int testX, int testY);
	void addTargetHost();

	//! Organizational layout
	QGridLayout* selectionLayout_;
	//! Combobox containing all Plots available for the current task
	QComboBox *plotSelection_;
	//! Combobox containing all Plot sizes available for the current plot
	QComboBox *sizeSelection_;
	//! A shared pointer to the current task's TaskConfig object.  Used for message-passing and connecting.
	QSharedPointer<TaskConfig> currentTaskConfig_;
	//! A pointer to the visualTargetHost displaying our task
	QWidget *pVisualTargetHost_;

	//! A two-index array of all Position Checkboxes by their grid location
	QCheckBox *positionCheckBoxes_[VIEWGRIDWIDTH][VIEWGRIDHEIGHT];
	//! A Map of Checkboxes to their grid coordinates.  This way we don't need to subclass CheckBox or search the array.
	QMap<QObject*, QSharedPointer<WidgetCoords>> coordsMap;

	//! A Hash of QSharedPointers to Plot Handlers, indexed by analysis path
	QHash<QString, QSharedPointer<OperatorPlotHandler>> cachedHandlers_;

	//! A Hash for restoring formerly displayed widgets.  Do not use pointers.
	QList<QWeakPointer<TaskConfig>> cachedConfigList_;
	QList<QHash<QWidget*, ViewProperties>> cachedViewSetup_;
	QWeakPointer<TaskConfig> lastTaskConfig_;

	int configIndex(QWeakPointer<TaskConfig> referenceConfig);
};

#endif
