/*! \file RenderingTarget.h
 * \ingroup picto_compositor
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

/*! \addtogroup picto_compositor
 * @{
 */

/*! \brief Generates CompositingSurface and MixingSample objects, composites stimuli, and renders the output
 *
 * RenderingTarget acts as a virtual base class for specialization to specific types of rendering hardware.
 * This could be pure software (e.g. for the web interface, movie file generation, printing, etcetera), pixmap based
 * (e.g. for the workstation), D3D based (e.g. for the director on a full Windows machine), DirectDraw based
 * (e.g. for the director on a PictoBox), or null based (e.g. for analysis).
 *
 * Each RenderingTarget contains a VisualTarget object and an AuralTarget object both of which act as
 * factory classes for elements that can then be combined by the renderer to produce the final output.
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

/*! @} */

}; //namespace Picto
#endif