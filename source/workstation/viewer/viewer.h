#ifndef _VIEWER_H_
#define _VIEWER_H_

#include "../common/experiment/experiment.h"

#include <QWidget>
#include <QSharedPointer>
#include <QTextDocument>

/*!	\brief A Viewer is a main panel used to view an experiment.  This is a base class
 *
 *	Within PictoWorkstation, you change modes by selecting different viewers.  For
 *	example, an experiment can be viewed as raw XML, or as a state machine.  You
 *	can also view the experiment as a running experiment, or as a test run, or within
 *	a debugger.
 *
 *	This base class atmepts to implemement the minimum neccessary functionality.  However
 *	since the viewers are all so different, this will be a pretty simple class
 *	and most of the work withh be in the derived classes.
 *
 *	Each viewer is allocated a single menu that it is given full control over.  The setupMenu
 *	function is called by the mainwindow along with a menu which is being allocated to that 
 *	viewer.
 *
 *	The relationship between experiment_ and experimentText_ needs to be explained.  Since
 *	it is likely that uring the early stages of developing an experiment, the XML for the
 *	experiment will be incomplete, we need to be able to store the actual text.  If the
 *	experiment text is legal, experiment_ will point to the object representation of the
 *	text.  Otherwise, it will be a null pointer.
 *
 *	When we change from one viewer to another, the process is:
 *		1. Call deinit on the old viewer
 *		2. Attempt to convert experiment text to an experiment object
 *		3. Call init on the new viewer
 *	This process makes keeping the engine and engineText in sync easy.  For example
 *	if the state machine editor viewer is operating on the experiment object
 *	directly, it only needs convert it to XML in the deinit() function.
 *
 */

class Viewer : public QWidget
{
	Q_OBJECT
public:
	Viewer(QWidget * parent=0);

	void setExperiment(QSharedPointer<Picto::Experiment> experiment) { experiment_ = experiment; }
	void setExperimentText(QTextDocument *experimentText) {experimentText_ = experimentText; }

	//Returns the type of viewer (e.g "Text")
	virtual QString type() = 0;
	virtual bool aboutToQuit() { return true; };  //called just before quitting

public slots:
	virtual void init() = 0;  //Called just before displaying the viewer
	virtual void deinit() = 0;	//Called just after the user switches out of the viewer

signals:
	void enableCutAction(bool enable);
	void enableCopyAction(bool enable);
	void enablePasteAction(bool enable);

protected:
	QSharedPointer<Picto::Experiment> experiment_;
	QTextDocument *experimentText_;
};

#endif