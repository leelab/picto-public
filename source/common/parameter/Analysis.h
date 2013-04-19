#ifndef _Analysis_H_
#define _Analysis_H_

#include "../common.h"
#include "../experiment/Experiment.h"
#include "../StateMachine/ScriptableContainer.h"

namespace Picto {

/*! \brief An analysis variable parameter.
 *
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API Analysis : public ScriptableContainer
#else
class Analysis : public ScriptableContainer
#endif
{
	Q_OBJECT

public:
	Analysis();
	virtual ~Analysis(){};
	static QSharedPointer<Asset> Create();
	virtual QString assetType(){return "Analysis";};
	QUuid getAnalysisId(){return propertyContainer_->getPropertyValue("AnalysisId").toUuid();};
	QUuid getLinkedTaskId(){return propertyContainer_->getPropertyValue("LinkedTaskId").toUuid();};
	QString getLinkedTaskName(){return propertyContainer_->getPropertyValue("LinkedTaskName").toString();};
	QSharedPointer<Task> getLinkableTask(QSharedPointer<Experiment> experiment);
	bool LinkToTask(QSharedPointer<Task> task, QString& feedback);
	QSharedPointer<Task> getLinkedTask(){return linkedTask_;};

protected:
	virtual QString defaultTagName(){return "Analysis";};
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
private:
	void setLinkedTask(QSharedPointer<Task> task);
	void updateLinkedTaskProperties();
	QSharedPointer<Task> linkedTask_;
	QSharedPointer<AssetFactory> variableFactory_;
	QSharedPointer<AssetFactory> dataSourceFactory_;
	QSharedPointer<AssetFactory> outputFactory_;
	QSharedPointer<AssetFactory> functionFactory_;
	QSharedPointer<AssetFactory> scriptFactory_;

private slots:
	void taskPropertyEdited(Property* changedProp,QVariant newValue);


};


}; //namespace Picto

#endif
