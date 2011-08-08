#include "viewer.h"

Viewer::Viewer(QWidget* parent)
: QWidget(parent),
pictoData_(0),
pictoDataText_(0)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
