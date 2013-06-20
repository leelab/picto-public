#include "AnalysisVariableList.h"
#include "../storage/ObsoleteAsset.h"
#include "../memleakdetect.h"

namespace Picto {

AnalysisVariableList::AnalysisVariableList()
: AnalysisVariable()
{
	list_.clear();
	AddDefinableObjectFactory("Value",QSharedPointer<AssetFactory>(new AssetFactory(0,-1,AssetFactory::NewAssetFnPtr(ObsoleteAsset::Create))));
}

QSharedPointer<Asset> AnalysisVariableList::Create()
{
	return QSharedPointer<Asset>(new AnalysisVariableList());
}

void AnalysisVariableList::reset()
{
	AnalysisVariable::reset();
	list_.clear();
}

int AnalysisVariableList::length()
{
	return list_.length();
}

void AnalysisVariableList::append(QVariant value)
{
	list_.append(value);
}

void AnalysisVariableList::prepend(QVariant value)
{
	list_.prepend(value);
}

void AnalysisVariableList::setValue(int index,QVariant value)
{
	if(index < 0 || index >= list_.size())
		return;
	list_[index] = value;
}

void AnalysisVariableList::fromArray(QVariantList array)
{
	list_ = array;
}

void AnalysisVariableList::removeFirst()
{
	removeAt(0);
}

void AnalysisVariableList::removeLast()
{
	removeAt(list_.size()-1);
}

void AnalysisVariableList::removeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return;
	list_.removeAt(index);
}

void AnalysisVariableList::postDeserialize()
{
	AnalysisVariable::postDeserialize();
}

bool AnalysisVariableList::validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader)
{
	if(!AnalysisVariable::validateObject(xmlStreamReader))
		return false;
	return true;
}

QVariant AnalysisVariableList::getValue(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	return list_[index];
}

QVariant AnalysisVariableList::first()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.first();
}
QVariant AnalysisVariableList::last()
{
	if(list_.isEmpty())
		return QVariant();
	return list_.last();
}

QVariantList AnalysisVariableList::toArray()
{
	return list_;
}

QVariant AnalysisVariableList::takeFirst()
{
	return takeAt(0);
}

QVariant AnalysisVariableList::takeLast()
{
	return takeAt(list_.size()-1);
}

QVariant AnalysisVariableList::takeAt(int index)
{
	if(index < 0 || index >= list_.size())
		return QVariant();
	QVariant returnVal = list_.takeAt(index);
	return returnVal;
}

}; //namespace Picto
