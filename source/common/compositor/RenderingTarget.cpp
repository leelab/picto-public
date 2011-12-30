/*! \file RenderingTarget.cpp
 * \ingroup picto_compositor
 * \brief
 */

#include "RenderingTarget.h"
#include "../stimuli/PictureGraphic.h"
#include "../stimuli/TextGraphic.h"
#include "../memleakdetect.h"

namespace Picto {

RenderingTarget::RenderingTarget(QSharedPointer<VisualTarget> visualTarget,
								 QSharedPointer<AuralTarget> auralTarget)
{
	visualTarget_ = visualTarget;
	auralTarget_ = auralTarget;
}

RenderingTarget::~RenderingTarget()
{
	printf("RenderTarget destructor");
}

QSharedPointer<CompositingSurface> RenderingTarget::generateCompositingSurface()
{
	return visualTarget_->generateCompositingSurface();
}

QSharedPointer<MixingSample> RenderingTarget::generateMixingSample()
{
	return auralTarget_->generateMixingSample();
}

/*!	\brief Displays a splash screen on the visual target
 *
 *	This displays a Picto splash screen on the visual target.  It is used at
 *	startup, and for debugging (since the splash screen includes the ability to
 *	write strings to the screen using updateStatus).
 */
void RenderingTarget::showSplash()
{
	QSharedPointer<CompositingSurface> compositingSurface = generateCompositingSurface();
	QSharedPointer<Picto::PictureGraphic> pictureGraphic(
											new PictureGraphic(QPoint(0,0),":/common/images/splash.png"));
	pictureGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
	QRect splashScreenRect = pictureGraphic->getBoundingRect();
	QRect visualTargetRect = visualTarget_->getDimensions();
	pictureGraphic->setPosition(QPoint((visualTargetRect.width()-splashScreenRect.width())/2,
								       (visualTargetRect.height()-splashScreenRect.height())/2));
	visualTarget_->draw(pictureGraphic->getPosition(), QPoint(),pictureGraphic->getCompositingSurface(compositingSurface->getTypeName()));

	compositingSurface = generateCompositingSurface();
	int textHeight = 20;
	QPoint textPos = QPoint(0,visualTargetRect.bottom()-textHeight);
	QRect textRect = QRect(0,0,visualTargetRect.width(), textHeight);
	QSharedPointer<TextGraphic> textGraphic(new TextGraphic(textPos,textRect,QColor(255,255,255),status_));
	textGraphic->addCompositingSurface(compositingSurface->getTypeName(),compositingSurface);
	visualTarget_->draw(textGraphic->getPosition(),QPoint(),textGraphic->getCompositingSurface(compositingSurface->getTypeName()));
	
	visualTarget_->present();
}

}; //namespace Picto
