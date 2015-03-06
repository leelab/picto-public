/*! \file namedefs.h
 * \brief Names used for %Picto applications and hardware
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */

#ifndef _NAMEDEFS_H_
#define _NAMEDEFS_H_

#include "common.h"
#include <QCoreApplication>

namespace Picto {

/*! \brief Exposes the names used for %Picto applications and hardware
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
 * \brief Components related to real-time and post-processing data analysis
 */

/* \namespace Picto::Archives
 * \brief Components related to working with %Picto generated files
 */

/* \namespace Picto::Compositor
 * \brief Components related to combining stimuli for final output
 */

/* \namespace Picto::ControlElements
 * \brief Components related to directing flow in a state machine
 */

/* \namespace Picto::Engine
 * \brief Components related to execution of tasks defined in an experiment
 */

/* \namespace Picto::Experiment
 * \brief Components related to working with experiments
 */

/* \namespace Picto::ExperimentalSystem
 * \brief Components related to the physical hardware comprising a complete setup for running experiments
 */

/* \namespace Picto::Network
 * \brief Components related to network based communication
 */

/* \namespace Picto::Parameter
 * \brief Components related to investigator defined parameters for experiments
 */

/* \namespace Picto::Protocol
 * \brief Components related to the protocol used for transmitting data and commands between %Picto applications
 */

/* \namespace Picto::Random
 * \brief Components related to reproducible pseudo-random number generation
 */

/* \namespace Picto::StateMachine
 * \brief Components related to the state machines that comprise a task
 */

/* \namespace Picto::Stimuli
 * \brief Components related to sensory stimuli generation and presentation
 */

/* \namespace Picto::Storage
 * \brief Components related to interfacing with data sources and storage of acquired data
 */

/* \namespace Picto::Task
 * \brief Components related to the tasks comprising an experiment
 */

#endif
