
#ifndef _VISUAL_TARGET_HOST_H_
#define _VISUAL_TARGET_HOST_H_


#include <QSharedPointer>
#include <QWidget>
#include <QMouseEvent>

#include "VisualTarget.h"

namespace Picto {

/*! \brief A Widget that contains a Visual Target
 *
 *	This object needs some explanation.  If we want to run an engine and place a
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
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API VisualTargetHost :  public QWidget
#else
class VisualTargetHost : public QWidget
#endif
{
	Q_OBJECT
public:
	VisualTargetHost();

	void setVisualTarget(QSharedPointer<VisualTarget> target);
	QSharedPointer<VisualTarget> getVisualTarget() { return target_; };
signals:
	void clickDetected(QPoint pos);
	 
protected:
	void paintEvent(QPaintEvent *);
	virtual void mousePressEvent(QMouseEvent *event);
	
private:
	QSharedPointer<VisualTarget> target_;
private slots:
	void presented(double frameTime);
};


}; //namespace Picto

#endif
