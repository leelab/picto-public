#ifndef _DESIGNROOT_H_
#define _DESIGNROOT_H_

#include <QObject>
#include <QTextDocument>
#include "PictoData.h"
#include "Design.h"

namespace Picto{

struct DesignMessage
{
	DesignMessage(){name="";details="";};
	QString name;
	QString details;
};
/*!	\brief The root of the Experiment/Analysis Design tree passed between different UI Viewers
 * 
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API DesignRoot : public QObject
#else
class DesignRoot : public QObject
#endif
{
	Q_OBJECT
public:
	DesignRoot();
	virtual ~DesignRoot(){};

	bool resetDesignRoot(QString DesignRootText);
	QStringList getDesignIdentifiers();
	int getDesignCount(QString identifier);
	QSharedPointer<Design> getDesign(QString identifier,int index);
	//Creates a new design of the identifier type (top level tag name) using the input designText and adds it to the design root.  Returns the new design.
	QSharedPointer<Design> importDesign(QString identifier,QString designText);
	//Removes the design with the input identifier and index
	bool removeDesign(QString identifier,int index);
	void refreshFromXml();//Rebuilds experiment from XML.  Hopefully we'll be able to get rid of this sometime, currently, we need it for deleting assets which requires serializing then deserializing.
	bool isModified();
	void setUnmodified();
	QString getDesignRootText();
	bool compiles();
	//The design starts in design mode.  Running the design is not possible until it
	//switches to run mode.  Use this function to switch to and from run mode.
	void enableRunMode(bool runMode);
	QString getDesignName(){return designName;};
	void setDesignName(QString name){designName = name;};
	bool hasError(){return lastError_.name != "";};
	DesignMessage getLastError(){DesignMessage returnVal = lastError_;lastError_.name="";return returnVal;};
	bool hasWarning(){return lastWarning_.name != "";};
	DesignMessage getLastWarning(){DesignMessage returnVal = lastWarning_;lastWarning_.name="";return returnVal;};

private:
	QMap<QString,QVector<QSharedPointer<Design>>> designMap_;
	QSharedPointer<PictoData> pictoData_;
	QString designName;
	DesignMessage lastError_;
	DesignMessage lastWarning_;
};
};
#endif