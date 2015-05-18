#ifndef _OPERATOR_PLOT_H_
#define _OPERATOR_PLOT_H_
#include <QObject>

#include "../common.h"
#include "DataViewElement.h"

namespace Picto {

class OperatorPlotHandler;

/*!	\brief A plot populated in realtime during experiments or analysis.
 *
 *	The plot is meant for display in the Operator View.
 *	\note We need to be careful how this is serialized, as sending actual plot data would be wildly redundant.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API OperatorPlot : public DataViewElement
#else
class OperatorPlot : public DataViewElement
#endif
{
	Q_OBJECT
public:
	OperatorPlot();
	virtual ~OperatorPlot();

	/*!	\brief Prepares the underlying Plot for rendering.
	 */
	virtual void draw();
	/*!	\brief Clears all values for this plot.
	 */
	virtual void reset() = 0;

	virtual QString friendlyTypeName(){return "Operator Plot";};
	virtual QString getUITemplate(){return "OperatorFeature";};
	virtual QString getUIGroup(){return "Operator Features";};

	virtual void setTitle(const QString &newTitle);
	virtual const QString getTitle() const;

	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

	virtual void sendWidgetToTaskConfig();
	virtual void enteredScope();

	virtual void scriptableContainerCustomInitialization();

	virtual void postLinkUpdate();

	virtual void receivePlotHandler(QSharedPointer<OperatorPlotHandler> plotHandler);

signals:
	void initializePlotSig(const QString &xTitle, const QString &yTitle);
	void setTitleSig(const QString &title);
	void connectToTaskConfigSig(QSharedPointer<TaskConfig> pTaskConfig);
	void exportPlot(int type, int size, const QString fileName);

protected:
	virtual void postDeserialize();

	virtual void initView();

	/*!	\brief Reconstructs elements of underlying plot.
 	 */
	virtual void replot() = 0;

	virtual QSharedPointer<OperatorPlotHandler> getNewHandler();
	virtual void connectHandler(QSharedPointer<OperatorPlotHandler> plotHandler);
	virtual void connectDataSignals(QSharedPointer<OperatorPlotHandler> plotHandler);

	//!	Flag for when the plot need to be recalculated.
	bool m_bDataChanged;
	//! Holds a copy of the plot's title
	QString m_title;

	//! Flag used to identify when the InitializationScript has run for a given Asset
	bool initialized_;

	QSharedPointer<OperatorPlotHandler> m_pPlotHandler;
};


}; //namespace Picto

#endif
