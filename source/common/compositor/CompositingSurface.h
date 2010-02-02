#ifndef _COMPOSITINGSURFACE_H_
#define _COMPOSITINGSURFACE_H_

#include "../common.h"

#include <QImage>

namespace Picto {

struct PICTOLIB_CLASS CompositingSurface
{
public:
	CompositingSurface();
	virtual ~CompositingSurface();

	virtual void convertImage(QImage image) = 0;
	virtual QString getTypeName() = 0;
};


}; //namespace Picto

#endif
