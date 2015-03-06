#ifndef _ImageFileParameter_H_
#define _ImageFileParameter_H_

#include <QImage>
#include "../common.h"

#include "FileParameter.h"

namespace Picto {

/*! \brief An Image File parameter.
 *
 *	\details This parameter holds an image file.  Not the path to the file, but the actual file such that changing
 *	the original file on disk will not affect the contents of this parameter unless the file is
 *	reloaded.  The class checks all loaded files to make sure that they have formats that Picto
 *	understands (it understands the formats that QImage understands).
 *
 *	This class works with the ImageGraphic class such that multiple ImageGraphic objects can use the same
 *	ImageFileParameter.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
 */
#if defined WIN32 || defined WINCE
	class PICTOLIB_API ImageFileParameter : public FileParameter
#else
class ImageFileParameter : public FileParameter
#endif
{
	Q_OBJECT

public:
	ImageFileParameter();
	virtual ~ImageFileParameter(){};

	static QSharedPointer<Asset> Create();

	virtual QString getUITemplate(){return "ImageFileParameter";};
	virtual QString friendlyTypeName(){return "Image File";};

protected:
	virtual void postDeserialize();
	virtual bool validateObject(QSharedPointer<QXmlStreamReader> xmlStreamReader);
	virtual void fileDataUpdated(const QByteArray& data);
private:
	QImage image_;
};


}; //namespace Picto

#endif
