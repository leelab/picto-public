#include <QBuffer>
#include <QDataStream>
#include "Task.h"
#include "PlexonWrapperDefs.h"

#include "../common/memleakdetect.h"

#define MAX_MAP_EVENTS_PER_READ 5000

HANDLE ghSemaphoreCall;
HANDLE ghSemaphoreDone;

#define LOCK_SHARED_MEMORY() \
	m_sharedMemory.lock();

#define UNLOCK_SHARED_MEMORY() \
	m_sharedMemory.unlock();

#define OPEN_READ() \
	QBuffer buffer;\
	QDataStream stream(&buffer);\
	buffer.setData((char*)m_sharedMemory.constData(), m_sharedMemory.size());\
	buffer.open(QBuffer::ReadOnly);\
	stream >> eCallType >> eCall;

#define CLOSE_READ() \
	buffer.close();

#define OPEN_WRITE(call) \
	QBuffer writeBuffer;\
	writeBuffer.open(QBuffer::ReadWrite);\
	QDataStream writeStream(&writeBuffer);\
	writeStream << RESPONSE << call;

#define CLOSE_WRITE() \
	int size = writeBuffer.size();\
	char *to = (char*)m_sharedMemory.data();\
	const char *from = writeBuffer.data().data();\
	memcpy(to, from, qMin(m_sharedMemory.size(), size));\
	writeBuffer.close();

/*! Constructs a label with very simple server information.
 */
Task::Task(QObject * parent)
	: QObject(parent), m_sharedMemory("PlexonWrapper")
{
}

Task::~Task()
{
	CloseHandle(ghSemaphoreCall);
	CloseHandle(ghSemaphoreDone);

	m_sharedMemory.detach();
}

void Task::run()
{
	printf("Plexon library wrapper program.\n\nDo not close.\n\n");

	ghSemaphoreCall = OpenSemaphore(SYNCHRONIZE|SEMAPHORE_MODIFY_STATE, 0, L"Global\\PlexonFunctionCall");
	if (ghSemaphoreCall == NULL)
	{
		printf("OpenSemaphore error: %d\n", GetLastError());
		return;
	}

	ghSemaphoreDone = OpenSemaphore(SYNCHRONIZE|SEMAPHORE_MODIFY_STATE, 0, L"Global\\PlexonFunctionDone");
	if (ghSemaphoreDone == NULL)
	{
		printf("OpenSemaphore error: %d\n", GetLastError());
		return;
	}

	//Our biggest message will be up to MAX_MAP_EVENTS_PER_READ copies of PL_WaveLong plus 5 more integers
	m_sharedMemory.attach();

	bool bQuit = false;

	while (!bQuit)
	{
		//Wait for signal that a new function call is ready
		DWORD dwWaitResult = WaitForSingleObject(ghSemaphoreCall, INFINITE);

		switch (dwWaitResult)
		{
		case WAIT_OBJECT_0:
		{
			int eCallType;
			int eCall;

			LOCK_SHARED_MEMORY();
			OPEN_READ();

			switch ((PlexonCallType)eCallType)
			{
			case CALL:
			{
#ifndef NDEBUG
				printf("Call Received.\n");
#endif
				//Correct Call Type
				break;
			}
			case NO_CALL_TYPE:
			case RESPONSE:
			case PLEXON_CALL_TYPE_MAX:
			default:
			{
				printf("Error.  Call Type: %d\n", eCallType);
				eCall = NO_CALL;
				break;
			}
			}

			switch ((PlexonFunctionCall)eCall)
			{
			case CLOSE_CLIENT:
			{
				CLOSE_READ();

				CloseClient();

				OPEN_WRITE(CLOSE_CLIENT);
				CLOSE_WRITE();

				break;
			}
			case INIT_CLIENT_EX3:
			{
				int type;
				stream >> type;
				CLOSE_READ();

				int response = 0;
				response = InitClientEx3(type, NULL, NULL);


				OPEN_WRITE(INIT_CLIENT_EX3);
				writeStream << response;
				CLOSE_WRITE();
				break;
			}
			case IS_SORT_CLIENT_RUNNING:
			{
				CLOSE_READ();

				int response = 0;
				response = IsSortClientRunning();

				OPEN_WRITE(IS_SORT_CLIENT_RUNNING);
				writeStream << response;
				CLOSE_WRITE();
				break;
			}
			case GET_TIME_STAMP_TICK:
			{
				CLOSE_READ();

				int response = 0;
				response = GetTimeStampTick();

				OPEN_WRITE(GET_TIME_STAMP_TICK);
				writeStream << response;
				CLOSE_WRITE();
				break;
			}
			case GET_SLOW_INFO:
			{
				CLOSE_READ();

				int freq = 0;
				int channels[128] = {};
				int gains[128] = {};

				GetSlowInfo(&freq, channels, gains);

				OPEN_WRITE(GET_SLOW_INFO);
				writeStream << freq;
				for (int i = 0; i < 128; i++)
				{
					writeStream << channels[i];
				}

				for (int i = 0; i < 128; i++)
				{
					writeStream << gains[i];
				}
				CLOSE_WRITE();
				break;
			}
			case GET_GAIN:
			{
				CLOSE_READ();

				int gain[128] = {};

				GetGain(gain);

				OPEN_WRITE(GET_GAIN);
				for (int i = 0; i < 128; i++)
				{
					writeStream << gain[i];
				}
				CLOSE_WRITE();
				break;
			}
			case GET_LONG_WAVE_FORM_STRUCTURES:
			{
				int pnmax = 0;
				stream >> pnmax;
				CLOSE_READ();

				PL_WaveLong *pServerEventBuffer = (PL_WaveLong*)malloc(sizeof(PL_WaveLong)*MAX_MAP_EVENTS_PER_READ);
				int serverdropped = 0;
				int mmfdropped = 0;

				GetLongWaveFormStructures(&pnmax, pServerEventBuffer, &serverdropped, &mmfdropped);

				OPEN_WRITE(GET_LONG_WAVE_FORM_STRUCTURES);
				writeStream << pnmax;
				writeStream.writeBytes((char*)pServerEventBuffer, sizeof(PL_WaveLong)*MAX_MAP_EVENTS_PER_READ);
				writeStream << serverdropped << mmfdropped;
				CLOSE_WRITE();

				free(pServerEventBuffer);
				break;
			}
			case GET_TIME_STAMP_STRUCTURES:
			{
				int pnmax = 0;
				stream >> pnmax;
				CLOSE_READ();

				PL_Event* pServerSkipDataBuffer = (PL_Event*)malloc(sizeof(PL_Event)*MAX_MAP_EVENTS_PER_READ);

				GetTimeStampStructures(&pnmax, pServerSkipDataBuffer);

				OPEN_WRITE(GET_TIME_STAMP_STRUCTURES);
				writeStream << pnmax;
				writeStream.writeBytes((char*)pServerSkipDataBuffer, sizeof(PL_Event)*MAX_MAP_EVENTS_PER_READ);
				CLOSE_WRITE();

				free(pServerSkipDataBuffer);
				break;
			}
			case QUIT:
			{
				CLOSE_READ();

				OPEN_WRITE(QUIT);
				CLOSE_WRITE();

				bQuit = true;
				break;
			}
			case PLEXON_CALL_MAX:
			{
				CLOSE_READ();
				printf("Max.\n");
				break;
			}
			case NO_CALL:
			{
				CLOSE_READ();
				printf("No Call.\n");
				break;
			}
			default:
			{
				CLOSE_READ();
				printf("Default.\n");
				break;
			}
			}

			UNLOCK_SHARED_MEMORY();

			if (!ReleaseSemaphore(ghSemaphoreDone, 1, NULL))
			{
				printf("ReleaseSemaphore error: %d\n", GetLastError());
				return;
			}

			break;
		}
		case WAIT_ABANDONED:
			printf("Wait timed out.\n");
			break;
		}

	}

	emit finished();
}

void Task::CloseClient()
{
	PL_CloseClient();
}

int Task::InitClientEx3(int type, HWND hWndList, HWND hWndMain)
{
	return PL_InitClientEx3(type, hWndList, hWndMain);
}

int Task::IsSortClientRunning()
{
	return PL_IsSortClientRunning();
}

int Task::GetTimeStampTick()
{
	return PL_GetTimeStampTick();
}

void Task::GetSlowInfo(int* freq, int* channels, int* gains)
{
	PL_GetSlowInfo(freq, channels, gains);
}

void Task::GetGain(int* gain)
{
	PL_GetGain(gain);
}

void Task::GetLongWaveFormStructures(int* pnmax, PL_WaveLong* waves, int* serverdropped, int* mmfdropped)
{
	PL_GetLongWaveFormStructures(pnmax, waves, serverdropped, mmfdropped);
}

void Task::GetTimeStampStructures(int* pnmax, PL_Event* events)
{
	PL_GetTimeStampStructures(pnmax, events);
}
