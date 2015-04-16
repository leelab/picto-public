#ifndef _OPERATOR_PLOT_H_
#define _OPERATOR_PLOT_H_

#include "../common.h"
#include "DataViewElement.h"

#include <qwt_plot.h>

namespace Picto {

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

	/*!	\brief Returns a pointer to the underlying plot element.  This is used by the TaskConfig object to pass the
	 *	widgets to the ViewSelectionWidget.
	 */
	virtual QWidget *getWidget() { return m_pPlot; };

	virtual void setTitle(const QString &newTitle);
	virtual const QString getTitle() const;

	//! \brief An Asset-identifying string used with AssetFactory::addAssetType
	static const QString type;

protected:
	virtual void postDeserialize();

	/*!	\brief Reconstructs elements of underlying plot.
 	 */
	virtual void replot() = 0;

	//! A pointer to the OperatorPlot's underlying Qwt Plot widget
	QwtPlot *m_pPlot;
	//!	Flag for when the plot need to be recalculated.
	bool m_bDataChanged;
	//! Holds a copy of the plot's title
	QString m_title;
};


}; //namespace Picto

#endif
