/*! \file portdefs.h
 * \ingroup picto
 * \brief Ports used for %Picto applications and hardware
 */

#ifndef _PORTDEFS_H_
#define _PORTDEFS_H_

#include <QCoreApplication>
#include <QStringList>
#include "common.h"


namespace Picto {

/*! \brief Exposes the port numbers used for %Picto applications and hardware
 * \ingroup picto
 */
#if defined WIN32 || defined WINCE
class PICTOLIB_API PortNums
#else
class PortNums 
#endif
{
public:

	PortNums(QString appName,bool usedInSystemService = false);

	//If the system number changes, you will almost definitely need to restart the application.  
	//Use the restart input to do this automatically.
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
