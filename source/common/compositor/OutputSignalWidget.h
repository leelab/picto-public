#ifndef _OUTPUTSIGNALWIDGET_H_
#define _OUTPUTSIGNALWIDGET_H_

#include <QWidget>
#include <QHBoxLayout>
#include <QVector>
#include <QLCDNumber>
#include "../iodevices/VirtualOutputSignalController.h"
#include "../common.h"
namespace Picto {

/*!	\brief Provides a visual display of Pictobox output signals
 */

#if defined WIN32 || defined WINCE
	class PICTOLIB_API OutputSignalWidget :  public QWidget
#else
class OutputSignalWidget : public QWidget
#endif
{
	Q_OBJECT
public:
	OutputSignalWidget(QSharedPointer<Picto::VirtualOutputSignalController> outSigController);
	virtual ~OutputSignalWidget();
	void enable(bool enable);

private:
	QSharedPointer<Picto::VirtualOutputSignalController> outSigController_;
	QHBoxLayout* layout_;
	QVector<QLCDNumber*> widgets_;
	bool enable_;

private slots:
	void outputSignalChanged(int pinId,bool enabled,bool high);
};


}; //namespace Picto

#endif
