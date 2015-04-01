#ifndef _DESIGNROOT_H_
#define _DESIGNROOT_H_

#include <QObject>
#include <QTextDocument>
#include "PictoData.h"

namespace Picto{

struct DesignMessage
{
	DesignMessage(){name="";details="";};
	QString name;
	QString details;
};
/*!	\brief The root of the Experiment/Analysis Design tree passed between different UI Viewers
 *	\details the DesignRoot is a high level class that can be passed between different components and viewers in Picto
 *	and provides easy to access to the experimental design and analyses as well as undo/redo operations and
 *	serialization/deserialization methods that include automatic upgrading.  It is essentially the user-friendly wrapper
 *	for the experimental design.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
	//Rebuilds experiment from XML.  Hopefully we'll be able to get rid of this sometime, currently, we need it
	//  for deleting assets which requires serializing then deserializing.
	void refreshFromXml();
	bool isModified();
	void setUnmodified();
	QString getDesignRootText();
	bool compiles(QString* errors = &QString());
	//The design starts in design mode.  Running the design is not possible until it switches to run mode. 
	//  Use this function to switch to and from run mode.
	void enableRunMode(bool runMode);
	/*! \brief Returns a QString containing the name of the experimental design
	 */
	QString getDesignName(){Q_ASSERT(pictoData_);return pictoData_->getName();};
	/*! \brief Sets a new name for the experimental design
	 */
	void setDesignName(QString name){Q_ASSERT(pictoData_);pictoData_->setName(name);};
	/*! \brief Returns true if an error occured in deserializing xml into a design since the last time getLastError()
	 *	was called., false otherwise.
	 */
	bool hasError(){return lastError_.name != "";};
	/*! \brief Returns the latest deserialization error message, and clears it out in the process.
	 */
	DesignMessage getLastError(){DesignMessage returnVal = lastError_;lastError_.name="";return returnVal;};
	/*! \brief Returns true if a warning occured in deserializing xml into a design since the last time getLastWarning()
	 *	was called., false otherwise.
	 */
	bool hasWarning(){return lastWarning_.name != "";};
	/*! \brief Returns the latest deserialization warning message, and clears it out in the process.
	 *	\details A DesignRoot warning indicates that Deserialization was successful, but there is something
	 *	that the user should know about.
	 *	\note This is currently used to inform the user that design contents were automatically upgraded.
	 */
	DesignMessage getLastWarning(){DesignMessage returnVal = lastWarning_;lastWarning_.name="";return returnVal;};

signals:
	/*! \brief Emitted when undo operations become available, or stop being available, according to the 'available'
	 *	boolean parameter.
	 */
	void undoAvailable(bool available);
	/*! \brief Emitted when redo operations become available, or stop being available, according to the 'available'
	 *	boolean parameter.
	 */
	void redoAvailable(bool available);
	/*! \brief Emitted whenever the underlying design object are reconstructed from xml design text.
	 */
	void refreshedFromXml();

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