#include "PictoServerSimulator.h"
using namespace PictoSim;
using namespace Server;

PictoServerSimulator::PictoServerSimulator(PictoSystemState* systemState, QString deviceName) : PictoDeviceSimulator(systemState, deviceName)
{}
QString PictoServerSimulator::GetDeviceTypeName()
{
	return "SERVER_SIMULATOR";
}

void PictoServerSimulator::Init()
{
}


void PictoServerSimulator::Act(QSharedPointer<SimActionDesc> actionDesc)
{}

void PictoServerSimulator::Deinit()
{
}