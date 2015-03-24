#ifndef _DATA_VIEW_WIDGET_H_
#define _DATA_VIEW_WIDGET_H_

#include <QString>
#include <QVector>
#include <QWidget>
#include <QGridLayout>

#include "../viewer.h"

using namespace Picto;

enum ViewSize : int;

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

	void setDisplayed (bool enable);
	/* \brief Returns whether the widget is currently displayed */
	bool getDisplayed() { return currentlyDisplayed_; };

	/* \brief Returns the current size of this widget */
	ViewSize getCurrentSize() { return lastViewSize_; };
	/* \brief Sets the current size of this widget */
	void setCurrentSize(ViewSize eSize) { lastViewSize_ = eSize; };

	/* \brief Returns the current name of this widget */
	const QString getName() const { return name_; };

	void setPosition(int xPos, int yPos);
	void getPosition(int &xPos, int &yPos);

	void hideDefaultTitle();

private:
	QGridLayout* layout_;
	QString name_;
	bool currentlyDisplayed_;
	ViewSize lastViewSize_;
	int xPos_;
	int yPos_;
};

#endif
