#ifndef _RESULTCONTAINER_H
#define _RESULTCONTAINER_H
#include <QString>
#include <QSharedPointer>
#include <QMap>

#include "../common.h"
#include "../statemachine/ScriptableContainer.h"
#include "Transition.h"
#include "Result.h"
namespace Picto {

/*!	\brief An ancestor class for DataStores's that container results
 *
 *
 */

#if defined WIN32 || defined WINCE
class PICTOLIB_API ResultContainer : public ScriptableContainer
#else
class ResultContainer : public ScriptableContainer
#endif
{
	Q_OBJECT
public:
	ResultContainer();
	virtual ~ResultContainer(){};

	QStringList getResultList();
	QSharedPointer<Result> getResult(QString name);
	virtual bool hasEditableDescendants();

protected:
	/*! \brief Defines a result that is required to be in this resultContainer.
	 *	This should be called in a child constructor.  If a result with this results name
	 *	isn't serialized in, it will be added automatically.  In validate object, the
	 *	result container will verify that the required results have been defined and no
	 *	result type limits have been exceeeded.
	 */
	void addRequiredResult(QString resultName);
	//Same as above except that the actual result object pointer is passed in.
	//checking for a deserialized copy is still by result name.
	//void addRequiredResult(QSharedPointer<Result> requiredResult, QString type = "");
	void setMaxOptionalResults(int max, QString type = "");
	/*! \brief This is used to add a result factory type.
	 * If, for example, an object can have regular results and ControlTargetResults,
	 * this function would be used to add a "target" type such that <Result type="Target">
	 * tags would get their results from the input factory.
	 */
	void defineResultFactoryType(QString type,QSharedPointer<AssetFactory> resultFactory);
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	QMap<QString, QSharedPointer<Result> > results_;

private:
	QSharedPointer<AssetFactory> resultFactory_;
	QMap<QString,QSharedPointer<AssetFactory>> resultFactoryByType_;
	QMap<QString,int> maxOptionalResults_;
	QMultiMap<QString,QSharedPointer<Result> > requiredResults_;
//private slots:
//	void updateResultsFromChildren();

};


}; //namespace Picto

#endif
