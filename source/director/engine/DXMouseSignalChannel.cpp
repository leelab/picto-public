#include "DXMouseSignalChannel.h"

#include "../../common/timing/Timestamper.h"
#include <QApplication>

namespace Picto {

/*! \brief Consturcotr requires a pointer to the top level widget (usually the visual target) 
 */
DXMouseSignalChannel::DXMouseSignalChannel(QSharedPointer<QWidget> widget)
	: SignalChannel(0)
{
	//add our subchannels to the list
	addSubchannel("xpos");
	addSubchannel("ypos");
	addSubchannel("time");

	memset(&mouseState_,0,sizeof(mouseState_));

	DirectInput8Create(qWinAppInst(), DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (void**)&DInput_, 0);

	DInput_->CreateDevice(GUID_SysMouse, &mouse_, 0);
	mouse_->SetDataFormat(&c_dfDIMouse2);
	mouse_->SetCooperativeLevel((HWND)widget->winId(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	mouse_->Acquire();

}

bool DXMouseSignalChannel::start()
{
	return true;
}

bool DXMouseSignalChannel::stop()
{
	return true;
}

//In this case, we are constantly polling the mouse position and 
//updating the data buffer, so update data buffer doesn't need to
//do anything.  However, if the timers aren't working (for example if
//we're running without an event loop), we'll call pollMouse anyway).
void DXMouseSignalChannel::updateDataBuffer()
{
	//Poll the mouse
	HRESULT hr
	DIMOUSESTATE2        mouseState;

	hr = mouse_->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&mouseState); 
	if(hr < 0)
	{
		// Mouse lost, zero out mouse data structure.
		memset(&mMouseState, 0, sizeof(mMouseState));

		// Try to acquire for next time we poll.
		mouse_->Acquire(); 
	}

	Timestamper stamper;

	rawDataBuffer_["xpos"].append(0.0);
	rawDataBuffer_["ypos"].append(0.0);
	rawDataBuffer_["time"].append(stamper.stampSec());
}

};