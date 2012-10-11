#include <QPalette>
#include <QVBoxLayout>
#include <QLabel>
#include "OutputSignalWidget.h"
#include "../memleakdetect.h"

namespace Picto {

OutputSignalWidget::OutputSignalWidget(QSharedPointer<Picto::VirtualOutputSignalController> outSigController)
:
enable_(false)
{
	outSigController_ = outSigController;
	if(!outSigController_->hasEnabledPins())
		setVisible(false);
	layout_ = new QHBoxLayout();
	layout_->addWidget(new QLabel(QString("%1:").arg(outSigController_->getPort())));
	widgets_.resize(outSigController_->numPins());
	QVBoxLayout* subLayout;
	QLabel* pinName;
	for(int i=0;i<widgets_.size();i++)
	{
		subLayout = new QVBoxLayout();
		widgets_[i] = new QLCDNumber(1);
		QPalette palette = widgets_[i]->palette();
		palette.setColor(QPalette::Normal, QPalette::Light, Qt::blue);
		palette.setColor(QPalette::Normal, QPalette::Dark, Qt::darkBlue);
		widgets_[i]->setPalette(palette);
		widgets_[i]->resize(widgets_[i]->size());	//Causes color changes to show up properly
		subLayout->addWidget(widgets_[i]);
		pinName = new QLabel("Pin "+QString::number(i));
		pinName->setAlignment(Qt::AlignHCenter);
		subLayout->addWidget(pinName);
		layout_->addLayout(subLayout);
	}
	setLayout(layout_);
	connect(outSigController_.data(),SIGNAL(pinChanged(int,bool,bool)),this,SLOT(outputSignalChanged(int,bool,bool)));
}

OutputSignalWidget::~OutputSignalWidget()
{

}

void OutputSignalWidget::enable(bool enable)
{
	enable_ = enable;
	if(!enable_)
		setVisible(false);
}

void OutputSignalWidget::outputSignalChanged(int pinId,bool enabled,bool high)
{
	if(!enabled && !outSigController_->hasEnabledPins())
		setVisible(false);

	if(pinId >= widgets_.size() || pinId < 0)
		return;
	widgets_[pinId]->display(enabled?double(high)*5.0:0.0);


	if(enable_ && !isVisible())
		setVisible(true);
}

}; //namespace Picto
