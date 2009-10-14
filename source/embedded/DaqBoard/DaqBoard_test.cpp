//! A simple test program for the DaqBoard class

#include "stdafx.h"
#include "DaqBoard.h"
#include <windows.h>

#define FULL_TEST

#define FRAME_PERIOD 16  //length of a frame in ms

int _tmain(int argc, TCHAR *argv[], TCHAR *envp[])
{
    _tprintf(_T("Hello World!\n"));

	DaqBoard *myBoard = new DaqBoard;

#ifdef REWARD_TEST
	unsigned int rewardDurations[4] = {200,200,200,200};
	if(!(myBoard->initRewardController(4,rewardDurations)))
		printf("intiRewardController failed!\n");

	printf("Reward controller initialized\n");
	getchar();

	while(1)
	{
		printf("Press enter to give a reward\n");
		getchar();
		if(!(myBoard->giveReward(1)))
			printf("giveReward(1) failed\n!");

	}
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

#ifdef FULL_TEST
	/* 
	 *  The full test is designed to be run with both a solenoid for 
	 *  the reward portion, and some sort of voltage source for the AI input
	 *  I started off using the "eye tracker simulator", but you could use a 
	 *  joystick, or even a real eyetracker (assuming you had access to a
	 *  willing eyeball).
	 *
	 *  The test involves taking inputs from the joystick/eye tracker, and providing
	 *  rewards based on those inputs.  Initially I was going to use the Konami code
	 *  to provide a reward, but since the eye tracker simulator only works in the down 
	 *  and left direction, I'll give a different number of rewards based on the dirction
	 *  selected.
	 */

	int loopCounter=0;
	
	//Set up single reward controller
	unsigned int rewardDurations[1] = {200};
	if(!(myBoard->initRewardController(1,rewardDurations)))
		printf("initRewardController failed!\n");
	printf("Reward controller initialized\n");

	//set up the analog input
	int valuesRead;
	DaqBoard::AIChannel *xCoord, *yCoord;

	myBoard->CreateAIChannel(0,myBoard->kChanTypeDifferential);
	myBoard->CreateAIChannel(1,myBoard->kChanTypeDifferential);

	//start the AI running at 500 Hz
	myBoard->StartAI(500,true);

	//set up a clock
	LARGE_INTEGER ticksPerMs;
	LARGE_INTEGER currTicks,endTicks;
	QueryPerformanceFrequency(&ticksPerMs);  //this is actually ticks per sec
	ticksPerMs.QuadPart /= 1000;			//get ticks per ms
		

	//loop at the frame rate
	while(loopCounter<500)
	{
		// Get current ticks
		QueryPerformanceCounter(&currTicks);		
		// Create timeout value
		endTicks.QuadPart = currTicks.QuadPart + ticksPerMs.QuadPart*FRAME_PERIOD;


		//dump the data from the board
		valuesRead = myBoard->CollectAIData(100);

		//grab the data from memory
		xCoord = myBoard->GetAIChannel(0);
		yCoord = myBoard->GetAIChannel(1);

		//print the coordinates (as voltages)
		//printf("\n Values read: %d\n", valuesRead);
		//for(unsigned int i=0; i < valuesRead; i++)
		for(unsigned int i=0; i < 1 && valuesRead>0; i++)
		{
			//check for left
			if(xCoord->voltages[i]<1.5 && yCoord->voltages[i]>3.0 && yCoord->voltages[i]<4.0)
			{
				myBoard->giveReward(1);
				printf("left\n");
				myBoard->FlushAIData();
			}
			//check for right
			else if(xCoord->voltages[i]>5.5 && yCoord->voltages[i]>3.0 && yCoord->voltages[i]<4.0)
			{
				myBoard->giveReward(1);
				Sleep(100);
				myBoard->giveReward(1);
				printf("right\n");
				myBoard->FlushAIData();
			}
			//check for down
			else if(yCoord->voltages[i]<1.5 && xCoord->voltages[i]>3.0 && xCoord->voltages[i]<4.0)
			{
				myBoard->giveReward(1);
				Sleep(100);
				myBoard->giveReward(1);
				Sleep(100);
				myBoard->giveReward(1);
				printf("down\n");
				myBoard->FlushAIData();
				break;
			}
			//check for up
			else if(yCoord->voltages[i]>5.5 && xCoord->voltages[i]>3.0 && xCoord->voltages[i]<4.0)
			{
				myBoard->giveReward(1);
				Sleep(100);
				myBoard->giveReward(1);
				Sleep(100);
				myBoard->giveReward(1);
				Sleep(100);
				myBoard->giveReward(1);
				printf("up\n");
				myBoard->FlushAIData();
				break;
			}
			
			//printf("%f\n",xCoord->voltages[i]);

		}

		//kill time until a frame period has passed
		do
		{
			// Get current ticks
			QueryPerformanceCounter(&currTicks);
			// Delay until timeout
		} while (currTicks.QuadPart<endTicks.QuadPart);

		loopCounter++;
	}
	



#endif

    return 0;
}

