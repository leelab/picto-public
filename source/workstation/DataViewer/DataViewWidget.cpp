#include <QPalette>
#include <QLabel>
#include "DataViewWidget.h"
#include "../../common/memleakdetect.h"

using namespace Picto;

/*! \brief Constructs a new DataViewWidget with the specified name and containing the passed Widget
*/
DataViewWidget::DataViewWidget(const QString cqsName, QWidget *pqwWidget)
	: enable_(false)
{
	layout_ = new QVBoxLayout();
	layout_->addWidget(new QLabel(cqsName));
	layout_->addWidget(pqwWidget);

	setLayout(layout_);
}

/*! \brief DataViewWidget destructor
*/
DataViewWidget::~DataViewWidget()
{

}

/*! \brief Enables/Disables this widget.
 * \details If the widget is disabled, it will be invisible.
 */
void DataViewWidget::enable(bool enable)
{
	enable_ = enable;
	if(!enable_)
		setVisible(false);
}

