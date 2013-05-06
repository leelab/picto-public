#ifndef AssetDescriber_H
#define AssetDescriber_H

#include <QSharedPointer>
#include <QMap>
#include <QHash>
#include <QString>
#include <QStringList>

struct AssetDescription
{
	QString overview;
	QMap<QString,QString> props;
	QMap<QString,QString> scriptProps;
	QMap<QString,QString> scriptFunctions;
	QSharedPointer<AssetDescription> inherits;
	void setOverview(QString value){overview = value;};
	void setInherits(QSharedPointer<AssetDescription> parent){inherits = parent;};
	void addProp(QString name,QString description){props[name] = description;};
	void addSProp(QString name,QString description){scriptProps[name] = description;};
	void addSFunc(QString name,QString description){scriptFunctions[name] = description;};

	QString getOverview(){return overview;};
	QStringList getProperties();
	QStringList getScriptProperties();
	QStringList getScriptFunctions();
	QString getPropertyDescription(QString name);
	QString getScriptPropertyDescription(QString name);
	QString getScriptFunctionDescription(QString name);
};

class AssetDescriber
{
public:
	static QSharedPointer<AssetDescription> getAssetDescription(QString className);
private:
	static bool isSetup_;
	static QSharedPointer<AssetDescription> addDescription(QString className);
	static void setupDescriptions();
	static QHash<QString,QSharedPointer<AssetDescription>> assetHash_;
};
//! [0]

#endif
