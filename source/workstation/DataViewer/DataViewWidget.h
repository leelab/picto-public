#ifndef _DATA_VIEW_WIDGET_H_
#define _DATA_VIEW_WIDGET_H_

#include <QString>
#include <QVector>
#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include "../viewer.h"
#include "../../common/operator/DataViewSpecs.h"

using namespace Picto;

/*!	\brief The RetentionPolicy dictates whether the widget should be deleted and removed when reset is called on the
 *	controlling ViewSelectionFrame.
 *	\sa ViewSelectionFrame::reset
 */
enum DVW_RetentionPolicy
{
	DVW_CLEAR = 0,
	DVW_RETAIN

};

/*!	\brief A widget to contain objects meant to be displayed in the Experiment Viewer
 *  \details The DataViewWidget is a wrapper for a widget to be displayed in the DataViewOrganizer.  Ideally, these widgets
 *	are resizeable and repositionable on a grid of views.  This wrapper class holds information about the widget's
 *	display, and provides an interface for the DataViewOrganizer to interact with the display data.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class DataViewWidget : public QWidget
{
	Q_OBJECT
public:
	DataViewWidget(const QString cqsName, QWidget *pqwWidget, DVW_RetentionPolicy eRetentionPolicy = DVW_CLEAR);
	virtual ~DataViewWidget();

	void setDisplayed (bool enable);
	/*!	\brief Returns whether the widget is currently displayed
	 */
	bool getDisplayed() { return currentlyDisplayed_; };

	/*!	\brief Returns the current size of this widget
	 */
	DataViewSize::ViewSize getCurrentSize() { return lastViewSize_; };
	/*!	\brief Sets the current size of this widget
	 */
	void setCurrentSize(DataViewSize::ViewSize eSize) { lastViewSize_ = eSize; };

	/*!	\brief Returns the current name of this widget
	 */
	virtual const QString getName() const { return name_; };
	virtual void setName(const QString &newName);

	/*! \brief Returns the current RetentionPolicy of this widget
	 */
	DVW_RetentionPolicy getRetentionPolicy() const { return eRetentionPolicy_; };

	void setPosition(int xPos, int yPos);
	void getPosition(int &xPos, int &yPos);

	bool containsPosition(int xPos, int yPos);

	void hideDefaultTitle();

	void dissociate();

	QWidget* getWidget();

private:
	//! This widget's organizational layout
	QGridLayout* layout_;
	//! The name of this widget
	QString name_;
	//! Holds a raw pointer to the widget to be wrapped.
	QWidget *myViewWidget_;
	//! An (optional) label.  We retain a pointer to it in case we need to remove it.
	QLabel *myLabel_;

	/*!	\brief The retention policy of this DataViewWidget.  DataViewWidgets that are set to be retained are not deleted
	*	when the views are reset.
	*/
	DVW_RetentionPolicy eRetentionPolicy_;
	//! The current ViewSize of this widget.  Only meaningful if it is currentlyDisplayed_.
	DataViewSize::ViewSize lastViewSize_;

	//! If this DataViewWidget is currently being displayed in the grid.
	bool currentlyDisplayed_;
	//! The current X-Position in the View Grid of this widget.  Only meaningful if it is currentlyDisplayed_.
	int xPos_;
	//! The current Y-Position in the View Grid of this widget.  Only meaningful if it is currentlyDisplayed_.
	int yPos_;

};

#endif
