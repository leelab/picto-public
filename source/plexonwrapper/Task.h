#ifndef TASK_H_
#define TASK_H_

#include <QObject>
#include <QSharedMemory>
#include <windows.h>
#include "plexon.h"

/*!	\brief A Task to contain the IPC Event loop
 *
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
class Task : public QObject
{
    Q_OBJECT

public:
	Task(QObject * parent = 0);
	virtual ~Task();

public slots:
	void run();

signals:
	void finished();

protected:
	void CloseClient();
	int InitClientEx3(int type, HWND hWndList, HWND hWndMain);
	int IsSortClientRunning();
	int GetTimeStampTick();
	void GetSlowInfo(int* freq, int* channels, int* gains);
	void GetGain(int* gain);
	void GetLongWaveFormStructures(int* pnmax, PL_WaveLong* waves, int* serverdropped, int* mmfdropped);
	void GetTimeStampStructures(int* pnmax, PL_Event* events);


private:
	QSharedMemory m_sharedMemory;
};

#endif