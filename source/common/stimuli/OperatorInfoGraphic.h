
#ifndef _OPERATORINFOGRAPHIC_H_
#define _OPERATORINFOGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief Generates a graphic with information for the operator.
 *	To use this graphic, call editInfo(field, value) from a script.
 *	The Graphic will automatically update its Fields property to include
 *	the input field and the Values property to include the input value.
 *	The information will be displayed on screen.  User's can use this
 *	to simplify analysis as well.  All analysis data can be sent to objects
 *	of this type, and they can simply be read from the output data file
 *	after the experiment.
 *	If field ordering is relevant, add Field can be called in an initialization
 *	script to make sure that the fields appear in the desired order.
 */
class PICTOLIB_CLASS OperatorInfoGraphic : public VisualElement
{
	Q_OBJECT
public:
	OperatorInfoGraphic();

	void draw();
	static VisualElement* NewVisualElement();
	static QSharedPointer<Asset> Create();

	//Resets the OperatorInfoGraphic Object to its initial state.
	virtual void reset();
	static const QString type;
	virtual QString friendlyTypeName(){return "Operator Info";};

public slots:
	void setData(QString field, QVariant value);
	int getDataAsInt(QString field){return getData(field).toInt();};
	QString getDataAsString(QString field){return getData(field).toString();};
	double getDataAsDouble(QString field){return getData(field).toDouble();};
	int getNumEntries(){return orderedFields_.size();};
	void updateValue();


protected:
	virtual QString defaultTagName(){return "OperatorInfo";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	bool deserializePropertiesFromXML(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QVariant getData(QString field);
	QStringList orderedFields_;
	QHash<QString,QVariant> infoMap_;
private slots:
	
};


}; //namespace Picto

#endif
