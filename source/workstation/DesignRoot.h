#ifndef _DESIGNROOT_H_
#define _DESIGNROOT_H_

#include <QObject>
#include <QTextDocument>
#include "PictoData.h"
#include "Design.h"
using namespace Picto;


/*!	\brief The root of the Experiment/Analysis Design tree passed between different UI Viewers
 * 
 */
class DesignRoot : public QObject
{
	Q_OBJECT
public:
	DesignRoot();
	virtual ~DesignRoot(){};

	bool resetDesignRoot(QString DesignRootText);
	QStringList getDesignIdentifiers();
	int getDesignCount(QString identifier);
	QSharedPointer<Design> getDesign(QString identifier,int index);
	void refreshFromXml();//Rebuilds experiment from XML.  Hopefully we'll be able to get rid of this sometime, currently, we need it for deleting assets which requires serializing then deserializing.
	bool isModified();
	void setUnmodified();
	QString getDesignRootText();
	bool compiles();

private:
	QMap<QString,QVector<QSharedPointer<Design>>> designMap_;
};
#endif