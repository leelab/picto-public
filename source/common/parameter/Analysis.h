#ifndef _Analysis_H_
#define _Analysis_H_

#include "../common.h"
#include "AssociateRootHost.h"

namespace Picto {

/*! \brief The top level object that defines Analysis code for extracting useful data from a Picto Session.
 *	\details In Picto, Analyses are conceptual layers on top of an Experimental Design.  They are linked to the experiment
 *	with each Analysis Variable or data source linking into some Design Element parent, and AnalysisScriptHolders linking
 *	into StateMachineElements and defining all Analysis logic.  To accomplish this linking, an Analysis is implemented as
 *	an AssociateRoot and Analysis elements are implemented as AssociateElement objects.  The "Associate classes" take care
 *	of all of the linkage code such that the Analysis doesn't need to maintain its own tree structure.  The structure is
 *	defined by the Design elements that each Analysis element links too.  For that reason, the Analysis object tree itself
 *	flat, with the Analysis as the root, and all Analysis Elements as direct children of the Analysis.
 *
 *	Analyses are run by replaying an Experimental session.  As the session plays back and the ReplayViewer tracks its
 *	control flow, every time control flow reaches the position of an Analysis Script in an active Analysis, that script
 *	is run.  The Analysis Script can call any read-only function on the experimental design to gather data about the
 *	current state. It can also use AnalysisDataSources to gather signal data like lfp or spike information.  Data can be
 *	stored for later use in AnalysisVariable objects and output to file or other user interface using AnalysisOutput
 *	objects.  
 *
 *	In the UI, new Analysis objects are created in the Design Editor using a yellow plus sign in the toolbar.  A drop down
 *	is provided there to determine which Analysis is in active development.  The various Analysis Elements are available
 *	in the Toolbox on the left side of the design canvas for adding to the current Analysis.
 *
 *	\note The Analysis is both an AssociateRoot and an AssociateRootHost.  It is an AssociateRoot so that it can link
 *	into an Experiment.  It is an AnalysisRootHost so that a UIData object can link to it and define the positions of
 *	all of its Analysis element children in the Design Editor canvas.
 *
 *	\sa AssociateElement
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Analysis : public AssociateRoot, public AssociateRootHost
#else
class Analysis : public AssociateRoot, public AssociateRootHost
#endif
{
	Q_OBJECT
public:
	Analysis();
	virtual ~Analysis(){};
	static QSharedPointer<Asset> Create();
	virtual QString friendlyTypeName(){return "Analysis";};
	virtual void upgradeVersion(QString deserializedVersion);

	ASSOCIATE_ROOT_HOST_PUBLIC_IMPLEMENTATION

protected:
	virtual QString defaultTagName(){return "Analysis";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);

private:
	QSharedPointer<AssetFactory> variableFactory_;
	QSharedPointer<AssetFactory> dataSourceFactory_;
	QSharedPointer<AssetFactory> outputFactory_;
	QSharedPointer<AssetFactory> functionFactory_;
	QSharedPointer<AssetFactory> scriptFactory_;
	QSharedPointer<AssetFactory> dataViewElementFactory_;

	bool hostIdBeingEdited_;
private slots:
	void changeHostId(){if(hostIdBeingEdited_) return;hostIdBeingEdited_ = true;propertyContainer_->setPropertyValue("HostId",QUuid::createUuid());hostIdBeingEdited_ = false;};


};


}; //namespace Picto

#endif
