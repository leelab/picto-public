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

namespace RegisteredResponseType
{
	typedef enum
	{
		NotRegistered = 0x0,			//This type of response doesn't include the command id of the command that it's responding to.  The command id from this type is appended to an unconfirmed command id list
		Immediate = 0x1,				//This type of response includes the command id of all commands in any list that preceded it.  It is used when all activities that needed to occur on the any previous responses all occured within the same thread in which they arrived.
		FirstInCommandPackage = 0x2,	//This type of response indicates that the current command is the first in a new unconfirmed command id list.  From this point on, new commands should go in this new list.
										//If this type of response is detected while there is already a closed unconfirmed command id list, that list is erased and replaced by the latest list.
		SendLastCommandPackage = 0x4,	//This type of response indicates that the current command should include a comma separated list of commands in the latest complete unconfirmed command list.
										//It is used when the thread handling the activities of the commands in LastInCommandPackage is complete.
		FirstInPackAndSendLast = 0x6
	} RegisteredResponseType;
}

/*!	\brief A response, issued as the result of a command
 *
 *	Protocol responses are the obvious object that gets returned after a command 
 *	is received.  The response can simply be an empty 200:OK, in response to a 
 *	simple command, or they can be much more complex.  Although there is functionality
 *	in place for streaming responses, I never used it.  (However, it might be wise
 *	to stream some of the responses for the repetitve commands.)
 */
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

	void setRegisteredType(RegisteredResponseType::RegisteredResponseType type);
	RegisteredResponseType::RegisteredResponseType getRegisteredType();

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
	RegisteredResponseType::RegisteredResponseType registeredType_;
	MultiPartResponseType::MultiPartResponseType multiPartResponseState;

	int timeout;
};

}; //namespace Picto

#endif
