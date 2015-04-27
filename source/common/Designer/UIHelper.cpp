#include <QSplitter>
#include <QBoxLayout>
#include <QFrame>

#include "UIHelper.h"
#include "../memleakdetect.h"
namespace Picto {
namespace UIHelper {

/*! \brief Adds a line widget to the passed-in QSplitterHandle to aid in using the splitter
*/
void addSplitterLine(QSplitterHandle *splitterHandle, int margin)
{
	QBoxLayout *handleLayout = nullptr;
	QFrame::Shape lineType = QFrame::HLine;
	int hMargin, vMargin;
	int foreSpace;

	switch (splitterHandle->orientation())
	{
	case Qt::Horizontal:
		handleLayout = new QHBoxLayout(splitterHandle);
		lineType = QFrame::VLine;
		hMargin = 0;
		vMargin = margin;
		foreSpace = 1;
		break;
	case Qt::Vertical:
		handleLayout = new QVBoxLayout(splitterHandle);
		lineType = QFrame::HLine;
		hMargin = margin;
		vMargin = 0;
		foreSpace = 2;
		break;
	default:
		Q_ASSERT(false);
		return;
	}

	handleLayout->setContentsMargins(hMargin, vMargin, hMargin, vMargin);
	QFrame *handleLine = new QFrame(splitterHandle);
	handleLine->setFrameShape(lineType);
	handleLine->setFrameShadow(QFrame::Raised);
	handleLayout->addSpacing(foreSpace);
	handleLayout->addWidget(handleLine);
}
}; //namespace UIHelper
}; //namespace Picto