#include <QPainter>

#include "OperatorInfoGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

	const QString OperatorInfoGraphic::type = "Operator Info Graphic";

/*! \brief Constructs an OperatorInfoGraphic object.
 *	\details Adds the following Properties:
 *	- Size: Stores the size of the graphic window containing text (width, height).
 *	- Fields: Stores a comma separated list of all of the graphic's fields names (like keys in a lookup table).
 *	- Values: Stores a comma separated list of all of the graphic's values ordered like the Fields Property value.
 */
OperatorInfoGraphic::OperatorInfoGraphic()
: VisualElement(QPoint(),QColor(Qt::green)),
settingPropsFromMap_(false)
{
	AddDefinableProperty(QVariant::Size,"Size",QSize());
	AddDefinableProperty(QVariant::String,"Fields","");
	AddDefinableProperty(QVariant::String,"Values","");
	//These are redefined from Visual Element to fix the default values.
	AddDefinableProperty(QVariant::Bool,"OperatorView",true);
	AddDefinableProperty(QVariant::Bool,"SubjectView",false);
}

void OperatorInfoGraphic::draw()
{
	QRect dimensions = QRect(QPoint(),propertyContainer_->getPropertyValue("Size").toSize());
	QColor color = propertyContainer_->getPropertyValue("Color").value<QColor>();

	QImage image(dimensions.width(),dimensions.height(),QImage::Format_ARGB32);
	image.fill(0);
	QPainter p(&image);
	p.setRenderHint(QPainter::Antialiasing, true);
	p.setBrush(color);
	p.setPen(color);
	p.setFont(QFont("Times",12));
	QString info;
	QStringList fields = propertyContainer_->getPropertyValue("Fields").toString().split(",");
	QStringList values = propertyContainer_->getPropertyValue("Values").toString().split(",");
	QStringList::iterator fIt;
	QStringList::Iterator vIt;
	for (fIt=fields.begin(),vIt=values.begin();
		(fIt != fields.end()) && (vIt != values.end());fIt++,vIt++) 
	{
		info.append(QString("%1: %2, ").arg((*fIt)).arg((*vIt)));
	}
	if(info.size())
		info.chop(2);//Get rid of comma space
	p.drawText(dimensions, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap,info);
	p.end();

	image_ = image;

	//updateCompositingSurfaces();

	shouldUpdateCompositingSurfaces_ = true;
}

/*! \brief Creates a new OperatorInfoGraphic object and returns a shared Asset pointer to it.*/
QSharedPointer<Asset> OperatorInfoGraphic::Create()
{
	return QSharedPointer<Asset>(new OperatorInfoGraphic());
}

void OperatorInfoGraphic::enteredScope()
{
	orderedFields_.clear();
	infoMap_.clear();
	VisualElement::enteredScope();
}

/*! \brief Sets the input value to the input field.  If the field does not yet exist, it will be added.
 *	\note This change does not actually take effect until updateValue is called.  This was originally designed
 *	to save lots of unnecessary writes of long Fields/Values Property values to the server.  Since then we
 *	have added things like the VariableList and VariableMap which don't worry about that type of issue though,
 *	so it might make sense to think about changing this at some point; however, this could certainly adversely
 *	effect older sessions, so some thought will need to be put into this.
 *	\note When data is actually written to the screen, it is written in the order that field/value pairs
 *	are added to this object.
 */
void OperatorInfoGraphic::setData(QString field, QVariant value)
{
	if(!infoMap_.contains(field))
	{
		orderedFields_.append(field);
	}
	infoMap_[field] = value;
}

/*! \brief Returns a list of all of the field names currently used in this object.
 */
QVariantList OperatorInfoGraphic::getFields()
{
	QStringList fields = propertyContainer_->getPropertyValue("Fields").toString().split(",");
	QVariantList returnVal;
	returnVal.reserve(fields.size());
	foreach(QString field,fields)
	{
		returnVal.append(field);
	}
	return returnVal;
}

/*! \brief Returns a list of all of the values currently used in this object ordered like the result of getFields().
 */
QVariantList OperatorInfoGraphic::getValues()
{
	QStringList values = propertyContainer_->getPropertyValue("Values").toString().split(",");
	QVariantList returnVal;
	returnVal.reserve(values.size());
	foreach(QString value,values)
	{
		returnVal.append(value);
	}
	return returnVal;
}

/*! \brief Causes all of the updates of fields/values that have been made since the last call to this function to "take effect" and be written
*	into the "Fields" and "Values" Properties.
*/
void OperatorInfoGraphic::updateValue()
{
	QStringList values;
	foreach(QString field,orderedFields_)
	{
		values.append(infoMap_.value(field).toString());
	}
	settingPropsFromMap_ = true;
	propertyContainer_->setPropertyValue("Fields",orderedFields_.join(","));
	propertyContainer_->setPropertyValue("Values",values.join(","));
	settingPropsFromMap_ = false;
}

/*! \brief Returns the value of the input field if it exists or an empty QVariant otherwise.
*/
QVariant OperatorInfoGraphic::getData(QString field)
{
	if(infoMap_.contains(field))
		return infoMap_[field];
	return QVariant();
}

void OperatorInfoGraphic::postDeserialize()
{
	VisualElement::postDeserialize();
	//Fields and Values properties are for information transfer only, don't show them in the UI
	propertyContainer_->getProperty("Fields")->setVisible(false);
	propertyContainer_->getProperty("Values")->setVisible(false);
	connect(propertyContainer_->getProperty("Fields").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propValueChanged(Property*,QVariant)));
	connect(propertyContainer_->getProperty("Values").data(),SIGNAL(valueChanged(Property*,QVariant)),this,SLOT(propValueChanged(Property*,QVariant)));
}

bool OperatorInfoGraphic::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!VisualElement::validateObject(xmlStreamReader))
		return false;
	return true;
}

/*! \brief Whenever the Fields or Values Properties change due to an outside source (a SlaveExperimentDriver might set them in playback for example)
 *	this object's internal data needs to be updated accordingly.  This takes care of that.
 */
void OperatorInfoGraphic::propValueChanged(Property*,QVariant)
{
	//If the change happened because of one of this object's functions, return.
	if(settingPropsFromMap_)
		return;
	//The change must have happened from an outside source (ie. The value was reset to the initValue
	//or this is a slave and something changed on the master)
	//Set our Map from the properties
	QStringList fields = propertyContainer_->getPropertyValue("Fields").toString().split(",");
	QStringList values = propertyContainer_->getPropertyValue("Values").toString().split(",");
	orderedFields_.clear();
	infoMap_.clear();
	QVariant value;
	for(int i=0;i<fields.length();i++)
	{
		if(i >= values.length())
			value.clear();
		else
			value = values[i];
		setData(fields[i],value);
	}
}
}; //namespace Picto
