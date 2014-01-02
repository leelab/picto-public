#include <QPainter>
#include "GridGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief I believe that this is no longer used.  It sbould probably be deleted.*/
const QString GridGraphic::type = "Grid Graphic";

/*! \brief Constructs a GridGraphic object.
 *	\detils Adds the following Properties:
 *	- Rows: The number of rows in the grid (where a row is the space between horizontal lines)
 *	- Columns: The number of colums in the grid (where a column is the space between vertical lines)
 *	- Size:	The total size of the grid graphic.
 */
GridGraphic::GridGraphic()
: VisualElement()
{
	AddDefinableProperty(QVariant::Int,"Rows",5);
	AddDefinableProperty(QVariant::Int,"Columns",5);
	AddDefinableProperty(QVariant::Size,"Size",QSize());
}

void GridGraphic::draw()
{
	QRect dims = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
	int rows = propertyContainer_->getPropertyValue("Rows").toInt();
	int cols = propertyContainer_->getPropertyValue("Columns").toInt();

	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dims.width(),dims.height(),QImage::Format_ARGB32);
	posOffset_ = QPoint(dims.width()/2.0,dims.height()/2.0);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	QPen pen(color);
	pen.setWidth(0);	//0 means its a cosmetic line that should not disappear at any zoom level
	p.setPen(pen);

	//Draw it
	int rowHeight = dims.height()/rows;
	int colWidth = dims.width()/cols;
	for(int i = 1; i < rows; i++)
	{
		p.drawLine(QPoint(0,rowHeight*i),QPoint(dims.width(),rowHeight*i));
	}
	for(int i = 1; i < cols; i++)
	{
		p.drawLine(QPoint(colWidth*i,0),QPoint(colWidth*i,dims.height()));
	}
	p.end();
	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief This is no longer used by parts of Picto that are being used.  It sbould probably be deleted.*/
VisualElement* GridGraphic::NewVisualElement()
{
	return new GridGraphic;
}

/*! \brief Creates a new GridGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> GridGraphic::Create()
{
	return QSharedPointer<Asset>(new GridGraphic());
}

void GridGraphic::upgradeVersion(QString deserializedVersion)
{
	VisualElement::upgradeVersion(deserializedVersion);
	if(deserializedVersion < "0.0.1")
	{
		//As of version 0.0.1, Grid graphics are positioned according to their center point, not their top
		//left corner.  This fixes the positions for older experiments.
		QRect dims = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
		QPoint newPos = getPosition();
		newPos = QPoint(newPos.x() + dims.width()/2.0,newPos.y() + dims.height()/2.0);
		setPosition(newPos);
	}
}

QPoint GridGraphic::getPositionOffset()
{
	return posOffset_;
}

void GridGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
	setPropertyRuntimeEditable("Rows");
	setPropertyRuntimeEditable("Columns");
	draw();
}

/*! \brief Extends VisualElement::validateObject() to verify that the Rows and Columns values are greater than zero.
*/
bool GridGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;

	int rows = propertyContainer_->getPropertyValue("Rows").toInt();
	int cols = propertyContainer_->getPropertyValue("Columns").toInt();
	if(rows <= 0)
	{
		addError("Rows value must be greater than zero.");
		return false;
	}

	if(cols <= 0)
	{
		addError("Columns value must be greater than zero.");
		return false;
	}
	return true;
}

}; //namespace Picto
