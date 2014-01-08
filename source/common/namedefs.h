/*! \file namedefs.h
 * \ingroup picto
 * \brief Names used for %Picto applications and hardware
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */

#ifndef _NAMEDEFS_H_
#define _NAMEDEFS_H_

#include "common.h"
#include <QCoreApplication>

namespace Picto {

/*! \brief Exposes the names used for %Picto applications and hardware
 *	\ingroup picto
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class TranslatedNames
{
	Q_DECLARE_TR_FUNCTIONS(TranslatedNames)

public:

	TranslatedNames();

	QString serverAppName;
	QString proxyServerAppName;
	QString configAppName;
	QString directorAppName;
	QString frontPanelAppName;
	QString directorHWName;
	QString workstationAppName;
};

}; //namespace Picto

/*! \defgroup picto Picto
 *  \defgroup picto_analysis Analysis
 *		\ingroup picto
 *  \defgroup picto_archives Archives
 *		\ingroup picto
 *  \defgroup picto_compositor Compositor
 *		\ingroup picto
 *  \defgroup picto_controlelements Control Elements
 *		\ingroup picto
 *  \defgroup picto_engine Engine
 *		\ingroup picto
 *  \defgroup picto_experiment Experiment
 *		\ingroup picto
 *  \defgroup picto_experimentalsystem Experimental System
 *		\ingroup picto
 *  \defgroup picto_network Network
 *		\ingroup picto
 *  \defgroup picto_parameter Parameter
 *		\ingroup picto
 *  \defgroup picto_protocol Protocol
 *		\ingroup picto
 *  \defgroup picto_random Random
 *		\ingroup picto
 *  \defgroup picto_statemachine State Machine
 *		\ingroup picto
 *  \defgroup picto_stimuli Stimuli
 *		\ingroup picto
 *  \defgroup picto_storage Storage
 *		\ingroup picto
 *  \defgroup picto_task Task
 *		\ingroup picto
 */

//  WARNING!!!!
//	These namespaces might be useful, but Doxygen gets confused when there is a namespace
//	defined here with the same name as a class (e.g. StateMachine).  To get around this,
//	(since I totally failed to use the namespaces in the first place) I am simply going to 
//	make these comments invisible to doxygen (by turning "/*!" into "/*".  If you want to
//	chagne this, feel free to do so, but pay attention to the list of classes.

/* \namespace Picto
 * \brief The namespace used for the main %Picto library
 *
 * This namespace contains everything within the main Picto library.
 *
 */

/* \namespace Picto::Analysis
 * \ingroup picto_analysis
 * \brief Components related to real-time and post-processing data analysis
 */

/* \namespace Picto::Archives
 * \ingroup picto_archives
 * \brief Components related to working with %Picto generated files
 */

/* \namespace Picto::Compositor
 * \ingroup picto_compositor
 * \brief Components related to combining stimuli for final output
 */

/* \namespace Picto::ControlElements
 * \ingroup picto_controlelements
 * \brief Components related to directing flow in a state machine
 */

/* \namespace Picto::Engine
 * \ingroup picto_engine
 * \brief Components related to execution of tasks defined in an experiment
 */

/* \namespace Picto::Experiment
 * \ingroup picto_experiment
 * \brief Components related to working with experiments
 */

/* \namespace Picto::ExperimentalSystem
 * \ingroup picto_experimentalsystem
 * \brief Components related to the physical hardware comprising a complete setup for running experiments
 */

/* \namespace Picto::Network
 * \ingroup picto_network
 * \brief Components related to network based communication
 */

/* \namespace Picto::Parameter
 * \ingroup picto_parameter
 * \brief Components related to investigator defined parameters for experiments
 */

/* \namespace Picto::Protocol
 * \ingroup picto_protocol
 * \brief Components related to the protocol used for transmitting data and commands between %Picto applications
 */

/* \namespace Picto::Random
 * \ingroup picto_random
 * \brief Components related to reproducible pseudo-random number generation
 */

/* \namespace Picto::StateMachine
 * \ingroup picto_statemachine
 * \brief Components related to the state machines that comprise a task
 */

/* \namespace Picto::Stimuli
 * \ingroup picto_stimuli
 * \brief Components related to sensory stimuli generation and presentation
 */

/* \namespace Picto::Storage
 * \ingroup picto_storage
 * \brief Components related to interfacing with data sources and storage of acquired data
 */

/* \namespace Picto::Task
 * \ingroup picto_task
 * \brief Components related to the tasks comprising an experiment
 */

#endif
