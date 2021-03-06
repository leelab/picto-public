#ifndef AssetDescriber_H
#define AssetDescriber_H

#include <QSharedPointer>
#include <QMap>
#include <QHash>
#include <QString>
#include <QStringList>

/*! \brief Contains documentation for the usage of a scriptable element of a Picto design both overall and within the
 *	scripting framework.
 *	\details In addition to documenting element properties, script properties, and script functions, an element overview
 *	is provided and its class inheritance is stored so that inherited values can also be included in documentation.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct AssetDescription
{
	//! An overview of the class described by this object
	QString overview;	
	//! Any dire warnings associated with the class
	QString warning;
	//! A lookup table of Property descriptions by property name
	QMap<QString,QString> props;
	//! A lookup table of script property descriptions by script property name
	QMap<QString,QString> scriptProps;
	//! A lookup table of script function descriptions by script function name.
	QMap<QString,QString> scriptFunctions;
	//! A pointer to the AssetDescription of this AssetDescription's class's parent class.
	QSharedPointer<AssetDescription> inherits;	
	//! Sets an descriptive overview of the class that this AssetDescription is handling.
	void setOverview(QString value){ overview = value; };
	//! Sets the warning for the class that this AssetDescription is handling.
	void setWarning(QString value){ warning = value; };
	/*! \brief Sets the AssetDescription of the class that this AssetDescription's class inherits so that its
	 *	documentation can be included with this class's documentation.
	 */
	void setInherits(QSharedPointer<AssetDescription> parent){ inherits = parent; };
	/*! \brief Adds the input description for the property with the input name.
	 *	\sa getPropertyDescription(), getProperties()
	 */
	void addProp(QString name,QString description){ props[name] = description; };
	/*! \brief Adds the input description for the script property with the input name.
	 *	\sa getScriptPropertyDescription(), getScriptProperties()
	 */
	void addSProp(QString name,QString description){ scriptProps[name] = description; };
	/*! \brief Adds the input description for the script function with the input name.
	 *	\sa getScriptFunctionDescription(), getScriptFunctions()
	 */
	void addSFunc(QString name,QString description){ scriptFunctions[name] = description; };

	QString getOverview();
	QString getWarning();
	QStringList getProperties();
	QStringList getScriptProperties();
	QStringList getScriptFunctions();
	QString getPropertyDescription(QString name);
	QString getScriptPropertyDescription(QString name);
	QString getScriptFunctionDescription(QString name);
};

/*! \brief A tool used to query documentation for classes used in the Picto Designer.
 *	\details Documentation gathered from this class is displayed in a widget at the bottom of the designer to streamline
 *	the development process by providing designers with in-application descriptions of element capabilities.
 *	Documentation is provided in the form of separate AssetDescription objects for each class.
 *	\note It would make more sense if the Asset Description text was somehow stored with the actual class itself rather
 *	than all being lumped together here.  It might be a good idea to figure out the best way to do that at some point.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class AssetDescriber
{
public:
	static QSharedPointer<AssetDescription> getAssetDescription(QString className);
private:
	static QSharedPointer<AssetDescription> addDescription(QString className);
	static void setupDescriptions();

	//! Indicates whether documentation text has been fully loaded into the AssetDescriber system.
	static bool isSetup_;
	//! A lookup table containing AssetDescription objects under className keys.
	static QHash<QString,QSharedPointer<AssetDescription>> assetHash_;
};


#endif
