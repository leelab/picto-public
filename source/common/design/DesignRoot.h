#ifndef _DESIGNROOT_H_
#define _DESIGNROOT_H_

#include <QObject>
#include <QTextDocument>
#include "PictoData.h"
//#include "Design.h"

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
	//QStringList getDesignIdentifiers();
	//int getDesignCount(QString identifier);
	//QSharedPointer<Design> getDesign(QString identifier,int index);
	//Creates a new Analysis using the input analysisText and adds it to the PictoData root.  Returns the new Analysis.
	QSharedPointer<Asset> importAnalysis(QString analysisText);
	//Removes the Analysis with the input index
	bool removeAnalysis(int index);

	QSharedPointer<Asset> getExperiment();
	int getNumAnalyses();
	QSharedPointer<Asset> getAnalysis(int index);
	QSharedPointer<Asset> getOpenAsset();
	void setOpenAsset(QSharedPointer<Asset> asset);

	void setUndoPoint();
	bool hasUndo();
	bool hasRedo();
	void undo();
	void redo();
	void refreshFromXml();//Rebuilds experiment from XML.  Hopefully we'll be able to get rid of this sometime, currently, we need it for deleting assets which requires serializing then deserializing.
	bool isModified();
	void setUnmodified();
	QString getDesignRootText();
	bool compiles(QString* errors = &QString());
	//The design starts in design mode.  Running the design is not possible until it
	//switches to run mode.  Use this function to switch to and from run mode.
	void enableRunMode(bool runMode);
	QString getDesignName(){Q_ASSERT(pictoData_);return pictoData_->getName();};
	void setDesignName(QString name){Q_ASSERT(pictoData_);pictoData_->setName(name);};
	bool hasError(){return lastError_.name != "";};
	DesignMessage getLastError(){DesignMessage returnVal = lastError_;lastError_.name="";return returnVal;};
	bool hasWarning(){return lastWarning_.name != "";};
	DesignMessage getLastWarning(){DesignMessage returnVal = lastWarning_;lastWarning_.name="";return returnVal;};

signals:
	void undoAvailable(bool available);
	void redoAvailable(bool available);

private:
	QSharedPointer<PictoData> pictoData_;
	QTextDocument designRootText_;
	QString designName;
	bool compiled_;
	DesignMessage lastError_;
	DesignMessage lastWarning_;

private slots:
	void designEdited();
};
};
#endif