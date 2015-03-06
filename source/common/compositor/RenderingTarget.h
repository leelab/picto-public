/*! \file RenderingTarget.h
 * \brief
 */

#ifndef _RENDERINGTARGET_H_
#define _RENDERINGTARGET_H_


#include <QSharedPointer>
#include <QString>

#include "../common.h"

#include "MixingSample.h"
#include "VisualTarget.h"
#include "AuralTarget.h"

namespace Picto {

/*! \brief This is a top level container acting as a single location to access all of the various objects that are part of the rendering system for a specific type of rendering hardware.
 * \details RenderingTarget acts as a virtual base class for specialization to specific types of rendering hardware.
 * This could be pure software (e.g. for the web interface, movie file generation, printing, etcetera), pixmap based
 * (e.g. for the workstation), D3D based (e.g. for the director on a full Windows machine), DirectDraw based
 * (e.g. for the director on a PictoBox), or null based (e.g. for analysis).
 *
 * Each RenderingTarget contains a VisualTarget object and an AuralTarget object both of which act as
 * factory classes for elements that can then be combined by the renderer to produce the final output.
 * Convenience functions are also available for creating CompositingSurface and MixingSample objects directly rather than
 * going through these VisualTarget and AuralTarget classes.
 * Commonly used operations, including showing a splash screen or status information are also available.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
struct PICTOLIB_API RenderingTarget
#else
struct RenderingTarget
#endif
{
public:
    RenderingTarget(QSharedPointer<VisualTarget> visualTarget, QSharedPointer<AuralTarget> auralTarget);
	virtual ~RenderingTarget();

	virtual QSharedPointer<CompositingSurface> generateCompositingSurface();
	virtual QSharedPointer<MixingSample> generateMixingSample();

	QSharedPointer<VisualTarget> getVisualTarget() { return visualTarget_; };
	QSharedPointer<AuralTarget> getAuralTarget() { return auralTarget_; };

	void showSplash();
	void updateStatus(QString status) { status_ = status; };

private:
	QSharedPointer<VisualTarget> visualTarget_;
	QSharedPointer<AuralTarget> auralTarget_;
	QString status_;
};

}; //namespace Picto
#endif