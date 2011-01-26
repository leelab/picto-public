#include "PictoServerTest.h"
#include <QString>
using namespace PictoSim;

PictoServerTest::PictoServerTest()
{
}

void PictoServerTest::standardExperiments_data()
{
	QTest::addColumn<ACTOR_PARAM>("simActorDescriptor");
	QTest::addColumn<ACTION_PARAM>("simActionDescriptor");

	QTest::newRow("SETUP") <<  ACTORPARAM(TESTBENCH ) <<  ACTIONPARAM( new TestBench::AddDeviceUnderTestDesc("Add Server", SERVER, "SERVER") );
	QTest::newRow("0") <<  ACTORPARAM(WORKSTATION,"Workstation0" ) <<  ACTIONPARAM( new Workstation::DiscoverServerDesc("Discover Server"));
	QTest::newRow("1") <<  ACTORPARAM(DIRECTOR,"Director0") <<  ACTIONPARAM( new Director::DiscoverServerDesc("Discover Server"));
	SimActionDesc* desc = new ProxyServer::AnnounceDesc("Announce Proxy");
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::ExtEventMsgDesc(100,1)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::SpikeMsgDesc(130,0)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::SpikeMsgDesc(150,0)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::SpikeMsgDesc(390,1)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::ExtEventMsgDesc(400,1)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::ExtEventMsgDesc(700,2)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::SpikeMsgDesc(780,0)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::SpikeMsgDesc(780,1)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::ExtEventMsgDesc(1000,2)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::ExtEventMsgDesc(1300,3)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::SpikeMsgDesc(1500,5)));
	desc->AddMsg(QSharedPointer<SimMsgDesc>(new ProxyServer::AnnounceDesc::ExtEventMsgDesc(1600,3)));
	QTest::newRow("2") <<  ACTORPARAM(PROXYSERVER,"Proxy0") <<  ACTIONPARAM(desc);
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
	QTest::newRow("4") <<  ACTORPARAM(TESTBENCH) <<  ACTIONPARAM( new TestBench::SleepDesc("Wait for proxy/director updates",500));
	QTest::newRow("5") <<  ACTORPARAM(WORKSTATION,"Workstation0") <<  ACTIONPARAM( new Workstation::DirectorListDesc ("Get Director List"));
	QTest::newRow("6") <<  ACTORPARAM(WORKSTATION,"Workstation0") <<  ACTIONPARAM( new Workstation::ProxyListDesc("GetProxyList"));
	QTest::newRow("7") <<  ACTORPARAM(WORKSTATION,"Workstation0") <<  ACTIONPARAM( new Workstation::StartSessionDesc("Start Session","Director0","Proxy0"));
	QTest::newRow("8") <<  ACTORPARAM(WORKSTATION,"Workstation0") <<  ACTIONPARAM( new Workstation::TaskDesc("Start Task",Workstation::TaskDesc::START));
	QTest::newRow("9") <<  ACTORPARAM(TESTBENCH) <<  ACTIONPARAM( new TestBench::SleepDesc("Let it run a little",5000));
	//QTest::newRow("10") <<  ACTORPARAM(WORKSTATION,"Workstation0") <<  ACTIONPARAM( new Workstation::TaskDesc("Stop Task",Workstation::TaskDesc::STOP));
	//QTest::newRow("11") <<  ACTORPARAM(WORKSTATION,"Workstation0") <<  ACTIONPARAM( new Workstation::EndSessionDesc("End Session"));
	QTest::newRow("12") <<  ACTORPARAM(PROXYSERVER,"Proxy0") <<  ACTIONPARAM( new ProxyServer::DepartDesc("Close Proxy Server"));
	QTest::newRow("13") <<  ACTORPARAM(TESTBENCH) <<  ACTIONPARAM( new TestBench::WaitForDeviceDesc("Wait for ENDSESSION","Workstation0") );
	QTest::newRow("14") <<  ACTORPARAM(TESTBENCH) <<  ACTIONPARAM( new TestBench::WaitForDeviceDesc("Wait for DEPART","Proxy0") );
	QTest::newRow("CLOSE") <<  ACTORPARAM(TESTBENCH) <<  ACTIONPARAM( new TestBench::CloseDevicesDesc("Close Devices") );

}

void PictoServerTest::standardExperiments()
{
	QFETCH(QSharedPointer<SimActorDesc>, simActorDescriptor);
	QFETCH(QSharedPointer<SimActionDesc>, simActionDescriptor);

	testBench_.DoAction(simActorDescriptor, simActionDescriptor);

	PictoSystemActivityReport activityReport;
	while(testBench_.PopActivityReport(activityReport))
	{
		QVERIFY2(activityReport.actualVal_ == activityReport.expectedVal_,activityReport.failMessage_.toAscii());
	}
}

