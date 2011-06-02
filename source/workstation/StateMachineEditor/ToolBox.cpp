#include <QtGui>
#include "Toolbox.h"
#include "AssetToolGroup.h"
#include "BackgroundToolGroup.h"

//! [0]
Toolbox::Toolbox(QWidget *parent) :
	QToolBox(parent)
{
	setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Ignored));
	assetTools_ = new AssetToolGroup();
	backgroundTools_ = new BackgroundToolGroup();
	addItem(assetTools_,tr("Assets"));
    addItem(backgroundTools_, tr("Backgrounds"));
	connect(assetTools_, SIGNAL(insertionItemSelected(QString)),this, SIGNAL(insertionItemSelected(QString)));
	connect(backgroundTools_, SIGNAL(insertionItemSelected(QString)),this, SIGNAL(insertionItemSelected(QString)));
	resize();
}

void Toolbox::setAsset(QSharedPointer<Asset> asset)
{
	static_cast<AssetToolGroup*>(assetTools_)->setAsset(asset);
	resize();
}
void Toolbox::resize()
{
	int assetWidth = assetTools_->sizeHint().width();
	int backgroundWidth = backgroundTools_->sizeHint().width();
	int minWidth = (assetWidth>backgroundWidth)?assetWidth:backgroundWidth;
    setMinimumWidth(minWidth);
}

QString Toolbox::getSelectedButton()
{
	QString resultVal = assetTools_->getSelectedButton();
	if(resultVal != "")
		return resultVal;
	resultVal = backgroundTools_->getSelectedButton();
	return resultVal;
}