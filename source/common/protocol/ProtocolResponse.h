#ifndef _PROTOCOLRESPONSE_H_
#define _PROTOCOLRESPONSE_H_

#include <map>

#include <QString>
#include <QByteArray>
#include <QAbstractSocket>
#include <QMap>

#include "../common.h"

namespace Picto {

/*! \brief The types of ProtocolResponses that are supported.
 */
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

/*! \brief The types of content encoding that can be used for ProtocolResponses.
 *	\note Encoding is no longer supported.
 */
namespace ContentEncodingType
{
	typedef enum
	{
		raw = 1, gzip, deflate
	} ContentEncodingType;
}

/*! \brief The type of a particular multipart ProtocolResponse.
 *	\note Multipart responses are not currently used in Picto even though they are supported here.
*/
namespace MultiPartResponseType
{
	typedef enum
	{
		NotMultiPart = 0, StreamingNonMultiPart, MultiPartInitial, MultiPartPart
	} MultiPartResponseType;
}

/*! \brief The registration type of the current Picto response.
 *	\details The Picto server tells its client whenever data from a "registered command" that was received has been written to disk.  Whenever a ProtocolResponse
 *	is sent, the command id of the command to which it is responding is added to a list.  The RegisteredResponseType on a ProtocolResponse is used to 
 *	piggyback information about whether the data from the commands with command-ids in that list has been written to disk.  When that has happened that list
 *	of command-ids is attached to the ProtocolResponse to tell the client which data was written to disk.
 *	\note RegisteredResponseType on ProtocolResponses is complicated by the fact that commands/responses live in one thread while flushing to disk
 *	occurs in another thread.  That is the source of some of the complexity involved in correctly replying to registered commands.
*/
namespace RegisteredResponseType
{
	typedef enum
	{
		NotRegistered = 0x0,			//!< This type of response doesn't include the command id of the command that it's responding to.  The command id from this type is appended to an unconfirmed command id list
		Immediate = 0x1,				//!< This type of response includes the command id of all commands in any list that preceded it.  It is used when all activities that needed to occur on the any previous responses all occured within the same thread in which they arrived.
		FirstInCommandPackage = 0x2,	//!< This type of response indicates that the current command is the first in a new unconfirmed command id list.  From this point on, new commands should go in this new list.
										//!< If this type of response is detected while there is already a closed unconfirmed command id list, that list is erased and replaced by the latest list.
		SendLastCommandPackage = 0x4,	//!< This type of response indicates that the current command should include a comma separated list of commands in the latest complete unconfirmed command list.
										//!< It is used when the thread handling the activities of the commands in LastInCommandPackage is complete.
		FirstInPackAndSendLast = 0x6
	} RegisteredResponseType;
}

/*!	\brief A response, issued as the result of a command
 *
 *	Protocol responses are the object that gets returned after a command 
 *	is received.  The response can simply be an empty 200:OK, in response to a 
 *	simple command, or it can be much more complex.  Although there is functionality
 *	in place for streaming responses, Picto doesn't currently use it.  Multipart responses
 *	are also supported but not used.
 *	\author Trevor Stavropoulos, Joey Schnurr, Mark Hammond, Matt Gay
 *	\date 2009-2015
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
