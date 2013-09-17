#ifndef _FileParameter_H_
#define _FileParameter_H_

#include <QByteArray>
#include "../common.h"

#include "parameter.h"

namespace Picto {

/*! \brief A File parameter.
 *
 *	This parameter holds a file.  Not the path to the file, but the actual file such that changing
 *	the original file on disk will not affect the contents of this parameter unless the file is
 *	reloaded.
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

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void fileDataUpdated(const QByteArray& data) = 0;
	QByteArray getFileData();
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
