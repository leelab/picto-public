/*! * \file PictoEngine.cpp
 * \ingroup picto_engine
 * \brief
 */

#include "PictoEngine.h"

namespace Picto {
	namespace Engine {

QList<QSharedPointer<RenderingTarget> > PictoEngine::renderingTargets_;
QMap<QString, QSharedPointer<SignalChannel> > PictoEngine::signalChannels_;


PictoEngine::PictoEngine() :
	timingType_(PictoEngineTimingType::precise),
	bExclusiveMode_(false)
{
}

void PictoEngine::setTimingControl(PictoEngineTimingType::PictoEngineTimingType _timingType)
{
	timingType_ = _timingType;
}

void PictoEngine::beginExclusiveMode()
{
	/*! \todo Implement Me */
}

void PictoEngine::endExclusiveMode()
{
	/*! \todo Implement Me */
}

QList<QSharedPointer<RenderingTarget> > PictoEngine::getRenderingTargets()
{
	return renderingTargets_;
}

void PictoEngine::addRenderingTarget(QSharedPointer<RenderingTarget> target)
{	
	renderingTargets_.append(target);
}

QSharedPointer<SignalChannel> PictoEngine::getSignalChannel(QString name)
{
	return signalChannels_.value(name, QSharedPointer<SignalChannel>());
}

void PictoEngine::addSignalChannel(QString name, QSharedPointer<SignalChannel> channel)
{
	signalChannels_.insert(name, channel);
}

void PictoEngine::loadExperiment(QSharedPointer<Picto::Experiment> //experiment
								 )
{
	/*! \todo Implement Me */
}

void executeTask(Picto::Task * //task
				 )
{
	/*! \todo Implement Me */
}


	}; //namespace Engine
}; //namespace Picto
