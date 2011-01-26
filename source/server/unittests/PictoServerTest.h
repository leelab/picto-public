#include <QtTest/QtTest>
#include "PictoTestBench.h"

/*! \brief 
 *
 */
class PictoServerTest : public QObject
{
	    Q_OBJECT

public:
	PictoServerTest();

private:

	PictoSim::PictoTestBench testBench_;
private slots:
	void standardExperiments_data();
	void standardExperiments();
};
