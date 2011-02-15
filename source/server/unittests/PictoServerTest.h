#include <QtTest/QtTest>
#include "PictoTestBench.h"

/*! \brief Runs various tests on the server's network interface using the PictoTestBench
 *
 */
class PictoServerTest : public QObject
{
	    Q_OBJECT

public:
	PictoServerTest();

private:

	PictoSim::PictoTestBench testBench_;
	void runExperiment();
private slots:

	void standardExperiments_data();
	void standardExperiments();
};