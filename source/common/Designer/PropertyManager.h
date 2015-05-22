#ifndef _PROPERTY_MANAGER_H_
#define _PROPERTY_MANAGER_H_
#include <QtVariantProperty.h>
#include <QtPropertyBrowser.h>
#include <QMap>
#include "../common.h"

QT_BEGIN_NAMESPACE
class QWidget;
class QtProperty;
QT_END_NAMESPACE

/*! \brief Extends the QtVariantPropertyManager to create extra widgets needed by the Picto Designer and track the
 *	relationship between those widgets, their underlying QtProperties and the Property objects that they represent.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PropertyManager : public QtVariantPropertyManager
#else
class PropertyManager : public QtVariantPropertyManager
#endif
{
    Q_OBJECT

public:
	PropertyManager(QWidget *parent = 0);
	virtual ~PropertyManager(){};

	static int viewPropertyTypeId();

	virtual bool isPropertyTypeSupported(int propertyType) const;
	virtual int valueType(int propertyType) const;
	virtual QtVariantProperty *addProperty(int propertyType, const QString &name = QString());

private:
	int creatingType;
	QMap<const QtProperty *, QVariant> propsToVariants_;
	
	virtual QVariant PropertyManager::value(const QtProperty *property) const;

public slots:
	virtual void setValue(QtProperty *property, const QVariant &val);
	
protected:
	virtual void initializeProperty(QtProperty *property);
	virtual void uninitializeProperty(QtProperty *property);
};

#endif
