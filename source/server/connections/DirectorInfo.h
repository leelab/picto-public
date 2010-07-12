/*!	\brief Contains info about a director connected to the server
 *
 *	The Server will likely have multiple Director instances connected
 *	at any given time.  This object holds info about the directors, 
 *	and provides a mechanism for catching timeouts.
 *
 *	This is more of a data structure than a true object, but I had
 *	to make it an actual object to deal with the timeouts.  Note that 
 *	it is a friend of the Connection Manager, so we don't have any 
 *	setter/getter functions.
 */

#ifndef _DIRECTOR_INFO_H_
#define _DIRECTOR_INFO_H_

#include <QString>

namespace DirectorStatus
{
	enum DirectorStatus
	{
		notFound, idle, stopped, paused, running
	};
}


class DirectorInfo 
{
public:
	DirectorInfo();

	//! clears the state of activity and returns it.
	bool clearActivity() {bool temp = activity_; activity_ = false; return temp; };
	void setActivity() { activity_ = true; };

	friend class ConnectionManager;

private:
	QString addressStr_;
	QString name_;
	DirectorStatus::DirectorStatus status_;

	bool activity_;
};

#endif

