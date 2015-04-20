#include "BarAxisHandler.h"

#include "../memleakdetect.h"

#include <qwt_text.h>

namespace Picto {

BarAxisHandler::BarAxisHandler(long lLowerBound, long lUpperBound)
	: m_lLowerBound(lLowerBound), m_lUpperBound(lUpperBound)
{

	setTickLength(QwtScaleDiv::MajorTick, 6);
	setTickLength(QwtScaleDiv::MediumTick, 0);
	setTickLength(QwtScaleDiv::MinorTick, 0);

	setSpacing(15);
}

QwtScaleDiv BarAxisHandler::getScaleDiv() const
{
	QMutexLocker locker(&mtxValues);
	QList<double> ticks;

	for (long i = m_lLowerBound; i < m_lUpperBound + 1; i++)
	{
		ticks << i;
	}

	return QwtScaleDiv(double(m_lLowerBound) - 0.5, double(m_lUpperBound) + 0.5, QList<double>(), QList<double>(), ticks);
}

void BarAxisHandler::rescale(long lLowerBound, long lUpperBound)
{
	QMutexLocker locker(&mtxValues);

	m_lLowerBound = lLowerBound;
	m_lUpperBound = lUpperBound;
}

/* \brief Sets the label for the indicated bin.
*/
void BarAxisHandler::submitLabel(long bin, const QString &rLabel)
{
	QMutexLocker locker(&mtxLabels);

	m_qhsLabels[bin] = rLabel;
}

/* \brief Returns the label for the indicated bin.
*/
const QString BarAxisHandler::getLabel(long bin) const
{
	QMutexLocker locker(&mtxLabels);

	if (m_qhsLabels.contains(bin))
	{
		return m_qhsLabels[bin];
	}

	return QString("");
}

/*!	\brief Overwriting labeling functionality
*/
QwtText BarAxisHandler::label(double value) const
{
	QMutexLocker locker(&mtxLabels);

	if (m_qhsLabels.contains(long(value)))
	{
		return m_qhsLabels[long(value)];
	}
	return QString("%1").arg(long(value));
}

/*!	\brief Clears all values for this plot.
 */
void BarAxisHandler::reset()
{
	QMutexLocker lockerLab(&mtxLabels);
	QMutexLocker lockerVal(&mtxValues);

	m_lLowerBound = 0;
	m_lUpperBound = 1;

	m_qhsLabels.clear();
}

}; //namespace Picto
