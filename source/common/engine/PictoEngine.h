/*! \file PictoEngine.h
 * \ingroup picto_engine
 * \brief
 */

#ifndef _PICTOENGINE_H_
#define _PICTOENGINE_H_

#include "../common.h"
#include "../compositor/RenderingTarget.h"
#include "../experiment/Experiment.h"
#include "../task/Task.h"
#include "../network/CommandChannel.h"
#include "SignalChannel.h"

#include <QSharedPointer>

namespace Picto {
	namespace Engine {

/*! \addtogroup picto_engine
 * @{
 */

/*! \brief The namespace used for the PictoEngineTimingType enumeration
 *
 * This namespace is used to improve code readability; it simply contains the PictoEngineTimingType
 * enum.  This necessitates qualifying an enumerated type in context.  For example, instead of
 * referring to an enum as "brief" in your code, you would have to refer to it as ExampleType::brief.
 * In so doing, it is unambiguous what you are referring to.  To refer to the enum as a type itself 
 * you would simply repeat the namespace (in the example above it would be ExampleType::ExampleType).
 *
 */
namespace PictoEngineTimingType
{
	/*! \brief An enumerated type containing timing control options for PictoEngine objects
	 *
	 * PictoEngine objects can be set to have precise timing control, or to ignore frame intervals
	 * (to catch up a playback, allow for fast forward and rewind behaviors, enable generation of movie
	 * files, allow analysis of replayed data, etcetera).  PictoEngineTimingType enumerates the
	 * valid timing options.
	 * 
	 */
	typedef enum
	{
		precise,				/*!< Utilize precise timing */
		ignoreFrameIntervals	/*!< Ignore frame intervals during processing */
	} PictoEngineTimingType;
}

/*! \brief Executes state machines contained in Experiment objects
 *
 * The PictoEngine object takes an Experiment object and executes its contained state machines by rendering to one or
 * more RenderingTarget derived objects.  The PictoEngine can be set to have precise timing control, or to ignore
 * frame intervals (to catch up a playback, allow for fast forward and rewind behaviors, enable generation of movie
 * files, allow analysis of replayed data, etcetera).  The storage for behavioral data accomodates out of order
 * receipt.  Additionally a PictoEngine can be assigned a CommandChannel with which to communicate with a
 * PictoServer instance.
 * 
 */
struct PICTOLIB_CLASS PictoEngine
{
public:
	PictoEngine();

	/*! Sets the timing control for the PictoEngine to \a _timingType.
	    \param _timingType one of the PictoEngineTimingType::PictoEngineTimingType
		       enum values specifying the desired timing
		\return none
	 */
	void setTimingControl(PictoEngineTimingType::PictoEngineTimingType _timingType);

	void beginExclusiveMode();
	void endExclusiveMode();

	void loadExperiment(QSharedPointer<Picto::Experiment> experiment);

	void executeTask(Picto::Task * task);

	static QList<QSharedPointer<RenderingTarget> > getRenderingTargets();
	void addRenderingTarget(QSharedPointer<RenderingTarget> target);

	static QSharedPointer<SignalChannel> getSignalChannel(QString name);
	void addSignalChannel(QString name, QSharedPointer<SignalChannel> channel);

private:

	QSharedPointer<Picto::Experiment> experiment_;
	PictoEngineTimingType::PictoEngineTimingType timingType_;
	static QList<QSharedPointer<RenderingTarget> > renderingTargets_;
	QList<QSharedPointer<CommandChannel> > commandChannels_;
	static QMap<QString, QSharedPointer<SignalChannel> > signalChannels_;
	bool bExclusiveMode_;
};

/*! @} */

	}; //namespace Engine
}; //namespace Picto

#endif
