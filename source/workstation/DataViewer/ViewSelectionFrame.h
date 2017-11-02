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
#include <QListWidget>
#include <QPushButton>

#include "../viewer.h"
#include "../../common/operator/DataViewSpecs.h"

using namespace Picto;

class DataViewWidget;
class DataViewOrganizer;
class Picto::OperatorPlotHandler;


/*!	\brief A widget to contain label and line edit for Scriptables (conditions)
*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2017
*/
class lblEdit : public QWidget
{
	Q_OBJECT
public:
	lblEdit(QString strLabel, QString strContent = "");

	QString getLabel();
	QLineEdit* getLineEdit();

private:
	QString label_;
	QLineEdit* lineEdit_;
};


/*!	\brief A widget to contain objects meant to be displayed in the Experiment Viewer
*	\details Details forthcoming
*	\author Vered Zafrany, Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2017
*/
class ViewSelectionFrame : public QFrame
{
	Q_OBJECT
public:
	ViewSelectionFrame(bool replay = false);
	virtual ~ViewSelectionFrame();

	void registerView(DataViewWidget *pNewView);
	void addContainer(QWidget *pNewView, bool signal = true);
	void tryDefault(QWidget *pNewView);
	void connectToViewerLayout(DataViewOrganizer *pOrganizer);
	void connectToTaskConfig(QSharedPointer<TaskConfig> pTaskConfig);
	void setVisualTargetHost(QWidget *pWidget);

	void rebuild();
	void clear();

	void spikeAdded(int channel, int unit);
	void preloadEvents(QSharedPointer<DesignRoot> designRoot, QMap<int, QList<int>> alignElements, QList<QPair<int, int>> spikes);
	void resetPlotsList(int resetSelection = 0); //1 means clear all lists, 2 means uncheck but do not clear
	void removeFromList(QWidget* widget);
	void resetDesignRoot(QSharedPointer<DesignRoot> designRoot);
	void hideCurrentWidget(QWidget *pWidget);
	void updateUI(bool activatePlots);
	void setTaskName(QString taskName);

signals:
	//! A signal broadcast when a widget is to be removed from the connected ViewSelectionLayout.
	void widgetRemoved(QWidget *pWidget);
	//! A signal broadcast when a widget is added to the connected ViewSelectionLayout.
	void widgetAdded(QWidget *pWidget, int x, int y, DataViewSize::ViewSize size);

	//! Message used to send the cached plotHandler (or a null pointer) back to the TaskConfig, for OperatorPlots
	void returnPlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath);
	//! Message used to indicate to TaskConfig when the manager is ready to receive 
	void managerConnectionEstablished(bool connected);

	void openInNewTab(QWidget* pWidget);

	public slots:
	void updateWidgetContainer(QWidget *pWidget);
	void removeWidgetContainer(QWidget *pWidget);
	void addWidgetContainer(QWidget *pWidget);
	void notifyAnalysisSelection(const QString &name, bool selected, bool local);
	void clearAnalysisSelection(bool local);

	void clearPlotHandlers();
	void widgetDropped(QPoint offset);
	void widgetDragged(QWidget *pWidget);

private:
	void updateSizeAndPositionOptions();
	void checkboxChanged(bool bNewValue, int x, int y);

	DataViewWidget *createDataViewWidget(QWidget *pWidget);
	void removeDataViewWidget(DataViewWidget *pWidget, bool bRemoveFromVector = true);

	void createCustomWidget(QString dataSetName, QList<int> alignIDList, QList<QPair<int, int>> scComb, QString scriptCombIndex = "");
	QList<QList<QPair<int, int>>> generateScrComb(QList<QPair<QString, QString>> scriptables);
	int maxVals(QList<QList<QPair<QString, QString>>> pairsList);
	QPair<int, int> getNeuron(QString dataSetName);
	int getEvent(QString dataSetName);
	QPair<int, int> getFlags();

private slots:
	void selectedPlotIndexChanged(int selectedIndex);
	void selectedSizeIndexChanged(int selectedIndex);
	void checkClicked(bool);

	void plotHandlerRequested(const QString plotPath);
	void cachePlotHandler(QSharedPointer<OperatorPlotHandler> handler, const QString plotPath);

	void selectedLevelIndexChangedNeur();
	void selectedLevelIndexChangedHor();
	void selectedLevelIndexChangedAl();
	void selectedLevelIndexChangedSc();
	void selectedLevelIndexChangedVal();
	void selectedLevelIndexChangedMark();
	void selectedPlotTypeChanged(int);
	void readSettings();
	void writeSettings();

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

	//! Combobox containing all Events available for the current task. 
	QListWidget *levelsSelectionHor_;
	//! Combobox containing all Neurons available for the current task. 
	QListWidget* levelsSelectionNeur_;
	//! Combobox containing all reference events for alignment available for the current task. 
	QListWidget *levelsSelectionAl_;
	//! Combobox containing all Scriptable members available for the current align event. 
	QListWidget *levelsSelectionSc_;
	//! Combobox containing all Conditions members available for the current align event. 
	QListWidget* levelsSelectionVal_;
	//! Combobox containing all Neurons available for the current task. 
	QListWidget* levelsSelectionMark_;

	//! Plot type selection: Raster, PSTH or both
	QString selectedType_;
	QComboBox* plotType_;
	QSharedPointer<DesignRoot> designRoot_;
	//! A Map of asset Ids corresponding to all elements (selected or not) that can be used
	// for alignment and their correspnding list of existing (selected or not) scriptable members
	QMap<int, QList<int>> alignElements_;
	QList<QPair<int, int>> spikesList_;
	QSharedPointer<Analysis> customAnalysis_;
	int customAnalysisIndex_;
	bool NoCallBack_;
	lblEdit* preflagEdit_;
	lblEdit* postflagEdit_;
	QWidget* neuronsSelWidget_;
	QWidget* typeSelWidget_;
	QWidget* flagSelWidget_;

	QSharedPointer<Picto::Analysis> CreateCustomAnalysis();
	void DeleteCustomAnalysis();

	void savePlotSettings();
	void retrievePlotSettings();

	bool activatePlots_;
	QPushButton* saveSettings_;
	QPushButton* recoverSettings_;
	QStringList plotSettings_;
	QString currentTask_;
};

#endif
