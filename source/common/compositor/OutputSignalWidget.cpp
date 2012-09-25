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
	if(!outSigController_->hasEnabledPorts())
		setVisible(false);
	layout_ = new QHBoxLayout();
	widgets_.resize(outSigController_->numPorts());
	QVBoxLayout* subLayout;
	QLabel* portName;
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
		portName = new QLabel("Port "+QString::number(i));
		portName->setAlignment(Qt::AlignHCenter);
		subLayout->addWidget(portName);
		layout_->addLayout(subLayout);
	}
	setLayout(layout_);
	connect(outSigController_.data(),SIGNAL(portChanged(int,bool,double)),this,SLOT(outputSignalChanged(int,bool,double)));
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

void OutputSignalWidget::outputSignalChanged(int portId,bool enabled,double voltage)
{
	if(!enabled && !outSigController_->hasEnabledPorts())
		setVisible(false);

	if(portId >= widgets_.size() || portId < 0)
		return;
	widgets_[portId]->display(enabled?voltage:0.0);


	if(enable_ && !isVisible())
		setVisible(true);
}

}; //namespace Picto
