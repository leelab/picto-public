#ifndef _REMOTE_STATE_XML_HANDLER_H_
#define _REMOTE_STATE_XML_HANDLER_H_

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QStringList>
#include <QXmlStreamWriter>

namespace Picto
{

class PropertyDataUnit;
class BehavioralDataUnitPackage;
class InputDataUnitPackage;
class StateDataUnit;
class FrameDataUnit;
class RewardDataUnit;

class RemoteStateUpdater;

/*! \brief An interface meant for classes that that encapsulate the XML handling for different data units for the
 *	RemoteStateUpdater.
 *
 *	\details This was created to simplify and speed up the XML handling process in the RemoteStateUpdater.
 *	\author Trevor, Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class RemoteStateXMLHandler : public QObject
{
	Q_OBJECT
public:
	/*! \brief RemoteStateXMLHandler constructor.
	 */
	RemoteStateXMLHandler() {};
	/*! \brief RemoteStateXMLHandler destructor.
	 */
	virtual ~RemoteStateXMLHandler() {};

	/*! \brief A virtual function meant to contain the implementation details for the subclasses.  This is where the class
	 *	would take care of its respective data package.
	 */
	virtual void handle(QSharedPointer<QXmlStreamReader> xmlReader) = 0;
};

/*! \brief This class handles incoming PropertyDataUnit XML.
 *	\author Trevor, Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class PropertyDataUnitHandler : public RemoteStateXMLHandler
{
	Q_OBJECT
public:
	PropertyDataUnitHandler(RemoteStateUpdater *pParent);
	virtual ~PropertyDataUnitHandler() {};

	virtual void handle(QSharedPointer<QXmlStreamReader> xmlReader);
	/*! \brief A static function that returns the XML Identifier for the package.  Used to index the handler in a map.
	 */
	static QString getDataPackageName() { return QString("PDU"); };
signals:
	/*!	\brief Emitted when a Property value changes for a Property with an AssetId of propId and a new value of value.
	 *	\sa RemoteStateUpdater::propertyValueChanged
	 */
	void propertyValueChanged(int propId, QString value);
	/*!	\brief Emitted when a Property initValue changes for a Property with an AssetId of propId and a new initValue of
	 *	value.
	 *	\sa RemoteStateUpdater::propertyInitValueChanged
	 */
	void propertyInitValueChanged(int propId, QString value);
protected:
	//! A local DataUnit to avoid unnecessary allocations.
	QSharedPointer<PropertyDataUnit> propUnit_;
};


/*! \brief This class handles incoming BehavioralData XML.
 *	\author Trevor, Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class BehavioralDataUnitHandler : public RemoteStateXMLHandler
{
	Q_OBJECT
public:
	BehavioralDataUnitHandler(RemoteStateUpdater *pParent);
	virtual ~BehavioralDataUnitHandler() {};

	virtual void handle(QSharedPointer<QXmlStreamReader> xmlReader);
	/*! \brief A static function that returns the XML Identifier for the package.  Used to index the handler in a map.
	*/
	static QString getDataPackageName() { return QString("BDUP"); };
signals:
	/*! \brief Emitted when new signal data comes in.
	 *	\sa RemoteStateUpdater::signalChanged
	 */
	void signalChanged(QString name, QStringList subChanNames, QVector<float> vals);
protected:
	//! A local DataUnit to avoid unnecessary allocations.
	QSharedPointer<BehavioralDataUnitPackage> behavUnitPack_;
};


/*! \brief This class handles incoming InputData XML.
 *	\author Trevor, Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class InputDataUnitHandler : public RemoteStateXMLHandler
{
	Q_OBJECT
public:
	InputDataUnitHandler(RemoteStateUpdater *pParent);
	virtual ~InputDataUnitHandler() {};

	virtual void handle(QSharedPointer<QXmlStreamReader> xmlReader);
	/*! \brief A static function that returns the XML Identifier for the package.  Used to index the handler in a map.
	*/
	static QString getDataPackageName() { return QString("IDUP"); };
signals:
	/*! \brief Emitted when new signal data comes in.
	 *	\sa RemoteStateUpdater::signalChanged
	 */
	void signalChanged(QString name, QStringList subChanNames, QVector<float> vals);
protected:
	//! A local DataUnit to avoid unnecessary allocations.
	QSharedPointer<InputDataUnitPackage> inputUnitPack_;
};


/*! \brief This class handles incoming StateData XML.
*	\author Trevor, Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2015
*/
class StateDataUnitHandler : public RemoteStateXMLHandler
{
	Q_OBJECT
public:
	StateDataUnitHandler(RemoteStateUpdater *pParent);
	virtual ~StateDataUnitHandler() {};

	virtual void handle(QSharedPointer<QXmlStreamReader> xmlReader);
	/*! \brief A static function that returns the XML Identifier for the package.  Used to index the handler in a map.
	*/
	static QString getDataPackageName() { return QString("SDU"); };
signals:
	/*! \brief Emitted when a Transition with AssetId of transId is traversed.
	 *	\sa RemoteStateUpdater::transitionActivated
	 */
	void transitionActivated(int transId);
protected:
	//! A local DataUnit to avoid unnecessary allocations.
	QSharedPointer<StateDataUnit> stateUnit_;
};


/*! \brief This class handles incoming FrameData XML.
*	\author Trevor, Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2015
*/
class FrameDataUnitHandler : public RemoteStateXMLHandler
{
	Q_OBJECT
public:
	FrameDataUnitHandler(RemoteStateUpdater *pParent);
	virtual ~FrameDataUnitHandler() {};

	virtual void handle(QSharedPointer<QXmlStreamReader> xmlReader);
	/*! \brief A static function that returns the XML Identifier for the package.  Used to index the handler in a map.
	*/
	static QString getDataPackageName() { return QString("FDU"); };
signals:
	/*! \brief Emitted when a new frame is presented.  time is the time at which the first phosphor appeared on the
	 *	display for this frame.
	 *	\sa RemoteStateUpdater::framePresented
	 */
	void framePresented(double time);
	/*!	\brief Emitted to update the parent StateUpdater with the currUnitTime
	 */
	void updateCurrUnitTime(QString time);
protected:
	//! A local DataUnit to avoid unnecessary allocations.
	QSharedPointer<FrameDataUnit> frameUnit_;
};


/*! \brief This class handles incoming RewardData XML.
*	\author Trevor, Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
*	\date 2009-2015
*/
class RewardDataUnitHandler : public RemoteStateXMLHandler
{
	Q_OBJECT
public:
	RewardDataUnitHandler(RemoteStateUpdater *pParent);
	virtual ~RewardDataUnitHandler() {};

	virtual void handle(QSharedPointer<QXmlStreamReader> xmlReader);
	/*! \brief A static function that returns the XML Identifier for the package.  Used to index the handler in a map.
	*/
	static QString getDataPackageName() { return QString("RDU"); };
signals:
	/*! \brief Emitted when a reward is triggered.
	*	\details time is the time when the reward starts, duration is its duration in ms, channel is the
	*	reward channel on which the reward was provided.
	 *	\sa RemoteStateUpdater::rewardSupplied
	*/
	void rewardSupplied(double time, int duration, int channel);
	/*!	\brief Emitted to update the parent StateUpdater with the currUnitTime
	*/
	void updateCurrUnitTime(QString time);
protected:
	//! A local DataUnit to avoid unnecessary allocations.
	QSharedPointer<RewardDataUnit> rewardUnit_;
};

}; //namespace Picto

#endif