#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtPlugin>
#include <QSharedPointer>
#include "../common/storage/NeuralDataUnit.h"
#include "../common/storage/AlignmentDataUnit.h"
#include "../common/storage/LFPDataUnitPackage.h"

/*! \brief Interface used for all proxy server plugins
 *
 *	This defines the interface used by proxy server plugins.  If you want to build a new
 *	plugin (for a different device), you'll need to make sure that it extends this class and 
 *	has these functions. Also, make sure to include:
 *	\code
		Q_OBJECT
		Q_INTERFACES(NeuralDataAcqInterface)
		Q_PLUGIN_METADATA(IID "org.blockdesign.Picto.ProxyServer.NeuralDataAcqInterface/1.0")
	\endcode
	at the beginning of the class definition.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
class NeuralDataAcqInterface
{
public:
	/*! \brief The status of the Neural Data Acquisition device handles by this Interface.*/
	enum deviceStatus
	{
		notStarted,		//!< Device is not ready.
		started,		//!< Device is ready to provide data.
		noData,			//!< Device is running but no data is currently available.
		hasData			//!< Device is running and data is available to be read.
	};

    virtual ~NeuralDataAcqInterface() {}

	/*! \brief The name of the device handled by this interface.*/
    virtual QString device() const = 0;
	/*! \brief Called when this device is first selected by the Proxy application.*/
	virtual void deviceSelected(){};
	/*! \brief Starts interfacing with the device and gathering data.  Returns the deviceStatus after the function was called.
	 *	\note The returned deviceStatus should be at least Started unless there was some error.
	 */
	virtual deviceStatus startDevice() = 0;
	/*! \brief Stops gathering data from the device handled by this interface.*/
	virtual deviceStatus stopDevice() = 0;
	/*! \brief Returns the current deviceStatus of this NeuralDataAcqInterface.*/
	virtual deviceStatus getDeviceStatus() = 0;
	/*! \brief Returns the rate at which spike waveforms are sampled by the device handled by this interface.*/
	virtual float samplingRate() = 0;
	/*! \brief Returns a list of DataUnits read from the Neural data device since the last call to this function.
	 *	\details The returned list includes NeuralDataUnit, AlignmentDataUnit, and LFPDataUnitPackage objects.  The
	 *	NeuralDataUnit and AlignmentDataUnit objects represent individual spikes and alignment events, but the 
	 *	LFPDataUnitPackage represents a list of LFP readings for a particular channel.  To keep the load on the server
	 *	low but balance that with fresh data on the workstation, the typical LFPDataUnitPackage should include about a half
	 *	second's worth of data.
	 */
	virtual QList<QSharedPointer<Picto::DataUnit>> dumpData() = 0;
	/*! \brief Causes the interface to throw out all data collected before now.  
	 *	\details No data gathered before this point should be returned by dumpData().  This function returns true if the 
	 *	data before this point was successfully thrown out, false otherwise.
	 */
	virtual bool acqDataAfterNow() = 0;
};

QT_BEGIN_NAMESPACE

Q_DECLARE_INTERFACE(NeuralDataAcqInterface,
                    "org.blockdesign.Picto.ProxyServer.NeuralDataAcqInterface/1.0")

QT_END_NAMESPACE



#endif