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
#include <QMutexLocker>
#include "../../common/memleakdetect.h"

QMutex GetFileCommandHandler::workingWithFiles_;
QStringList GetFileCommandHandler::fileNames_;
QList<QByteArray> GetFileCommandHandler::files_;

/*! \brief Constructs a GetFileCommandHandler.
 *	\details When the GetFileCommandHandler is constructed, all files in the run directory are compressed and added to a list.
 *	When GETFILE commands come in, these files can be downloaded.
 */
GetFileCommandHandler::GetFileCommandHandler()
{
	QMutexLocker locker(&workingWithFiles_);
	//If the files lists have not yet been initiated, do that now.
	if(files_.size())
		return;

	//Create files list
	QDir currentDir;
	QStringList filters;
	filters << "*.dll" << "*.exe" << "*.wav";
	currentDir.setNameFilters(filters);
	currentDir.setSorting(QDir::Name);
	currentDir.setFilter(QDir::Files | QDir::Dirs);
	fileNames_ = currentDir.entryList();

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
				//Add filename to list
				fileNames_.append(fileInfoList[i].fileName()+"/"+subFile);
			}
		}
	}

	//Create cached list of compressed files in memory for quick updating of connected machines
	//Go through all filenames
	foreach(QString fileName,fileNames_)
	{
		//Get file with that name and open it
		QFile* requestedFile = new QFile(fileName);
		requestedFile->open(QIODevice::ReadOnly);
		//Compress the file and add data to the list
		files_.append(qCompress(requestedFile->readAll(),9));
		//Close file handle
		delete requestedFile;
	}
}

/*! \brief Parses the input ProtocolCommand to check its syntax, responds with a ProtocolResponse including the compressed file with the index that
 *	was sent as the command target in the Response content area.
 *	\details The filename, its directory, and the total number of files available (totalFiles) are also sent as fields in the response.
 */
QSharedPointer<Picto::ProtocolResponse> GetFileCommandHandler::processCommand(QSharedPointer<Picto::ProtocolCommand> command)
{
	//printf("GETDATA  handler: %d %d\n", QThread::currentThreadId());

	QSharedPointer<Picto::ProtocolResponse> response(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTOUPDATE","1.0",Picto::ProtocolResponseType::OK));
	QSharedPointer<Picto::ProtocolResponse> notFoundResponse(new Picto::ProtocolResponse(Picto::Names->serverAppName, "PICTOUPDATE","1.0",Picto::ProtocolResponseType::NotFound));

	QMutexLocker locker(&workingWithFiles_);
	int fileIndex = command->getTarget().toInt();
	if(fileIndex >= fileNames_.size())
	{
		response->setFieldValue("FileName","");
		response->setRegisteredType(Picto::RegisteredResponseType::Immediate);
		return response;
	}

	//QFile* requestedFile = new QFile(fileNames_[fileIndex]);
	//requestedFile->open(QIODevice::ReadOnly);
	//QByteArray fileData(requestedFile->readAll());
	//delete requestedFile;
	
	QStringList fileNameParts = fileNames_[fileIndex].split("/");
	QString directory = (fileNameParts.size()>1)?fileNameParts[0]:"";
	QString fileName = (fileNameParts.size()>1)?fileNameParts[1]:fileNames_[fileIndex];
	response->setFieldValue("Directory",directory);
	response->setFieldValue("FileName",fileName);
	response->setFieldValue("TotalFiles",QString::number(fileNames_.size()));
	response->setContent(files_[fileIndex]);
	response->setRegisteredType(Picto::RegisteredResponseType::Immediate);

	return response;

}

