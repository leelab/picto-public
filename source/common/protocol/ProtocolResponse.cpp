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
	serverType(_serverType),
	protocol(_protocol),
	version(_version),
	bShouldTerminateConnection(false),
	bStreamingResponse(false),
	multiPartResponseState(MultiPartResponseType::NotMultiPart),
	contentEncodingType(ContentEncodingType::raw),
	contentType("")
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
}

QString ProtocolResponse::generateHeadersString()
{
	QString headers;

	headers = QString("%1/%2 %3 %4\r\n").arg(protocol)
										.arg(version)
										.arg(protocolResponseCode)
										.arg(protocolResponseTypeStrings[protocolResponseCode]);

	headers += QString("Server: %1\r\n").arg(serverType);
	if(!contentType.isEmpty())
	{
		headers += QString("Content-Type: %1\r\n").arg(contentType);
	}
	if(contentEncodingType != ContentEncodingType::raw && multiPartResponseState < MultiPartResponseType::MultiPartInitial)
	{
		headers += QString("Content-Encoding: %1\r\n").arg(contentEncodingTypeStrings[contentEncodingType]);
	}
	if(multiPartResponseState < MultiPartResponseType::MultiPartInitial)
	{
		headers += QString("Content-Length: %1\r\n").arg(encodedContent.size());
	}
	headers += QString("Date: %1 GMT\r\n").arg(QDateTime::currentDateTime().toUTC().toString("ddd, dd MMM yyyy hh:mm:ss"));

	for(std::map<QString,QString>::const_iterator fieldsIter = fields.begin(); fieldsIter != fields.end(); ++fieldsIter)
	{
		headers += QString("%1: %2\r\n").arg(fieldsIter->first).arg(fieldsIter->second);
	}
	return headers;
}

QString ProtocolResponse::getMultiPartHeaders()
{
	QString headers;

	headers = QString("%1\r\n").arg(multiPartBoundary);
	headers += QString("Content-Length: %1\r\n").arg(encodedContent.size());
	headers += QString("Content-Type: %1\r\n").arg(multiPartContentType);
	if(contentEncodingType != ContentEncodingType::raw)
	{
		headers += QString("Content-Encoding: %1\r\n").arg(contentEncodingTypeStrings[contentEncodingType]);
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

QByteArray ProtocolResponse::getDecodedContent()
{
	return content;
}

void ProtocolResponse::encodeContent()
{
	switch(contentEncodingType)
	{
		case ContentEncodingType::gzip:
		{
			QBuffer contentCompressedBuffer;

			QtIOCompressor compressor(&contentCompressedBuffer);
			compressor.setStreamFormat(QtIOCompressor::GzipFormat);
			compressor.open(QIODevice::WriteOnly);
			compressor.write(content);
			compressor.close();

			encodedContent = contentCompressedBuffer.buffer();
			break;
		}
		case ContentEncodingType::deflate:
		{
			QBuffer contentCompressedBuffer;

			QtIOCompressor compressor(&contentCompressedBuffer);
			compressor.setStreamFormat(QtIOCompressor::ZlibFormat);
			compressor.open(QIODevice::WriteOnly);
			compressor.write(content);
			compressor.close();

			encodedContent = contentCompressedBuffer.buffer();
			break;
		}
		case ContentEncodingType::raw:
		default:
		{
			encodedContent = content;
			break;
		}
	}
}

void ProtocolResponse::decodeContent()
{
	switch(contentEncodingType)
	{
		case ContentEncodingType::gzip:
		{
			QBuffer compressedContent(&encodedContent);

			QtIOCompressor compressor(&compressedContent);
			compressor.setStreamFormat(QtIOCompressor::GzipFormat);
			compressor.open(QIODevice::ReadOnly);
			content = compressor.readAll();
			compressor.close();
			break;
		}
		case ContentEncodingType::deflate:
		{
			QBuffer compressedContent(&encodedContent);

			QtIOCompressor compressor(&compressedContent);
			compressor.setStreamFormat(QtIOCompressor::ZlibFormat);
			compressor.open(QIODevice::ReadOnly);
			content = compressor.readAll();
			compressor.close();
			break;
		}
		case ContentEncodingType::raw:
		default:
		{
			content = encodedContent;
			break;
		}
	}

}

void ProtocolResponse::setContentType(QString contentTypeString)
{
	contentType = contentTypeString;
}

void ProtocolResponse::setContentEncoding(ContentEncodingType::ContentEncodingType contentEncoding)
{
	contentEncodingType = contentEncoding;

	if(!content.isEmpty())
	{
		encodeContent();
	}
}

bool ProtocolResponse::hasContent()
{
	return(!content.isEmpty());
}

void ProtocolResponse::setContent(QByteArray _content)
{
	content = _content;
	if(!content.isEmpty())
		encodeContent();
}

void ProtocolResponse::addField(QString field, QString value)
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

bool ProtocolResponse::shouldStream()
{
	return bStreamingResponse;
}

void ProtocolResponse::setShouldStream(bool shouldStream)
{
	bStreamingResponse = shouldStream;
	if(multiPartResponseState == MultiPartResponseType::NotMultiPart)
	{
		multiPartResponseState = MultiPartResponseType::StreamingNonMultiPart;
	}
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

	std::map<QString,QString>::const_iterator fieldIter = fields.find(field);

	if(fieldIter != fields.end())
	{
		fieldValue = fieldIter->second;
	}

	return fieldValue;
}

int ProtocolResponse::read(QAbstractSocket *socket)
{
	QString commandHeader;
	QString currentLine;
	int contentLength=0;
	
	if(!socket->waitForReadyRead(1000))
	{
		return -1;
	}

	//initialize everything
	protocolResponseCode = ProtocolResponseType::UninitializedResponse;
	protocol = "";
	version = "0.0";
	contentEncodingType = ContentEncodingType::raw;
	contentType = "";

	
	
	//read status line (e.g. ACQ/1.0 200 OK)
	currentLine = socket->readLine();
	
	QStringList tokens = currentLine.split(QRegExp("[ ][ ]*"));
	
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

	//read headers
	QRegExp newLineRegExp("[\r\n]+");
	currentLine = socket->readLine();
	while(!newLineRegExp.exactMatch(currentLine))
	{

		currentLine.remove(QRegExp("[\r\n]"));
		QString fieldKey,fieldValue;

		int fieldEndPosition = currentLine.indexOf(':');		
		fieldKey = currentLine.left(fieldEndPosition);
		fieldValue = currentLine.mid(fieldEndPosition+2);

		if(!fieldKey.compare("Content-Type",Qt::CaseInsensitive))
		{
			setContentType(fieldValue);
		}
		else if(!fieldKey.compare("Content-Encoding",Qt::CaseInsensitive))
		{
			//this is ugly, but I don't see a better way to get the 
			//encoding type enum value, since we can't find() on a value...
			std::map<ContentEncodingType::ContentEncodingType,QString>::const_iterator encodingIter = contentEncodingTypeStrings.begin();
			while(encodingIter != contentEncodingTypeStrings.end())
			{
				if(!QString::compare(fieldValue,encodingIter->second,Qt::CaseInsensitive))
				{
					setContentEncoding(encodingIter->first);
					break;
				}
				encodingIter ++;
			}
			if(encodingIter == contentEncodingTypeStrings.end())
			{
				return -2;
			}
		}
		else if(!fieldKey.compare("Content-Length",Qt::CaseInsensitive))
		{
			contentLength = fieldValue.toInt();
		}
		else if(!fieldKey.compare("Server",Qt::CaseInsensitive))
		{
			serverType = fieldValue;
			addField(fieldKey,fieldValue);
		}
		else
		{
			addField(fieldKey,fieldValue);
		}

		currentLine = socket->readLine();
	}

	//read content
	encodedContent = socket->read(contentLength);
	while(encodedContent.size() < contentLength && socket->bytesAvailable() > 0)
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
