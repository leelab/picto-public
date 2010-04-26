/*!	\brief A mouse signal channel that uses DirectInput
 *
 *	!!!! WARNING !!!!
 *	This code was abandoned in an incomplete state.  Do not expect it
 *	to work.
 *
 *	While testing, I realized that using the QCursor::pos() function 
 *	only works if the Director thread isn't at maximum priority (if
 *	it is at max, the mouse movement messages never get propogated).
 *	The solution to this is to write a MouseSignalChannel that directly
 *	accesses the hardware.
 *
 *	Obviously this is only for use in Win32 (non-CE) systems.  It should
 *	probably only be used for testing, but I don't think it will
 *	have any issues that would prevent its use in a real system.
 *
 *	It should be noted that much of this code was taken from Frank D. Luna's
 *	book "Introduction to 3D Game Programming with DirectX9.0c: A Shader
 *	Approach"
 */

#ifndef _DX_MOUSE_SIGNAL_CHANNEL_H
#define _DX_MOUSE_SIGNAL_CHANNEL_H

#include "../../common/common.h"
#include "../../common/engine/SignalChannel.h"

#include <QSharedPointer>
#include <QWidget>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

namespace Picto {

class DXMouseSignalChannel : public SignalChannel
{
public:
	DXMouseSignalChannel(QSharedPointer<QWidget> widget);
	~DXMouseSignalChannel() {};

	bool start();
	bool stop();
	void updateDataBuffer();

private:
	IDirectInput8*       DInput_;

	IDirectInputDevice8* mouse_;

};

};

#endif