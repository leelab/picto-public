#ifndef _GETFILE_COMMAND_HANDLER_H_
#define _GETFILE_COMMAND_HANDLER_H_
#include <QStringList>
#include <QByteArray>
#include <QMutex>
#include "../../common/common.h"

#include "../../common/protocol/ProtocolCommandHandler.h"
#include "../../common/protocol/ProtocolResponse.h"

/*! \brief Handles the GETFILE commands sent by all of the Server's clients.
 *
 *	\details The GETFILE command handler works closely with the UpdateDownloader class.
 *	This handler stores a list of byte arrays each of which contains a compressed version of one
 *	of the files in or underneath the Picto Server's run directory.  Together, these files 
 *	constitute a complete Picto installation, so a client can completely update its installation
 *	to the version on the server by downloading all of these files, decompressing them, and running
 *	the appropriate executable (which is what UpdateDownloader does).  
 *
 *	GETFILE commands include an index in their target area that can be anywhere from zero to the total number 
 *	of compressed files available.  The compressed file who's index is sent is returned in the 
 *	ProtocolResponse when a GETFILE command is sent.  Other fields are included in the response 
 *	like "TotalFiles" which tells the client how many files it needs to have a complete updated Picto installation.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
struct GetFileCommandHandler : Picto::ProtocolCommandHandler
{
public:
	GetFileCommandHandler();

	QString method() { return QString("GETFILE"); }
	QSharedPointer<Picto::ProtocolResponse> processCommand(QSharedPointer<Picto::ProtocolCommand>);
private:
	static QMutex workingWithFiles_;
	static QStringList fileNames_;
	static QList<QByteArray> files_;
};


#endif
