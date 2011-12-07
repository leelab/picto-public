#include <QUdpSocket>
#include <QLabel>

/*!	\brief A label that displays the address of a PictoServer
 *
 *	This was really just a test piece to confirm that the ServerDiscoverer was
 *	working.  You would probably kill it if you were writing some sort of 
 *	configuration app.
 */
class TestLabel : public QLabel
{
    Q_OBJECT

public:
	TestLabel(QWidget * parent = 0);
	virtual ~TestLabel();
};
