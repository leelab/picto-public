
#ifndef _VISUAL_TARGET_HOST_H_
#define _VISUAL_TARGET_HOST_H_


#include <QSharedPointer>
#include <QWidget>
#include <QMouseEvent>

#include "VisualTarget.h"

namespace Picto {

/*! \brief A Widget that contains a Visual Target
 *
 *	\note Joey: This object seems to have been created due to an incomplete understanding
 *	of the QWidget system.  Whenever a QWidget is deleted, all of its descendant QWidgets are
 *	automatically deleted as well.  For some reason, the RenderingTarget object is holding 
 *	a shared pointer to its VisualTarget object which also automatically deletes the VisualTarget
 *	when it is deleted.  Apparently, the idea behind this object is to never actually set a parent
 *	to the VisualTarget in the QWidget system so that we can just use the QSharedPointer system to
 *	delete it.  Probably, in practice we just should never be using QSharedPointer's on VisualTargets
 *	and we should parent it within the QWidget system the way we are supposed to.  In practice though,
 *	since the current system works, and we don't have time to fix it, we are going to leave it alone
 *	for now.  If you want to fix it though, go for it.  The original notes on this class appear below
 *	for your reference.
 *	
 *	\details This object needs some explanation.  If we want to run an engine and place a
 *	VisualTarget within another widget, the obvious approach would be:
 *		QSharedPointer<VisualTarget> visualTarget(new VisualTarget());
 *		....
 *		QSharedPointer<RenderingTarget> renderingTarget(new RenderingTarget(visualTarget, auralTarget));
 *		...
 *		mainLayout->addWidget(visualTarget.data());
 *	However, this approach will blow up during destruction.  The visual target will
 *	be deleted twice.  Once when the rendering target is deleted (because it holds
 *	the only strong reference), and once when the main window is deleted (because
 *	the visual target was reparented when it was added to the layout).  Needless to 
 *	say, the second call to delete results in a memory exception.
 *
 *	This object works around this problem by holding a strong reference to the pointer, and
 *	drawing the Visual Target directly (without reparenting it).
 *
 *	NOTE: As a result of this situation, I have come to the conclusion that it is 
 *	generally a bad idea to create QSharedPointer<QWidget>, because both QWidgets and
 *	QSharedPointers have lots of automatic calls to delete that occur behind the 
 *	scenes.
 *	\note That was the end of the original notes
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API VisualTargetHost :  public QWidget
#else
class VisualTargetHost : public QWidget
#endif
{
	Q_OBJECT
public:
	VisualTargetHost(QColor bgColor=2);
	virtual ~VisualTargetHost(){};

	virtual void resizeEvent(QResizeEvent *event);
	void setVisualTarget(QSharedPointer<VisualTarget> target);
	/*! \brief Returns the VisualTarget hosted by this VisualTargetHost*/
	QSharedPointer<VisualTarget> getVisualTarget() { return target_; };
	const QPoint viewportPointToTargetPoint(const QPoint point) const;
signals:
	/*! \brief Emitted with a click position whenever someone clicks within the VisualTargetHost widget*/
	void clickDetected(QPoint pos);
	 
protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void mousePressEvent(QMouseEvent *event);
	
private:
	QSharedPointer<VisualTarget> target_;

	QColor BgCol_;
private slots:
	void presented(double frameTime);
};

}; //namespace Picto

#endif
