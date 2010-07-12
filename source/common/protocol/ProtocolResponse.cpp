#include "ProtocolResponse.h"

#include "../globals.h"

#include <QDateTime>
#include <QBuffer>
#include <QtIOCompressor>
#include <QRegExp>
#include <QStringList>


namespace Picto {

ProtocolResponse::ProtocolResponse(QString _serverType,
								   QString _protocol,
								   QString _version,
								   ProtocolResponseType::ProtocolResponseType protocolResponseType) :
	protocolResponseCode(protocolResponseType),
	//serverType(_serverType),
	protocol(_protocol),
	version(_version),
	bShouldTerminateConnection(false),
	bStreamingResponse(false),
	multiPartResponseState(MultiPartResponseType::NotMultiPart),
	multiPartBoundary(""),
	timeout(1000)
	//contentEncodingType(ContentEncodingType::raw),
	//contentType("")
{
	protocolResponseTypeStrings[ProtocolResponseType::NotImplemented] = "Not Implemented";
	protocolResponseTypeStrings[ProtocolResponseType::IncompleteRequest] = "Incomplete Request";
	protocolResponseTypeStrings[ProtocolResponseType::OK] = "OK";
	protocolResponseTypeStrings[ProtocolResponseType::UninitializedResponse] = "Error - Uninitialized Response";
	protocolResponseTypeStrings[ProtocolResponseType::UnknownRequest] = "Unknown Request";
	protocolResponseTypeStrings[ProtocolResponseType::InternalServerError] = "Internal Server Error";
	protocolResponseTypeStrings[ProtocolResponseType::ConnectionTimedOut] = "Request submission time elapsed";
	protocolResponseTypeStrings[ProtocolResponseType::NotFound] = "Resource Not Found";
	protocolResponseTypeStrings[ProtocolResponseType::Unauthorized] = "Unauthorized";
	protocolResponseTypeStrings[ProtocolResponseType::BadRequest] = "Bad Request";

	contentEncodingTypeStrings[ContentEncodingType::raw] = "";
	contentEncodingTypeStrings[ContentEncodingType::gzip] = "gzip";
	contentEncodingTypeStrings[ContentEncodingType::deflate] = "deflate";

	fields["Server"] = _serverType;
}

QString ProtocolResponse::generateHeadersString()
{
	//NOTE: The approach to generate headers changed sometime around 12/1/09.  If
	//you're experiencing problems with protocolResponses this might be the cause...
	QString headers;

	headers = QString("%1/%2 %3 %4\r\n").arg(protocol)
										.arg(version)
										.arg(protocolResponseCode)
										.arg(protocolResponseTypeStrings[protocolResponseCode]);

	//Print the content length and encoding, but only if this isn't a multipart response
	if(!fields.value("Content-Type").contains("multipart", Qt::CaseInsensitive))
	{
		if(fields.contains("Content-Encoding"))
			headers+= QString("Content-Encoding: %1\r\n").arg(fields.value("Content-Encoding"));
		headers+= QString("Content-Length: %1\r\n").arg(fields.value("Content-Length","0"));
	}

	//update (or add) the date field
	fields["Date"] = QString("%1 GMT").arg(QDateTime::currentDateTime().toUTC().toString("ddd, dd MMM yyyy hh:mm:ss"));

	//print everything else that we haven't dealt with yet
	QMap<QString,QString>::const_iterator fieldsIter = fields.begin();
	while(fieldsIter != fields.end())
	{
		if(fieldsIter.key() != "Content-Encoding" &&
			fieldsIter.key() != "Content-Length")
		{
			headers += QString("%1: %2\r\n")
							.arg(fieldsIter.key())
							.arg(fieldsIter.value());
		}
		fieldsIter++;
	}
	return headers;
}

//In the case of a multipart response, after the initial headers are sent, 
//we would send this header with the next piece of content.
QString ProtocolResponse::getMultiPartHeaders()
{
	QString headers;

	headers = QString("%1\r\n").arg(multiPartBoundary);
	headers += QString("Content-Length: %1\r\n").arg(fields.value("Content-Length","0"));
	headers += QString("Content-Type: %1\r\n").arg(multiPartContentType);
	if(fields.contains("Content-Encoding"))
	{
		headers += QString("Content-Encoding: %1\r\n").arg(fields.value("Content-Encoding"));
	}

	return headers;
}

void ProtocolResponse::setMultiPartContentType(QString multiPartContentTypeString)
{
	multiPartContentType = multiPartContentTypeString;
}

QString ProtocolResponse::getMultiPartContentType()
{
	return multiPartContentType;
}

//Note that this does not affect the Content-Type field.  So, if for example
//you were starting to build a mutlipart response, and this was the first 
//piece of it, you would use the following functions:
//	response->setContentType("multipart/x-mixed-replace; boundary=--pictoboundary");
//	response->setMultiPartBoundary("--pictoboundary");

void ProtocolResponse::setMultiPartBoundary(QString multiPartBoundaryString)
{
	multiPartBoundary = multiPartBoundaryString;
}

QString ProtocolResponse::getMultiPartBoundary()
{
	return multiPartBoundary;
}

int ProtocolResponse::getResponseCode()
{
	return protocolResponseCode;
}

QString ProtocolResponse::getResponseType()
{
	return protocolResponseTypeStrings[protocolResponseCode];
}

QByteArray ProtocolResponse::getContent()
{
	return encodedContent;
}

//Yes, this is identical to getContent, but it makes things a bit clearer
QByteArray ProtocolResponse::getEncodedContent()
{
	return encodedContent;
}

QByteArray ProtocolResponse::getDecodedContent()
{
	return content;
}

void ProtocolResponse::encodeContent()
{
	if(fields.value("Content-Encoding") == "gzip")
	{
		QBuffer contentCompressedBuffer;

		QtIOCompressor compressor(&contentCompressedBuffer);
		compressor.setStreamFormat(QtIOCompressor::GzipFormat);
		compressor.open(QIODevice::WriteOnly);
		compressor.write(content);
		compressor.close();

		encodedContent = contentCompressedBuffer.buffer();	
	}
	else if(fields.value("Content-Encoding") == "deflate")
	{
		QBuffer contentCompressedBuffer;

		QtIOCompressor compressor(&contentCompressedBuffer);
		compressor.setStreamFormat(QtIOCompressor::ZlibFormat);
		compressor.open(QIODevice::WriteOnly);
		compressor.write(content);
		compressor.close();

		encodedContent = contentCompressedBuffer.buffer();
	}
	else
	{
		encodedContent = content;
	}
}

void ProtocolResponse::decodeContent()
{
	if(fields.value("Content-Encoding") == "gzip")
	{
		QBuffer compressedContent(&encodedContent);

		QtIOCompressor compressor(&compressedContent);
		compressor.setStreamFormat(QtIOCompressor::GzipFormat);
		compressor.open(QIODevice::ReadOnly);
		content = compressor.readAll();
		compressor.close();
	}
	else if(fields.value("Content-Encoding") == "deflate")
	{
		QBuffer compressedContent(&encodedContent);

		QtIOCompressor compressor(&compressedContent);
		compressor.setStreamFormat(QtIOCompressor::ZlibFormat);
		compressor.open(QIODevice::ReadOnly);
		content = compressor.readAll();
		compressor.close();
	}
	else
	{
		content = encodedContent;
	}

}

void ProtocolResponse::setContentType(QString contentTypeString)
{
	//contentType = contentTypeString;
	fields["Content-Type"]=contentTypeString;
}

void ProtocolResponse::setContentEncoding(ContentEncodingType::ContentEncodingType contentEncoding)
{
	//contentEncodingType = contentEncoding;
	fields["Content-Encoding"] = contentEncodingTypeStrings[contentEncoding];

	if(!content.isEmpty())
	{
		encodeContent();
		fields["Content-Length"] = QString("%1").arg(encodedContent.size());
	}
}

bool ProtocolResponse::hasContent()
{
	return(!content.isEmpty());
}

void ProtocolResponse::setContent(QByteArray _content)
{
	content = _content;
	encodeContent();
	fields["Content-Length"] = QString("%1").arg(encodedContent.size());
}

void ProtocolResponse::addField(QString field, QString value)
{
	fields[field] = value;
}

void ProtocolResponse::setFieldValue(QString field, QString value)
{
	fields[field] = value;
}

void ProtocolResponse::setShouldTerminateConnection(bool terminateConnection)
{
	bShouldTerminateConnection = terminateConnection;
}

bool ProtocolResponse::shouldTerminateConnection()
{
	return bShouldTerminateConnection;
}

void ProtocolResponse::setShouldStream(bool shouldStream)
{
	bStreamingResponse = shouldStream;
	if(multiPartResponseState == MultiPartResponseType::NotMultiPart)
	{
		multiPartResponseState = MultiPartResponseType::StreamingNonMultiPart;
	}
}

bool ProtocolResponse::shouldStream()
{
	return bStreamingResponse;
}

void ProtocolResponse::setMultiPart(MultiPartResponseType::MultiPartResponseType multiPartState)
{
	multiPartResponseState = multiPartState;
}

MultiPartResponseType::MultiPartResponseType ProtocolResponse::getMultiPart()
{
	return multiPartResponseState;
}

QString ProtocolResponse::getFieldValue(QString field)
{
	QString fieldValue;

	return fields.value(field,"");
}

//! Writes a response to the passed in socket.  Returns total bytes written
int ProtocolResponse::write(QAbstractSocket *socket)
{
	QByteArray response;

	//before doing anything, make sure that Content-Length is accurate
	if(fields.value("Content-Length").toInt() != content.size())
		return -1;

	if(multiPartResponseState <= MultiPartResponseType::MultiPartInitial)
	{
		QString headers = generateHeadersString();
		headers += "\r\n";
		response.append(headers);
	}

	if(multiPartResponseState == MultiPartResponseType::MultiPartPart)
	{
		response.append("\r\n");
	}

	if(multiPartResponseState >= Picto::MultiPartResponseType::MultiPartInitial)
	{
		QString multiPartHeaders = getMultiPartHeaders();
		multiPartHeaders += "\r\n";
		response.append(multiPartHeaders);
	}

	if(hasContent())
	{
		response.append(getContent());
	}
	
	int bytesWritten = socket->write(response);
	
	socket->flush();

	return bytesWritten;
}

//! Reads an incoming response from a socket and returns the bytes of content read

/*!	read() faces some interesting challenges, since it may not be easy to tell what
 *	an incoming response actually is.  For example, if an incoming response is part of
 *	a multi-part response, the response will start with a boundary string, but if we
 *	haven't set the boundary string for the response, we won't know what to 
 *	do with it.  So, the following rules should be followed when reading:
 *		1. If you know anything about the incoming response, tell the object (ie
 *		   if it's part of a multi-part response, set the multi-part response type)
 *		2. You can't assume that write will create an exact copy of the response
 *		   that was sent.  For example, the original response may have had the 
 *		   shouldTerminateConnection value set, but there is no way for us to tell if
 *		   this was the case on the receiving end.
 *	The safest way to use a response that you have just read is to look at the fields 
 *	directly
 *
 *	The timeoutSec parameter is used to determine how long we should wait for incoming data
 *	on the socket.  The default value is 0.
 */
int ProtocolResponse::read(QAbstractSocket *socket, int timeoutMs)
{
	QString currentLine;
	int contentLength=0;
	
	if(socket->bytesAvailable() <= 0)
	{
		if(!socket->waitForReadyRead(timeoutMs))
		{
			return -1;
		}
	}

	//read the first line, it might be a status line (e.g. ACQ/1.0 200 OK)
	//or it might be a CRLF followed by a multipart boundary string.  This 
	//tells us what type of response we're dealing with.  Adittionally, if the
	//first line is null, there's no data on the socket, so we should give up.
	
	currentLine = socket->readLine();

	//there is no response.
	if(currentLine.isEmpty())
	{
		return -1;
	}
	//it's a multipart part
	//NOTE: if it's the first multipart part, it will start with the boundary --....
	//Otherwise, it will start with \r\n bollowed by the boundary
	//I'm trying to leave the flexibilty for the user to not have to define the
	//boundary string

	//It's a multi part part, but not the first one
	else if(currentLine == "\r\n")
	{
		multiPartResponseState = MultiPartResponseType::MultiPartPart;

		//grab the boundary string, and check it
		currentLine = socket->readLine();
		currentLine.remove(QRegExp("[\r\n]"));

		//If the user never set the boundary string, we're just going to assume that 
		//this is a correct boundary.  However, if they did set the string, we
		//need to confirm that it is correct
		if(!multiPartBoundary.isEmpty() &&
			currentLine != multiPartBoundary)
			return -3;
	}
	//Its' the first part of a multipary part (meaning that it starts with the boundary
	//rather than LFCR)
	else if	((!multiPartBoundary.isEmpty() && currentLine.remove("\r\n") == multiPartBoundary) ||
		(multiPartBoundary.isEmpty() && currentLine.startsWith("--")))
	{
		multiPartResponseState = MultiPartResponseType::MultiPartPart;
	}
	//it's a multipart initial or, a non multipart, or something else entirely 
	//(in which case we return -2
	else
	{
		//if it turns out to be multiPartInitial, this gets changed when the 
		//Content-type field is read.
		multiPartResponseState = MultiPartResponseType::NotMultiPart;

		QStringList tokens = currentLine.split(" ");
		
		if(tokens.count() < 2)
		{
			return -2;
		}
		
		protocolResponseCode = (ProtocolResponseType::ProtocolResponseType)(tokens[1].toInt());

		int protocolVersionPosition = tokens[0].indexOf('/');
		if(protocolVersionPosition == -1)
		{
			return -2;
		}
		else
		{
			protocol = tokens[0].left(protocolVersionPosition);
			version = tokens[0].mid(protocolVersionPosition+1);
		}
	}

	//read headers
	QRegExp newLineRegExp("[\r\n]+");
	currentLine = socket->readLine();
	while(!newLineRegExp.exactMatch(currentLine))
	{
		currentLine.remove(QRegExp("[\r\n]"));
		QString fieldKey,fieldValue;

		int keyEndPosition = currentLine.indexOf(':');
		if(keyEndPosition < 0)
			return -4;

		fieldKey = currentLine.left(keyEndPosition);
		fieldValue = currentLine.mid(keyEndPosition+2);
		fields[fieldKey] = fieldValue;

		currentLine = socket->readLine();
	}

	//Interpret the headers
	contentLength = fields.value("Content-Length","0").toInt();

	if(fields.value("Content-Type").contains("multipart", Qt::CaseInsensitive))
	{
		multiPartResponseState = MultiPartResponseType::MultiPartInitial;
		int boundaryIndex = fields.value("Content-Type").indexOf("boundary=");
		if(boundaryIndex >= 0)
			multiPartBoundary = fields.value("Content-Type").mid(boundaryIndex+9);
	}

	//read content
	encodedContent = socket->read(contentLength);
	while(encodedContent.size() < contentLength && 
		(socket->bytesAvailable() > 0 || socket->waitForReadyRead(timeout)))
	{
		encodedContent.append(socket->read(contentLength-encodedContent.size()));
	}

	//decode content
	decodeContent();

	//return length of content read
	if(encodedContent.size() == contentLength)
		return encodedContent.size();
	else
		return 0-encodedContent.size();
}

}; //namespace Picto
