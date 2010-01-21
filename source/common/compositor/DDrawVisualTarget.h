/*!	\brief A visual target that uses the DirectDraw interface.  For use on PictoBox.
 *	
 *	This visual target uses the DirectDraw interface to present visual stimuli.
 *	DirectDraw is only supported by Windows CE, so this code is only included
 *	if the target OS is WinCE.  
 *
 *	The other VisualTargets allow us to run in windowed mode.  However, since 
 *	this target is going to be exclusively used on PictoBox, there is no
 *	reason to allow a windowed mode.
 *
 *	It should also be noted that some of the code was copied directly from the
 *	DirectDraw examples included with WindowsCE.
 *
 *	WARNING: Running in WindowsCE on the PictoBox, we found that any transparency
 *	(color-keyed or alpha) resulted in extremely slow rendering times.  As a result
 *	this code was abandoned and should be considered to be in an unknown state
 */

#ifndef _DDRAWVISUALTARGET_H_
#define _DDRAWVISUALTARGET_H_

#include "../common.h"

#include "VisualTarget.h"

#include <ddraw.h>
#include <QWidget>
#include <QList>

namespace Picto {

struct PICTOLIB_CLASS DDrawVisualTarget : public VisualTarget, public QWidget
{
public:
	DDrawVisualTarget(int _width = 800, int _height = 600);
	~DDrawVisualTarget();

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QString getTypeName();
	void draw(QPoint location, QSharedPointer<CompositingSurface> compositingSurface);
	void present();

	void drawNonExperimentText(QFont font, QColor color, QRect rect, Qt::AlignmentFlag alignment, QString text);

protected:
	void paintEvent(QPaintEvent *);

private:
	void initFail(QString);

	
	IDirectDraw                 *directDraw_;        // DirectDraw object
	LPDIRECTDRAWSURFACE         primarySurface_;// DirectDraw primary surface
	LPDIRECTDRAWSURFACE         backSurface_;   // DirectDraw back surface
};

}; //namespace Picto

#endif
