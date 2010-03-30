/*!	\Brief Keeps track of all PictoDirector instances on the network
 *
 *	The server will be talking to multiple instances of PictoDirector at
 *	any given time.  This object is used to keep track of them.  It maintains
 *	a static list of directors so that we don't need to pass around pointers.
 */

#ifndef _DIRECTOR_LIST_H_
#define _DIRECTOR_LIST_H_

#include <QHostAddress>
#include <QString>
#include <QMap>

class DirectorList
{
public:
	typedef enum
	{
		idle, running
	}DirectorStatus;

	DirectorList();

	void updateDirector(QHostAddress addr, QString name, DirectorStatus status);
	void removeDirector(QHostAddress addr);

	QString getList();

private:
	typedef struct
	{
		QHostAddress address;
		QString name;
		DirectorStatus status;
	}DirectorInfo;

	//We're using a string representation of the address as a key.
	static QMap<QString, DirectorInfo> directorMap;
};


#endif