
#include <QDebug>

#include "../../common/storage/BehavioralDataUnitPackage.h"
#include "../../common/storage/InputDataUnitPackage.h"
#include "../../common/storage/RewardDataUnit.h"
#include "../../common/storage/PropertyDataUnit.h"
#include "../../common/storage/FrameDataUnit.h"
#include "../../common/storage/StateDataUnit.h"

#include "RemoteStateXMLHandler.h"
#include "RemoteStateUpdater.h"

#include "../../common/memleakdetect.h"

using namespace Picto;

PropertyDataUnitHandler::PropertyDataUnitHandler(RemoteStateUpdater *pParent)
{
	propUnit_ = QSharedPointer<PropertyDataUnit>(new PropertyDataUnit());
	connect(this, SIGNAL(propertyValueChanged(qulonglong, int, QString)), pParent, SIGNAL(propertyValueChanged(qulonglong, int, QString)));
	connect(this, SIGNAL(propertyInitValueChanged(qulonglong, int, QString)), pParent, SIGNAL(propertyInitValueChanged(qulonglong, int, QString)));
}

void PropertyDataUnitHandler::handle(QSharedPointer<QXmlStreamReader> xmlReader)
{
	propUnit_->fromXml(xmlReader);
	if (propUnit_->initValue_)
	{
		emit propertyInitValueChanged(propUnit_->getDataID(), propUnit_->index_, propUnit_->value_);
	}
	else
	{
		emit propertyValueChanged(propUnit_->getDataID(), propUnit_->index_, propUnit_->value_);
	}
}


BehavioralDataUnitHandler::BehavioralDataUnitHandler(RemoteStateUpdater *pParent)
{
	behavUnitPack_ = QSharedPointer<BehavioralDataUnitPackage>(new BehavioralDataUnitPackage());
	connect(this, SIGNAL(signalChanged(QString, QStringList, QVector<float>)), pParent, SIGNAL(signalChanged(QString, QStringList, QVector<float>)));
}

void BehavioralDataUnitHandler::handle(QSharedPointer<QXmlStreamReader> xmlReader)
{
	behavUnitPack_->fromXml(xmlReader);
	QVector<float> data(2);
	QSharedPointer<BehavioralDataUnit> unit = behavUnitPack_->takeLastDataPoint();
	data[0] = unit->x;
	data[1] = unit->y;
	emit signalChanged(behavUnitPack_->getChannel(),
			behavUnitPack_->getDescriptor().split(",", QString::SkipEmptyParts),
			data);
}


InputDataUnitHandler::InputDataUnitHandler(RemoteStateUpdater *pParent)
{
	inputUnitPack_ = QSharedPointer<InputDataUnitPackage>(new InputDataUnitPackage());
	connect(this, SIGNAL(signalChanged(QString, QStringList, QVector<float>)), pParent, SIGNAL(signalChanged(QString, QStringList, QVector<float>)));
}

void InputDataUnitHandler::handle(QSharedPointer<QXmlStreamReader> xmlReader)
{
	inputUnitPack_->fromXml(xmlReader);
	QVector<float> data(8);
	QSharedPointer<InputDataUnit> unit = inputUnitPack_->takeLastDataPoint();
	for (int i = 0; i < 8; i++)
	{
		data[i] = unit->input[i];
	}
	emit signalChanged(inputUnitPack_->getChannel(),
			inputUnitPack_->getDescriptor().split(",", QString::SkipEmptyParts),
			data);
}


StateDataUnitHandler::StateDataUnitHandler(RemoteStateUpdater *pParent)
{
	stateUnit_ = QSharedPointer<StateDataUnit>(new StateDataUnit());
	connect(this, SIGNAL(transitionActivated(qulonglong, int)), pParent, SIGNAL(transitionActivated(qulonglong, int)));
}

void StateDataUnitHandler::handle(QSharedPointer<QXmlStreamReader> xmlReader)
{
	stateUnit_->fromXml(xmlReader);
	qDebug() << "Received Transition ID:" << stateUnit_->getTransitionID();
	emit transitionActivated(stateUnit_->getDataID(), stateUnit_->getTransitionID());
}


FrameDataUnitHandler::FrameDataUnitHandler(RemoteStateUpdater *pParent)
{
	frameUnit_ = QSharedPointer<FrameDataUnit>(new FrameDataUnit());
	connect(this, SIGNAL(framePresented(double)), pParent, SIGNAL(framePresented(double)));
	connect(this, SIGNAL(updateCurrUnitTime(QString)), pParent, SLOT(updateCurrUnitTime(QString)));
}

void FrameDataUnitHandler::handle(QSharedPointer<QXmlStreamReader> xmlReader)
{
	frameUnit_->fromXml(xmlReader);
	//Both frame and rewards (while pause or stopped) can cause state update
	emit updateCurrUnitTime(frameUnit_->time);
	emit framePresented(frameUnit_->time.toDouble());
}


RewardDataUnitHandler::RewardDataUnitHandler(RemoteStateUpdater *pParent)
{
	rewardUnit_ = QSharedPointer<RewardDataUnit>(new RewardDataUnit());
	connect(this, SIGNAL(rewardSupplied(double, int, int)), pParent, SIGNAL(rewardSupplied(double, int, int)));
	connect(this, SIGNAL(updateCurrUnitTime(QString)), pParent, SLOT(updateCurrUnitTime(QString)));
}

void RewardDataUnitHandler::handle(QSharedPointer<QXmlStreamReader> xmlReader)
{
	rewardUnit_->fromXml(xmlReader);
	//Both frame and rewards (while pause or stopped) can cause state update
	emit updateCurrUnitTime(rewardUnit_->getTime());
	emit rewardSupplied(rewardUnit_->getTime().toDouble(), rewardUnit_->getDuration(), rewardUnit_->getChannel());
}