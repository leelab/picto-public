#ifndef EXPERIMENTDESIGNER_H
#define EXPERIMENTDESIGNER_H

#include "Designer.h"
#include "../PictoData.h"
using namespace Picto;


class ExperimentDesigner : public Designer
{
    Q_OBJECT

public:
	ExperimentDesigner(QWidget *parent=0);
	virtual ~ExperimentDesigner();

protected:
	virtual void setOpenAsset(int assetId);
	virtual QSharedPointer<DataStore> createTopLevelAsset();
private:
	QSharedPointer<PictoData> topAsset_;
};

#endif
