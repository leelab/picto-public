#ifndef _SESSIONPLAYER_H_
#define _SESSIONPLAYER_H_
#include <QObject>
#include <QHash>

#include "SessionState.h"

namespace Picto {
/*! \brief Component of Picto Playback system that plays back sessions.
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API SessionPlayer : public QObject
#else
class SessionPlayer : public  QObject
#endif
{
	Q_OBJECT
public:
	SessionPlayer(QSharedPointer<SessionState> sessState);
	virtual ~SessionPlayer();

	bool stepForward();
	bool stepToNextFrame();
	bool stepBack();
	bool stepToPrevFrame();
	bool stepToTime(double time);

protected:

private:
	bool step(bool backward = false);
	QSharedPointer<SessionState> sessionState_;
	bool started_;
	double lastTime_;
	bool onFrame_;
	qulonglong lastIndex_;

	template <class I>	//index type
	void stepToIndex(QList<QSharedPointer<DataState<I>>> stateList,I toIndex,bool includeToIndex)
	{
		I indexForUpdate;
		int nOfLowest = 0;
		int currInd = 0;
		while(true)
		{
				indexForUpdate = toIndex;
				for(int i=0;i<stateList.size();i++)
				{
					currInd = stateList[i]->getNextIndex();
					if(currInd < indexForUpdate)
					{
						indexForUpdate = currInd;
						nOfLowest = i;
					}
				}
				if(	(includeToIndex && (indexForUpdate > toIndex)) || 
					(!includeToIndex && (indexForUpdate >= toIndex)))
					break;
				//We've got the lowest index, update the data state that has it.
				stateList[nOfLowest]->setCurrentIndex(indexForUpdate);
		}
	}

private slots:
	void sessionStateReset();

};

}; //namespace Picto
#endif