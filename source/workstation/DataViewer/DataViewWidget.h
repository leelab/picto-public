#ifndef _DATA_VIEW_WIDGET_H_
#define _DATA_VIEW_WIDGET_H_

#include <QString>
#include <QVector>
#include <QWidget>
#include <QVBoxLayout>

#include "../viewer.h"
using namespace Picto;

/*!	\brief A widget to contain objects meant to be displayed in the Experiment Viewer
 *  \details Details forthcoming
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

class DataViewWidget : public QWidget
{
	Q_OBJECT
public:
	DataViewWidget(const QString cqsName, QWidget *pqwWidget);
	virtual ~DataViewWidget();

	void enable(bool enable);

private:
	QVBoxLayout* layout_;
	bool enable_;
};

#endif
