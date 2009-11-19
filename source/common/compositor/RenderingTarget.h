/*! \file RenderingTarget.h
 * \ingroup picto_compositor
 * \brief
 */

#ifndef _RENDERINGTARGET_H_
#define _RENDERINGTARGET_H_

#include "../common.h"

#include "VisualTarget.h"
#include "AuralTarget.h"

#include <QWidget>
#include <QHostAddress>
#include <QSharedPointer>

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
struct PICTOLIB_API RenderingTarget : QWidget
#else
struct RenderingTarget : QWidget
#endif
{
	Q_OBJECT

public:
    RenderingTarget(QSharedPointer<VisualTarget> visualTarget, QSharedPointer<AuralTarget> auralTarget, bool bWindowed = false, int width = 800, int height = 600, QWidget *parent = 0);

	QSharedPointer<CompositingSurface> generateCompositingSurface();
	QSharedPointer<MixingSample> generateMixingSample();

	void updateStatus(QString status);
	void showSplash(bool visible=true);

protected:
	void paintEvent(QPaintEvent *);

private:
	QSharedPointer<VisualTarget> visualTarget_;
	QSharedPointer<AuralTarget> auralTarget_;

	bool bWindowed_;
	bool bShowSplash_;
	QString status_;

private slots:
	void foundServer(QHostAddress serverAddress, quint16 serverPort);
	void discoverServerFailed();
};

/*! @} */

#endif

}; //namespace Picto
