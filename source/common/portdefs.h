/*! \file portdefs.h
 * \brief Ports used for %Picto applications and hardware
 */

#ifndef _PORTDEFS_H_
#define _PORTDEFS_H_

#include <QCoreApplication>
#include <QStringList>
#include "common.h"


namespace Picto {

/*! \brief Defines which port numbers will be used for %Picto applications based on the current Picto system number.
 *	\details Picto is designed such that multiple Picto Systems can operate independently on the same network so long
 *	as they have different system numbers.  This parallelism is implemented by defining different communication ports
 *	based on the system number being used.  Since communication ports are a function of system number, applications
 *	with different system numbers never recieve any communications from each other.  This class implements this
 *	system number mapping.  Once the system number is set using setSystemNumber(), that number is saved on the
 *	computer's file system and all port numbers are updated internally.  To access the correct port number for 
 *	a particular type of operation, the get*Port() functions are used.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PortNums
#else
class PortNums 
#endif
{
public:

	PortNums(QString appName,bool usedInSystemService = false);

	void setSystemNumber(QString appExePath,QStringList appExeArgs,int systemNumber,bool restart);

	int getSystemNumber();

	int getUpdatePort();
	int getMinDiscoverPort();
	int getMaxDiscoverPort();
	int getServerPort();
	int getLCDCommandPort();
	int getLCDEventPort();
private:
	void updatePortNumbers();
	QString appName_;
	int systemNumber_;

	int appUpdatePort_;
	int minDiscoverPort_;
	int maxDiscoverPort_;
	int serverPort_;
	int lcdCommandPort_;
	int lcdEventPort_;
	bool usedInSystemService_;
};

}; //namespace Picto

#endif
