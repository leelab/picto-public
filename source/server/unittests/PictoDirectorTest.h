#include <QtTest/QtTest>
#include "PictoTestBench.h"

/*! \brief Runs various tests on the server's network interface using the PictoTestBench
 *
 */
class PictoDirectorTest : public QObject
{
	    Q_OBJECT

public:
	PictoDirectorTest();

private:

	PictoSim::PictoTestBench testBench_;
	void runExperiment();
private slots:

	void directorTest_data();
	void directorTest();
};
