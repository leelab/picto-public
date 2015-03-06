
#ifndef _OPERATORINFOGRAPHIC_H_
#define _OPERATORINFOGRAPHIC_H_

#include "../common.h"

#include "VisualElement.h"
#include "../property/PropertyContainer.h"
#include "../property/Property.h"

namespace Picto {

/*!	\brief A convenience class that essentially mixes a TextGraphic and a VariableMap to generate a graphic with experimental information for the operator.
 *	
 *	\details To use this graphic, call setData(field, value) from a script as many times as necessary to store data under field names.  When tne necessary
 *	data has been written, call updateValue() to cause the changes to take effect.  The Graphic will update its Fields property to include
 *	the input fields and the Values property to include the input values.  The information will be displayed on screen in a "FieldA: ValueA, FieldB: ValueB"
 *	format.  This can be used to simplify analysis as well.  Analysis data can be sent to objects of this type, and it can simply be read using getValues()
 *	during Analysis.  By default, the text in this graphic is set as green and displayed to the Operator but not the subject.
 *	\note If field ordering is relevant, make sure to call setData() in the order in which fields should be displayed.  The most
 *	convenient way to do this is likely to simply to call it lots of times in the correct order at the beginning of a task with
 *	default data in the value input.
 *	\note Like the TextGraphic, the size of this graphic determines the size of the graphic window in which text can be shown.  If it is too small some data
 *	will not be visible.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PICTOLIB_CLASS OperatorInfoGraphic : public VisualElement
{
	Q_OBJECT
public:
	OperatorInfoGraphic();

	void draw();
	static QSharedPointer<Asset> Create();
	static const QString type;

	//Resets the OperatorInfoGraphic Object to its initial state.
	virtual void enteredScope();
	virtual QString friendlyTypeName(){return "Operator Info";};
	virtual QString getUITemplate(){return "OperatorInfoElement";};
public slots:
	void setData(QString field, QVariant value);
	/*! \brief Gets the value of the input field and casts it to an int.
	*/
	int getDataAsInt(QString field){return getData(field).toInt();};
	/*! \brief Gets the value of the input field and casts it to a QString.
	*/
	QString getDataAsString(QString field){return getData(field).toString();};
	/*! \brief Gets the value of the input field and casts it to a double.  Equivalent to getDataAsNum().
	*/
	double getDataAsDouble(QString field){return getData(field).toDouble();};
	/*! \brief Gets the value of the input field and casts it to a double.
	*/
	double getDataAsNum(QString field){return getData(field).toDouble();};
	QVariantList getFields();
	QVariantList getValues();
	/*! \brief Returns the current number of entries in the Operator info graphic.
	 *	\details This is equal to the number of fields in the object.
	 */
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
	bool settingPropsFromMap_;
private slots:
	void propValueChanged(Property*,QVariant);
};


}; //namespace Picto

#endif
