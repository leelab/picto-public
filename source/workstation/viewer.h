#ifndef _VIEWER_H_
#define _VIEWER_H_

#include "../common/experiment/experiment.h"
#include "../common/design/PictoData.h"

#include <QWidget>
#include <QSharedPointer>
#include <QTextDocument>

#include "../common/design/DesignRoot.h"
using namespace Picto;

/*!	\brief A Viewer is a base class for panels in the Workstation that are used to handle some clearly defined job.
 *
 *	\details Within PictoWorkstation, you change modes by selecting different Viewers.  For
 *	example, an Experiment Design can be viewed and edited in the StateEditViewer.  It can also be run in the
 *	TestViewer.
 *
 *	This base class implemements the minimum neccessary functionality: initialization, deinitialization,
 *	quitting and saving functionality are defined.  Every viewer also has a pointer to a main DesignRoot
 *	since they all operate on the same main DesignRoot in some way.
 *
 *	When we change from one viewer to another, the process is:
 *		1. Call deinit() on the old Viewer,
 *		2. Wait for the old Viewer to emit deinitComplete().
 *		3. Call init on the new Viewer
 *		4. Change focus to the new Viewer.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class Viewer : public QWidget
{
	Q_OBJECT
public:
	Viewer(QWidget * parent=0);
	virtual ~Viewer(){};

	/*! \brief Sets this Viewer's DesignRoot to the input.*/
	void setDesignRoot(QSharedPointer<DesignRoot> designRoot){designRoot_ = designRoot;};

	/*! \brief Returns a string containing the type of this viewer (e.g "State Edit Viewer").*/
	virtual QString type() = 0;
	/*! \brief This is called just before the application quits and is used to take-down anything that needs to be taken down before
	 *	a quit.  In particular, if there is some long running thread going on in the background, this function makes sure to end it.
	 */
	virtual bool aboutToQuit() { return true; };  //called just before quitting

public slots:
	/*! \brief Called just before displaying the viewer.  Handles any setup that needs to occur before the viewer takes focus.*/
	virtual void init() = 0;
	/*! \brief Called just before moving focus to a different viewer.  Handles any take-down that needs to occur before the viewer loses focus.
	 *	\note deinit() needs to make sure somehow that deinitComplete() is going to be emitted. The next viewer only takes focus once 
	 *	deinitComplete() is emitted.  See that signal for more details.
	 */
	virtual void deinit() = 0;
	/*! \brief Called just before the DesignRoot is saved to file.  Handles any actions that need to occur before that happens.*/
	virtual void aboutToSave() {};

signals:
	/*! \brief This is emitted when the Viewer's deinitialization process is completed.  
	 *	\details We don't just rely on the deinit() function returning because in some cases, deinitialization needs to wait for
	 *	another thread, or deinit() may even be getting called from underneath another function that needs to end before 
	 *	deinitialization is complete (TestViewer::deinit()).
	 */
	void deinitComplete();
	/*! \brief NO LONGER USED.  SHOULD PROBABLY BE DELETED.*/
	void enableCutAction(bool enable);
	/*! \brief NO LONGER USED.  SHOULD PROBABLY BE DELETED.*/
	void enableCopyAction(bool enable);
	/*! \brief NO LONGER USED.  SHOULD PROBABLY BE DELETED.*/
	void enablePasteAction(bool enable);

protected:
	QSharedPointer<DesignRoot> designRoot_;
};

#endif