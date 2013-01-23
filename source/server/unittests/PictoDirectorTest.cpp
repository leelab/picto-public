#include "PictoDirectorTest.h"
#include <QString>
#include "../../common/memleakdetect.h"
using namespace PictoSim;

PictoDirectorTest::PictoDirectorTest()
{
}

/*! \brief Runs a director test
 */
void PictoDirectorTest::directorTest_data()
{
	QTest::addColumn<ACTOR_PARAM>("simActorDescriptor");
	QTest::addColumn<ACTION_PARAM>("simActionDescriptor");

	QTest::newRow("0") << ACTORPARAM(SERVER,"Server" ) << ACTIONPARAM(new Server::GetDiscoveredDesc("Get Discovered by Director", 5000) );
	QTest::newRow("4") <<  ACTORPARAM(TESTBENCH) <<  ACTIONPARAM( new TestBench::SleepDesc("Wait for server startup",500));
	QTest::newRow("1") <<  ACTORPARAM(DIRECTOR,"Director0") <<  ACTIONPARAM( new Director::DiscoverServerDesc("Discover Server"));
	// Supply the server with a list of ordered commands
	SimActionDesc* desc = new Server::InterfaceDirectorDesc("Start Server Activity");
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Server::InterfaceDirectorDesc::NewSessionMsgDesc()));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Server::InterfaceDirectorDesc::LoadExpMsgDesc()));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Server::InterfaceDirectorDesc::StartMsgDesc(100)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Server::InterfaceDirectorDesc::StopMsgDesc(20000)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Server::InterfaceDirectorDesc::EndSessionMsgDesc()));
	QTest::newRow("0") <<  ACTORPARAM(SERVER,"Server" ) <<  ACTIONPARAM( desc);
	
	// The director needs to tell the server that it's here.  Setup its messages and start it
	// updating the server.  When the server starts requesting data, the simulated device will send
	// the messages at the appropriate times.
	desc = new Director::DirectorUpdateDesc("Update Server");
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::TrialMsgDesc(100,1,1)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::DataMsgDesc(110,45,15)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::DataMsgDesc(115,42,11)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::DataMsgDesc(116,38,14)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::TrialMsgDesc(400,1,1)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::TrialMsgDesc(700,2,2)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::DataMsgDesc(790,45,0)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::TrialMsgDesc(1000,2,2)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::TrialMsgDesc(1300,3,3)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::DataMsgDesc(1500,40,1)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new Director::DirectorUpdateDesc::TrialMsgDesc(1600,3,3)));
	QTest::newRow("3") <<  ACTORPARAM(DIRECTOR,"Director0") <<  ACTIONPARAM( desc );
	// Make sure all the previous commands have time to occur.
	QTest::newRow("4") <<  ACTORPARAM(TESTBENCH) <<  ACTIONPARAM( new TestBench::SleepDesc("Wait for everything to happen",5000));
	//QTest::newRow("CLOSEDIRECTOR") <<  ACTORPARAM(TESTBENCH) <<  ACTIONPARAM( new TestBench::CloseDeviceDesc("Close Director","Director0") );
	QTest::newRow("CLOSE") <<  ACTORPARAM(TESTBENCH) <<  ACTIONPARAM( new TestBench::CloseDevicesDesc("Close Devices") );


}
void PictoDirectorTest::directorTest()
{
	runExperiment();
}

/*! \brief Runs tests command by command through the PictoTestBench.
 */
void PictoDirectorTest::runExperiment()
{
	QFETCH(QSharedPointer<SimActorDesc>, simActorDescriptor);
	QFETCH(QSharedPointer<SimActionDesc>, simActionDescriptor);

	testBench_.DoAction(simActorDescriptor, simActionDescriptor);

	PictoSystemActivityReport activityReport;
	while(testBench_.PopActivityReport(activityReport))
	{
		QVERIFY2(activityReport.actualVal_ == activityReport.expectedVal_,activityReport.failMessage_.toLatin1());
	}
}
