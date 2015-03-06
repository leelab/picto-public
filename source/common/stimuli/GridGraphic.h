#ifndef _GRIDGRAPHIC_H_
#define _GRIDGRAPHIC_H_

#include <QVector>
#include <QPoint>

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Represents a grid graphic (ie. a sort of giant tic tac toe board formed by crossing vertical and horizontal lines).
 *
 *	\details In addition to the Properties provided by the VisualElement class, this class provides size, rows and columns 
 *	Properties.  The size is just the total width and height of the graphic.  The rows and columns are the number of rows
 *	and columns that appear in the grid between the grid lines.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PICTOLIB_CLASS GridGraphic : public VisualElement
{
	Q_OBJECT
	/*! \brief Sets/gets the width of this graphic in pixels.*/
	Q_PROPERTY(int width READ getWidth WRITE setWidth)
	/*! \brief Sets/gets the height of this graphic in pixels.*/
	Q_PROPERTY(int height READ getHeight WRITE setHeight)
	/*! \brief Sets/gets the number of rows that are displayed in the grid.*/
	Q_PROPERTY(int rows READ getRows WRITE setRows)
	/*! \brief Sets/gets the number of columns that are displayed in the grid.*/
	Q_PROPERTY(int columns READ getColumns WRITE setColumns)
public:
	GridGraphic();

	void draw();
	static QSharedPointer<Asset> Create();
	static const QString type;

	virtual void upgradeVersion(QString deserializedVersion);
	/*! \brief Returns the width of this graphic in pixels.*/
	int getWidth(){return getDimensions().width();};
	/*! \brief Returns the height of this graphic in pixels.*/
	int getHeight(){return getDimensions().height();};
	/*! \brief Sets the width of this graphic in pixels.*/
	void setWidth(int w){QRect dims = getDimensions(); dims.setWidth(w);setDimensions(dims);};
	/*! \brief Sets the height of this graphic in pixels.*/
	void setHeight(int h){QRect dims = getDimensions(); dims.setHeight(h);setDimensions(dims);};
	/*! \brief Gets the number of rows that are displayed in the grid.*/
	int getRows(){return propertyContainer_->getPropertyValue("Rows").toInt();};
	/*! \brief Sets the number of rows that are displayed in the grid.*/
	void setRows(int val){propertyContainer_->setPropertyValue("Rows",val);};
	/*! \brief Gets the number of columns that are displayed in the grid.*/
	int getColumns(){return propertyContainer_->getPropertyValue("Columns").toInt();};
	/*! \brief Sets the number of columns that are displayed in the grid.*/
	void setColumns(int val){propertyContainer_->setPropertyValue("Columns",val);};
	/*! \brief Returns a QRect with the dimensions (width, height) of this graphic.
	 *	\note The returned value contains no position information.  Only the Width, Height
	 *	values should be used.
	 */	
	QRect getDimensions(){ return QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize()); };
	/*! \brief Set the dimensions (width, height) of this graphic.
	 *	\note Only the width and height of the input QRect are used.  x,y position values will be ignored.
	 */	
	void setDimensions(QRect dimensions){ propertyContainer_->setPropertyValue("Size",dimensions.size());};
	QPoint getPositionOffset();

	virtual QString friendlyTypeName(){return "Grid";};
	virtual QString getUITemplate(){return "GridElement";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	QVector<QPoint> points_;
	QPoint posOffset_;
private slots:
	
};


}; //namespace Picto

#endif
