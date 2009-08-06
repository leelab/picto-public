#include "ProtocolResponse.h"

#include "../globals.h"

#include <QDateTime>
#include <QBuffer>
#include <QtIOCompressor>

namespace Picto {

ProtocolResponse::ProtocolResponse(QString _protocol,
								   QString _version,
								   ProtocolResponseType::ProtocolResponseType protocolResponseType) :
	protocolResponseCode(protocolResponseType),
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

QString ProtocolResponse::getHeaders()
{
	QString headers;

	headers = QString("%1/%2 %3 %4\r\n").arg(protocol)
										.arg(version)
										.arg(protocolResponseCode)
										.arg(protocolResponseTypeStrings[protocolResponseCode]);

	headers += QString("Server: %1\r\n").arg(Picto::Names->serverAppName);
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

QByteArray ProtocolResponse::getContent()
{
	return encodedContent;
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

}; //namespace Picto
