#include "viewer.h"

Viewer::Viewer(QWidget* parent)
: QWidget(parent),
experiment_(0),
experimentText_(0)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}