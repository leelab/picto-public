#ifndef _PROTOCOLRESPONSE_H_
#define _PROTOCOLRESPONSE_H_

#include <map>

#include <QString>
#include <QByteArray>
#include <QAbstractSocket>
#include <QMap>

#include "../common.h"

namespace Picto {

namespace ProtocolResponseType
{
	typedef enum
	{
		UninitializedResponse = -1,
		OK = 200,
		BadRequest = 400, Unauthorized, NotFound = 404, IncompleteRequest = 450, UnknownRequest,
		InternalServerError = 500, NotImplemented, ConnectionTimedOut = 550
	} ProtocolResponseType;
}

namespace ContentEncodingType
{
	typedef enum
	{
		raw = 1, gzip, deflate
	} ContentEncodingType;
}

namespace MultiPartResponseType
{
	typedef enum
	{
		NotMultiPart = 0, StreamingNonMultiPart, MultiPartInitial, MultiPartPart
	} MultiPartResponseType;
}

#if defined WIN32 || defined WINCE
struct PICTOLIB_API ProtocolResponse
#else
struct ProtocolResponse
#endif
{
public:
	ProtocolResponse(
		QString serverType = "PictoServer",
		QString protocol = "PICTO",
		QString version = "1.0",
		ProtocolResponseType::ProtocolResponseType protocolResponseType = ProtocolResponseType::UninitializedResponse);

	QString generateHeadersString();
	QString getMultiPartHeaders();
	QByteArray getContent();
	QByteArray getEncodedContent();
	QByteArray getDecodedContent();
	void setContent(QByteArray _content);
	void addField(QString field, QString value);
	void setShouldTerminateConnection(bool terminateConnection);
	void setContentType(QString contentTypeString);
	void setMultiPartContentType(QString multiPartContentTypeString);
	QString getMultiPartContentType();
	void setContentEncoding(ContentEncodingType::ContentEncodingType contentEncoding);

	void setMultiPartBoundary(QString multiPartBoundaryString);
	QString getMultiPartBoundary();

	bool hasContent();
	bool shouldTerminateConnection();
	bool shouldStream();
	
	void setMultiPart(MultiPartResponseType::MultiPartResponseType multiPartState);
	MultiPartResponseType::MultiPartResponseType getMultiPart();

	void setShouldStream(bool shouldStream);

	QString getFieldValue(QString field);
	void setFieldValue(QString field, QString value);

	int getResponseCode();
	QString getResponseType();

	int read(QAbstractSocket *socket, int timeoutMs=0);
	int write(QAbstractSocket *socket);

private:
	void encodeContent();
	void decodeContent();

	void parseHeaders(QString headers);

	ProtocolResponseType::ProtocolResponseType protocolResponseCode;

	//QString serverType;
	QString protocol;
	QString version;

	QMap<QString, QString> fields;

	//QString contentType;
	QString multiPartContentType;
	QString multiPartBoundary;
	QByteArray content;
	QByteArray encodedContent;
	//ContentEncodingType::ContentEncodingType contentEncodingType;

	QMap<ProtocolResponseType::ProtocolResponseType, QString> protocolResponseTypeStrings;
	QMap<ContentEncodingType::ContentEncodingType, QString> contentEncodingTypeStrings;

	bool bShouldTerminateConnection;
	bool bStreamingResponse;
	MultiPartResponseType::MultiPartResponseType multiPartResponseState;

	int timeout;
};

}; //namespace Picto

#endif
