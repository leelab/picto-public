#include <QPalette>
#include <QVBoxLayout>
#include <QLabel>
#include "OutputSignalWidget.h"
#include "../memleakdetect.h"

namespace Picto {

/*! \brief Constructs a new OutputSignalWidget that is controlled by the input VirtualOutputSignalController
*/
OutputSignalWidget::OutputSignalWidget(QSharedPointer<Picto::VirtualOutputSignalController> outSigController)
:
enable_(false)
{
	outSigController_ = outSigController;
	//Only show the OutputSignalWidget if there are some enabled pins in the outSigController_
	if(!outSigController_->hasEnabledPins())
		setVisible(false);
	layout_ = new QHBoxLayout();
	layout_->addWidget(new QLabel(QString("%1:").arg(outSigController_->getPort())));
	//Create one digital display for each pin controlled by the outSigController_ and line
	//them up in a row.
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
/*! \brief Enables/Disables this widget.
 * \details If the widget is disabled, it will be invisible, if it is enabled, it will only be
 * visible if the VirtualOutputSignalController has enabled pins.
 */
void OutputSignalWidget::enable(bool enable)
{
	enable_ = enable;
	if(!enable_)
		setVisible(false);
}
/*! \brief Sets the value of the digital display representing the input pinId to the enabled/high value.
 * \details If there are no enabled pins in the port, this widget will be invisible.  If there are enabled
 *	pins, then all pins that are not enabled or not set high will show a zero.  Enabled, high pins will 
 *	show 5 for a TTL 5 Volts.
 *	\note This function is called whenever something about a pin on the attached VirtualOutputSignalController
 *	changes.
 */
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
