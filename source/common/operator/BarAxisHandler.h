#ifndef _BAR_AXIS_HANDLER_H_
#define _BAR_AXIS_HANDLER_H_

#include "../common.h"
#include <qwt_scale_draw.h>
#include <qwt_scale_div.h>
#include <QHash>

namespace Picto {

/*!	\brief Encapsulation of BarPlot labeling and scaling functionality.
 *
 *	This is currently utilized by SamplingBarPlot and BarPlot
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API BarAxisHandler : public QwtScaleDraw
#else
class BarAxisHandler : public QwtScaleDraw
#endif
{
public:
	BarAxisHandler(long lLowerBound, long lUpperBound);
	virtual ~BarAxisHandler(){};

	void rescale(long lLowerBound, long lUpperBound);

	QwtScaleDiv getScaleDiv() const;
	void submitLabel(long bin, const QString &rLabel);
	const QString getLabel(long bin) const;

	virtual QwtText label(double value) const;
	void reset();
protected:
	//!	\brief The collected binwise Labels.
	QHash<long, QString> m_qhsLabels;
	long m_lLowerBound;
	long m_lUpperBound;
};


}; //namespace Picto

#endif
