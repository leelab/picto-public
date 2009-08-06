#include <QUdpSocket>
#include <QLabel>

class TestLabel : public QLabel
{
    Q_OBJECT

public:
	TestLabel(QWidget * parent = 0);
	~TestLabel();
};
