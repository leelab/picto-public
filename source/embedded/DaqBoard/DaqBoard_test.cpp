// DaqBoard.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DaqBoard.h"

#define AI_TEST

int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{
    _tprintf(_T("Hello World!\n"));

	DaqBoard *myBoard = new DaqBoard;
	DaqBoard *myBoard2 = new DaqBoard;

#ifdef REWARD_TEST
	unsigned int rewardDurations[4] = {3000,3000,3000,3000};
	myBoard->initRewardController(4,rewardDurations);
	for(int controller = 1; controller<=4; controller++)
	{
		myBoard->giveReward(controller);
	}

	printf("Press enter to flush third controller\n");
	getchar();
	myBoard->flushReward(3,true);
	printf("Press enter to stop flushing 3rd controller\n");
	getchar();
	myBoard->flushReward(3,false);
#endif

#ifdef AI_TEST
	int valuesRead1,valuesRead2;

	myBoard->CreateAIChannel(0,myBoard->kChanTypeRSE);
	myBoard->CreateAIChannel(1,myBoard->kChanTypeRSE);
	myBoard->StartAI(500,true);
	
	Sleep(16);
	valuesRead1 = myBoard->CollectAIData(100);
	Sleep(1000);
	Sleep(16);
	valuesRead2 = myBoard2->CollectAIData(100);

	DaqBoard::AIChannel *myChan;

	printf("Channel 0\n");
	myChan = myBoard->GetAIChannel(0);
	for(unsigned int i=0; i<myChan->voltages.size(); i++)
		printf("%d: %f V\n", i, myChan->voltages[i]);
	
	printf("Channel 1\n");
	myChan = myBoard->GetAIChannel(1);
	for(unsigned int i=0; i<myChan->voltages.size(); i++)
		printf("%d: %f V\n", i, myChan->voltages[i]);

	printf("valuesRead1 = %d\n", valuesRead1);
	printf("valuesRead2 = %d\n", valuesRead2);

#endif

    return 0;
}

