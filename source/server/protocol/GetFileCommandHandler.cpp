#include "GetFileCommandHandler.h"

#include "../../common/globals.h"
#include "../../common/storage/StateDataUnit.h"
#include "../connections/SessionInfo.h"
#include "../connections/ConnectionManager.h"
#include "../../common/timing/Timestamper.h"

#include <QXmlStreamWriter>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QUuid>
#include <QDir>
#include "../../common/memleakdetect.h"

GetFileCommandHandler::GetFileCommandHandler()
{
	//Create files list
	QDir currentDir;
	QStringList filters;
	filters << "*.dll" << "*.exe" << "*.wav";
	currentDir.setNameFilters(filters);
	currentDir.setSorting(QDir::Name);
	currentDir.setFilter(QDir::Files | QDir::Dirs);
	files_ = currentDir.entryList();

	currentDir.setNameFilters(QStringList());
	currentDir.setSorting(QDir::Name);
	currentDir.setFilter(QDir::Files | QDir::Dirs);
	QFileInfoList fileInfoList = currentDir.entryInfoList();
	//Append all files in subdirectories
	for(int i=0;i<fileInfoList.size();i++)
	{
		if(fileInfoList[i].isDir())
		{
			QDir subDir = fileInfoList[i].absoluteDir();
			subDir.cd(fileInfoList[i].fileName());
			subDir.setNameFilters(filters);
			subDir.setSorting(QDir::Name);
			subDir.setFilter(QDir::Files);
			QStringList subFiles = subDir.entryList();
			foreach(QString subFile,subFiles)
			{
				files_.append(fileInfoList[i].fileName()+"/"+subFile);
			}
		}
	}
}

/*! \brief handles a GETDATA command
 *
 *	The data is stored in a database table appropraite to the type of data,
 *	and a response is returned indicating that the data was received.
 */
QSharedPointer<Picto::ProtocolResponse> GetFileCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//printf("GETDATA  handler: %d %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTOUPDATE","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTOUPDATE","1.0",Picto::ProtocolResponseType::NotFound));

	int fileIndex = command->getTarget().toInt();
	if(fileIndex >= files_.size())
	{
		response->setFieldValue("FileName","");
		response->setRegisteredType(Picto::RegisteredResponseType::Immediate);
		return response;
	}

	QFile* requestedFile = new QFile(files_[fileIndex]);
	requestedFile->open(QIODevice::ReadOnly);
	QByteArray fileData(requestedFile->readAll());
	delete requestedFile;
	
	QStringList fileNameParts = files_[fileIndex].split("/");
	QString directory = (fileNameParts.size()>1)?fileNameParts[0]:"";
	QString fileName = (fileNameParts.size()>1)?fileNameParts[1]:files_[fileIndex];
	response->setFieldValue("Directory",directory);
	response->setFieldValue("FileName",fileName);
	response->setFieldValue("TotalFiles",QString::number(files_.size()));
	response->setContent(fileData);
	response->setRegisteredType(Picto::RegisteredResponseType::Immediate);

	return response;

}

