#ifndef PICTODEVICESIMULATORTYPES_H
#define PICTODEVICESIMULATORTYPES_H
#include <QMetaType>
#include <QtTest/QtTest>
namespace PictoSim{

/*! \brief Helper macros for use in defining tests using the PictoTestBench
 */
#define ACTOR_PARAM QSharedPointer<SimActorDesc>
#define ACTION_PARAM QSharedPointer<SimActionDesc>
#define ACTORPARAM(...) QSharedPointer<SimActorDesc>( new SimActorDesc(__VA_ARGS__) )
#define ACTIONPARAM(X) QSharedPointer<SimActionDesc>( X )

/*! \brief A parent class for use in defining messages to be sent by PictoDeviceSimulators.
 *	Each message must have a timestamp and a type.  The type is used by the PictoDeviceSimulator
 *	to identify the type of message desired.
 */
struct SimMsgDesc
{
	SimMsgDesc(float timestamp,int type):
		timestamp_(timestamp),
		type_(type)
		{};
	float timestamp_;
	int type_;
};

/*! \brief The various types of PictoDeviceSimulators available in the PictoTestBench
 */
typedef enum {	
				TESTBENCH,
				WORKSTATION,
				DIRECTOR,
				PROXYSERVER,
				SERVER
			} SimActorType;

/*! \brief Used to identify the PictoDeviceSimulator that will perform an action defined by a SimActionDesc
 *	Each SimActorDesc has a SimActorType and a name.  Used to create or identify the desired PictoDeviceSimultor
 *	depending whether it already exists.
 */
struct SimActorDesc
{
	SimActorDesc(){};
	SimActorDesc(SimActorType type, QString name = ""){type_ = type;name_ = name;};
	SimActorType type_;
	QString name_;
};
Q_DECLARE_METATYPE(QSharedPointer<SimActorDesc>)

/*! \brief Used to define an action to be performed by a PictoDeviceSimulator.
 *	Each SimActionDesc has:
 *		-	a testStep string used for output in the case of a test failure,
 *		-	a type used by the PictoDeviceSimulator to identify the action.
 *		-	a breakPrevAction flag used to indicate whether the PictoDeviceSimulator
 *			should wait for the previous action to complete or break it before performing
 *			this action.
 */
struct SimActionDesc
{
	SimActionDesc(){};
	SimActionDesc(QString testStep, int type, bool breakPrevAction = false){testStep_ = testStep;type_ = type;breakPrevAction_ = breakPrevAction;};
	QString testStep_;
	int type_;
	bool breakPrevAction_;
	/*! \brief A list of messages associated with this action (ie. NeuralData messages for ProxyServer).
	 */
	QVector<QSharedPointer<SimMsgDesc>> msgs_;
	/*! \brief A helper function used when defining a test to add messages to this SimActionDesc
	*/
	void AddMsg(QSharedPointer<SimMsgDesc> msg){msgs_.push_back(msg);}
};
Q_DECLARE_METATYPE(QSharedPointer<SimActionDesc>)

}; // PictoSim namespace
#endif