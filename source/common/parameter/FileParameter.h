#ifndef _FileParameter_H_
#define _FileParameter_H_

#include <QByteArray>
#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A base class for File Parameters that store File contents.
 *
 *	This parameter holds a file.  It also holds the path to the file.  Don't be fooled though, this is storing
 *	the actual file contents such that changing the original file on disk will not affect the contents of this parameter unless 
 *	the file path is reset.  
 *
 *	Whenever the FileName Property changes, this class automatically reads the contents of the file at the FileName
 *	path and stores them in a special XML friendly Base64 format in the Design file.  This way no matter where the
 *	Design.xml goes, the File will come with it, there will be no need to make sure that file resources are at proper
 *	paths to get the design to run.
 *	\author Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2013
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API FileParameter : public Parameter
#else
class FileParameter : public Parameter
#endif
{
	Q_OBJECT

public:
	FileParameter();
	virtual ~FileParameter(){};


	virtual QString getUITemplate(){return "FileParameter";};
	virtual QString friendlyTypeName(){return "File";};
	
	QByteArray getFileData();
protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	/*! \brief Called whenever the QByteArray accessible from getFileData() changes.
	 *	\details Performs whatever operations are necessary for the switch to the new
	 *	file data.
	 */
	virtual void fileDataUpdated(const QByteArray& data) = 0;
private:
	void loadDataFromPropery();
	bool updatingFile_;
	QByteArray fileData_;
private slots:
	void filePathChanged(Property*,QVariant);
	void fileDataChanged(Property*,QVariant);
};


}; //namespace Picto

#endif
