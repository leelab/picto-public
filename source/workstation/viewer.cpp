#include "viewer.h"
#include "../common/memleakdetect.h"

Viewer::Viewer(QWidget* parent)
: QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
