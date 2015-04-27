#ifndef _UI_HELPER_H_
#define _UI_HELPER_H_

#include <Qt>
#include "../common.h"

QT_BEGIN_NAMESPACE
class QSplitterHandle;
QT_END_NAMESPACE

namespace Picto
{
	//! A namespace to hold some global UI-building functions
	namespace UIHelper
	{
		PICTOLIB_GLOBAL void addSplitterLine(QSplitterHandle *splitterHandle, int margin);
	};
};

#endif
